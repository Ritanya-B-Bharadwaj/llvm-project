//===- XtensaAsmPrinter.cpp Xtensa LLVM Assembly Printer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format Xtensa assembly language.
//
//===----------------------------------------------------------------------===//

#include "XtensaAsmPrinter.h"
#include "MCTargetDesc/XtensaInstPrinter.h"
#include "MCTargetDesc/XtensaMCAsmInfo.h"
#include "MCTargetDesc/XtensaTargetStreamer.h"
#include "TargetInfo/XtensaTargetInfo.h"
#include "XtensaConstantPoolValue.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

static Xtensa::Specifier
getModifierSpecifier(XtensaCP::XtensaCPModifier Modifier) {
  switch (Modifier) {
  case XtensaCP::no_modifier:
    return Xtensa::S_None;
  case XtensaCP::TPOFF:
    return Xtensa::S_TPOFF;
  }
  report_fatal_error("Invalid XtensaCPModifier!");
}

void XtensaAsmPrinter::emitInstruction(const MachineInstr *MI) {
  unsigned Opc = MI->getOpcode();

  switch (Opc) {
  case Xtensa::BR_JT:
    EmitToStreamer(
        *OutStreamer,
        MCInstBuilder(Xtensa::JX).addReg(MI->getOperand(0).getReg()));
    return;
  default:
    MCInst LoweredMI;
    lowerToMCInst(MI, LoweredMI);
    EmitToStreamer(*OutStreamer, LoweredMI);
    return;
  }
}

void XtensaAsmPrinter::emitMachineConstantPoolValue(
    MachineConstantPoolValue *MCPV) {
  XtensaConstantPoolValue *XtensaCPV =
      static_cast<XtensaConstantPoolValue *>(MCPV);
  MCSymbol *MCSym;

  if (XtensaCPV->isBlockAddress()) {
    const BlockAddress *BA =
        cast<XtensaConstantPoolConstant>(XtensaCPV)->getBlockAddress();
    MCSym = GetBlockAddressSymbol(BA);
  } else if (XtensaCPV->isMachineBasicBlock()) {
    const MachineBasicBlock *MBB =
        cast<XtensaConstantPoolMBB>(XtensaCPV)->getMBB();
    MCSym = MBB->getSymbol();
  } else if (XtensaCPV->isJumpTable()) {
    unsigned Idx = cast<XtensaConstantPoolJumpTable>(XtensaCPV)->getIndex();
    MCSym = this->GetJTISymbol(Idx, false);
  } else {
    assert(XtensaCPV->isExtSymbol() && "unrecognized constant pool value");
    XtensaConstantPoolSymbol *XtensaSym =
        cast<XtensaConstantPoolSymbol>(XtensaCPV);
    const char *SymName = XtensaSym->getSymbol();

    if (XtensaSym->isPrivateLinkage()) {
      const DataLayout &DL = getDataLayout();
      MCSym = OutContext.getOrCreateSymbol(Twine(DL.getPrivateGlobalPrefix()) +
                                           SymName);
    } else {
      MCSym = OutContext.getOrCreateSymbol(SymName);
    }
  }

  MCSymbol *LblSym = GetCPISymbol(XtensaCPV->getLabelId());
  auto *TS =
      static_cast<XtensaTargetStreamer *>(OutStreamer->getTargetStreamer());
  auto Spec = getModifierSpecifier(XtensaCPV->getModifier());

  if (XtensaCPV->getModifier() != XtensaCP::no_modifier) {
    std::string SymName(MCSym->getName());
    StringRef Modifier = XtensaCPV->getModifierText();
    SymName += Modifier;
    MCSym = OutContext.getOrCreateSymbol(SymName);
  }

  const MCExpr *Expr = MCSymbolRefExpr::create(MCSym, Spec, OutContext);
  TS->emitLiteral(LblSym, Expr, false);
}

void XtensaAsmPrinter::emitMachineConstantPoolEntry(
    const MachineConstantPoolEntry &CPE, int i) {
  if (CPE.isMachineConstantPoolEntry()) {
    XtensaConstantPoolValue *XtensaCPV =
        static_cast<XtensaConstantPoolValue *>(CPE.Val.MachineCPVal);
    XtensaCPV->setLabelId(i);
    emitMachineConstantPoolValue(CPE.Val.MachineCPVal);
  } else {
    MCSymbol *LblSym = GetCPISymbol(i);
    auto *TS =
        static_cast<XtensaTargetStreamer *>(OutStreamer->getTargetStreamer());
    const Constant *C = CPE.Val.ConstVal;
    const MCExpr *Value = nullptr;

    Type *Ty = C->getType();
    if (const auto *CFP = dyn_cast<ConstantFP>(C)) {
      Value = MCConstantExpr::create(
          CFP->getValueAPF().bitcastToAPInt().getSExtValue(), OutContext);
    } else if (const auto *CI = dyn_cast<ConstantInt>(C)) {
      Value = MCConstantExpr::create(CI->getValue().getSExtValue(), OutContext);
    } else if (isa<PointerType>(Ty)) {
      Value = lowerConstant(C);
    } else {
      llvm_unreachable("unexpected constant pool entry type");
    }

    TS->emitLiteral(LblSym, Value, false);
  }
}

