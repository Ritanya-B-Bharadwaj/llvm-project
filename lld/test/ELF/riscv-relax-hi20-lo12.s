# REQUIRES: riscv
# RUN: rm -rf %t && split-file %s %t && cd %t

# RUN: llvm-mc -filetype=obj -triple=riscv32-unknown-elf -mattr=+relax a.s -o rv32.o
# RUN: llvm-mc -filetype=obj -triple=riscv64-unknown-elf -mattr=+relax a.s -o rv64.o

# RUN: ld.lld --relax-gp --undefined=__global_pointer$ --defsym baz=420 rv32.o lds -o rv32
# RUN: ld.lld --relax-gp --undefined=__global_pointer$ --defsym baz=420 rv64.o lds -o rv64
# RUN: llvm-objdump -td -M no-aliases --no-show-raw-insn rv32 | FileCheck %s
# RUN: llvm-objdump -td -M no-aliases --no-show-raw-insn rv64 | FileCheck %s

# CHECK: 00000040 l       .text {{0*}}0 a

# CHECK-NOT:  lui
# CHECK:      addi    a0, gp, -0x800
# CHECK-NEXT: lw      a0, -0x800(gp)
# CHECK-NEXT: sw      a0, -0x800(gp)
# CHECK-NOT:  lui
# CHECK-NEXT: addi    a0, gp, 0x7ff
# CHECK-NEXT: lb      a0, 0x7ff(gp)
# CHECK-NEXT: sb      a0, 0x7ff(gp)
# CHECK-NEXT: lui     a0, 0x201
# CHECK-NEXT: addi    a0, a0, 0x0
# CHECK-NEXT: lw      a0, 0x0(a0)
# CHECK-NEXT: sw      a0, 0x0(a0)
# CHECK-NOT:  lui
# CHECK:      addi    a0, zero, 0x0
# CHECK-NEXT: lw      a0, 0x0(zero)
# CHECK-NEXT: sw      a0, 0x0(zero)
# CHECK-NOT:  lui
# CHECK:      addi    a0, zero, 0x1a4
# CHECK-NEXT: lw      a0, 0x1a4(zero)
# CHECK-NEXT: sw      a0, 0x1a4(zero)
# CHECK-EMPTY:
# CHECK-NEXT: <a>:
# CHECK-NEXT: addi a0, a0, 0x1

#--- a.s
.global _start
_start:
  lui a0, %hi(foo)
  addi a0, a0, %lo(foo)
  lw a0, %lo(foo)(a0)
  sw a0, %lo(foo)(a0)
  lui a0, %hi(bar)
  addi a0, a0, %lo(bar)
  lb a0, %lo(bar)(a0)
  sb a0, %lo(bar)(a0)
  lui a0, %hi(norelax)
  addi a0, a0, %lo(norelax)
  lw a0, %lo(norelax)(a0)
  sw a0, %lo(norelax)(a0)
  lui a0, %hi(undefined_weak)
  addi a0, a0, %lo(undefined_weak)
  lw a0, %lo(undefined_weak)(a0)
  sw a0, %lo(undefined_weak)(a0)
  lui a0, %hi(baz)
  addi a0, a0, %lo(baz)
  lw a0, %lo(baz)(a0)
  sw a0, %lo(baz)(a0)
a:
  addi a0, a0, 1

.section .sdata,"aw"
foo:
  .word 0
  .space 4091
bar:
  .byte 0
norelax:
  .word 0
.weak undefined_weak

#--- lds
SECTIONS {
  .text : {*(.text) }
  .sdata 0x200000 : { }
}