// EmitConstantPool - Print to the current output stream assembly
// representations of the constants in the constant pool MCP. This is
// used to print out constants which have been "spilled to memory" by
// the code generator.
void XtensaAsmPrinter::emitConstantPool() {
  const Function &F = MF->getFunction();
  const MachineConstantPool *MCP = MF->getConstantPool();
  const std::vector<MachineConstantPoolEntry> &CP = MCP->getConstants();
  if (CP.empty())
    return;

  OutStreamer->pushSection();

  auto *TS =
      static_cast<XtensaTargetStreamer *>(OutStreamer->getTargetStreamer());
  MCSection *CS = getObjFileLowering().SectionForGlobal(&F, TM);
  TS->startLiteralSection(CS);

  int CPIdx = 0;
  for (const MachineConstantPoolEntry &CPE : CP) {
    emitMachineConstantPoolEntry(CPE, CPIdx++);
  }

  OutStreamer->popSection();
}

void XtensaAsmPrinter::printOperand(const MachineInstr *MI, int OpNo,
                                    raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(OpNo);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
  case MachineOperand::MO_Immediate: {
    MCOperand MC = lowerOperand(MI->getOperand(OpNo));
    XtensaInstPrinter::printOperand(MC, O);
    break;
  }
  default:
    llvm_unreachable("unknown operand type");
  }
}

bool XtensaAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                       const char *ExtraCode, raw_ostream &O) {
  // Print the operand if there is no operand modifier.
  if (!ExtraCode || !ExtraCode[0]) {
    printOperand(MI, OpNo, O);
    return false;
  }

  // Fallback to the default implementation.
  return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);
}

bool XtensaAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                             unsigned OpNo,
                                             const char *ExtraCode,
                                             raw_ostream &OS) {
  if (ExtraCode && ExtraCode[0])
    return true; // Unknown modifier.

  assert(OpNo + 1 < MI->getNumOperands() && "Insufficient operands");

  const MachineOperand &Base = MI->getOperand(OpNo);
  const MachineOperand &Offset = MI->getOperand(OpNo + 1);

  assert(Base.isReg() &&
         "Unexpected base pointer for inline asm memory operand.");
  assert(Offset.isImm() && "Unexpected offset for inline asm memory operand.");

  OS << XtensaInstPrinter::getRegisterName(Base.getReg());
  OS << ", ";
  OS << Offset.getImm();

  return false;
}

MCSymbol *
XtensaAsmPrinter::GetConstantPoolIndexSymbol(const MachineOperand &MO) const {
  // Create a symbol for the name.
  return GetCPISymbol(MO.getIndex());
}

MCSymbol *XtensaAsmPrinter::GetJumpTableSymbol(const MachineOperand &MO) const {
  return GetJTISymbol(MO.getIndex());
}

MCOperand
XtensaAsmPrinter::LowerSymbolOperand(const MachineOperand &MO,
                                     MachineOperand::MachineOperandType MOTy,
                                     unsigned Offset) const {
  const MCSymbol *Symbol;
  switch (MOTy) {
  case MachineOperand::MO_GlobalAddress:
    Symbol = getSymbol(MO.getGlobal());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    Symbol = MO.getMBB()->getSymbol();
    break;
  case MachineOperand::MO_BlockAddress:
    Symbol = GetBlockAddressSymbol(MO.getBlockAddress());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_ExternalSymbol:
    Symbol = GetExternalSymbolSymbol(MO.getSymbolName());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_JumpTableIndex:
    Symbol = GetJumpTableSymbol(MO);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    Symbol = GetConstantPoolIndexSymbol(MO);
    Offset += MO.getOffset();
    break;
  default:
    report_fatal_error("<unknown operand type>");
  }

  const MCExpr *ME = MCSymbolRefExpr::create(Symbol, OutContext);
  if (Offset) {
    // Assume offset is never negative.
    assert(Offset > 0);

    const MCConstantExpr *OffsetExpr =
        MCConstantExpr::create(Offset, OutContext);
    ME = MCBinaryExpr::createAdd(ME, OffsetExpr, OutContext);
  }

  return MCOperand::createExpr(ME);
}

MCOperand XtensaAsmPrinter::lowerOperand(const MachineOperand &MO,
                                         unsigned Offset) const {
  MachineOperand::MachineOperandType MOTy = MO.getType();

  switch (MOTy) {
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      break;
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm() + Offset);
  case MachineOperand::MO_RegisterMask:
    break;
  case MachineOperand::MO_GlobalAddress:
  case MachineOperand::MO_MachineBasicBlock:
  case MachineOperand::MO_BlockAddress:
  case MachineOperand::MO_ExternalSymbol:
  case MachineOperand::MO_JumpTableIndex:
  case MachineOperand::MO_ConstantPoolIndex:
    return LowerSymbolOperand(MO, MOTy, Offset);
  default:
    report_fatal_error("unknown operand type");
  }

  return MCOperand();
}

void XtensaAsmPrinter::lowerToMCInst(const MachineInstr *MI,
                                     MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);
    MCOperand MCOp = lowerOperand(MO);

    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}

char XtensaAsmPrinter::ID = 0;

INITIALIZE_PASS(XtensaAsmPrinter, "xtensa-asm-printer",
                "Xtensa Assembly Printer", false, false)

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeXtensaAsmPrinter() {
  RegisterAsmPrinter<XtensaAsmPrinter> A(getTheXtensaTarget());
}
