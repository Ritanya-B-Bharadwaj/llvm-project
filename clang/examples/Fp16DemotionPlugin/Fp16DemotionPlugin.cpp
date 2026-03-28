#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Type.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/Decl.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/Lex/Lexer.h"
#include <cmath>
#include <unordered_set>
#include <vector>
#include <algorithm>

using namespace clang;

namespace {

// Constants for FP16 range
const float FP16_MAX = 65504.0f;
const float FP16_MIN = -65504.0f;
const float FP16_MIN_POSITIVE = 6.103515625e-5f; // 2^-14
const float SMALL_DIVISION_THRESHOLD = 0.001f;   // Threshold for "small number" in division

class Fp16TypeChecker {
public:
    static bool isValueInFp16Range(float Value) {
        if (std::isnan(Value) || std::isinf(Value))
            return false;
            
        float AbsValue = std::fabs(Value);
        if (AbsValue > FP16_MAX)
            return false;
        if (AbsValue > 0 && AbsValue < FP16_MIN_POSITIVE)
            return false;
            
        return true;
    }
    
    // Overload: returns false and sets reason if not demotable
    static bool canDemoteFloatExpr(const Expr* E, ASTContext* Context, std::string* Reason = nullptr) {
        if (!E || !Context)
            return false;
            
        E = E->IgnoreParenCasts();
            
        // Handle literal values
        if (const auto* FL = dyn_cast<FloatingLiteral>(E)) {
            llvm::APFloat Val = FL->getValue();
            llvm::SmallString<16> Str;
            Val.toString(Str);
            float FloatVal;
            if (sscanf(Str.c_str(), "%f", &FloatVal) == 1) {
                return isValueInFp16Range(FloatVal);
            }
            return false;
        }
        
        // Handle variables
        if (const auto* DRE = dyn_cast<DeclRefExpr>(E)) {
            if (const auto* VD = dyn_cast<VarDecl>(DRE->getDecl())) {
                QualType T = VD->getType();
                return canDemoteType(T, Context);
            }
            return false;
        }
        
        // Handle binary operations
        if (const auto* BO = dyn_cast<BinaryOperator>(E)) {
            bool CanDemoteLHS = canDemoteFloatExpr(BO->getLHS(), Context, Reason);
            bool CanDemoteRHS = canDemoteFloatExpr(BO->getRHS(), Context, Reason);
            
            // For division, check for very small denominators
            if (BO->getOpcode() == BO_Div) {
                if (const auto* RHSLit = dyn_cast<FloatingLiteral>(BO->getRHS()->IgnoreParenCasts())) {
                    llvm::APFloat Val = RHSLit->getValue();
                    llvm::SmallString<16> Str;
                    Val.toString(Str);
                    float FloatVal;
                    if (sscanf(Str.c_str(), "%f", &FloatVal) == 1 && 
                        std::fabs(FloatVal) < SMALL_DIVISION_THRESHOLD) {
                        if (Reason) *Reason = "division by small number";
                        return false;  // Avoid division by very small numbers
                    }
                }
            }
            
            return CanDemoteLHS && CanDemoteRHS;
        }
        
        // Handle unary operations
        if (const auto* UO = dyn_cast<UnaryOperator>(E)) {
            return canDemoteFloatExpr(UO->getSubExpr(), Context, Reason);
        }
        
        // Handle function calls - conservative approach
        if (isa<CallExpr>(E)) {
            // Don't demote variables used in function calls unless we can analyze the function
            if (Reason) *Reason = "used in function call";
            return false;
        }
        
        // Conservatively handle other expression types
        return false;
    }
    
    static bool canDemoteType(QualType T, ASTContext* Context) {
        if (!Context || T.isNull())
            return false;
            
        // Only handle float types
        if (!T->isSpecificBuiltinType(BuiltinType::Float))
            return false;
            
        // Don't demote volatile or atomic types
        if (T.isVolatileQualified() || T->isAtomicType())
            return false;
            
        return true;
    }
};

struct VarTransform {
    VarDecl* Decl;
    SourceLocation Begin;
    size_t Length;

    VarTransform(VarDecl* D, SourceLocation B, size_t L)
        : Decl(D), Begin(B), Length(L) {}
};

class Fp16DemotionVisitor : public RecursiveASTVisitor<Fp16DemotionVisitor> {
public:
    explicit Fp16DemotionVisitor(ASTContext *Context, Rewriter &R)
        : Context(Context), TheRewriter(R) {}

    bool VisitVarDecl(VarDecl *VD) {
        if (!VD || !Context)
            return true;  // Continue traversal
            
        // Only process variables in the main file
        SourceManager &SM = Context->getSourceManager();
        if (!SM.isInMainFile(VD->getLocation()))
            return true;
            
        // Check if it's a float variable
        QualType T = VD->getType();
        if (!Fp16TypeChecker::canDemoteType(T, Context))
            return true;
            
        // Skip if already processed
        if (ProcessedDecls.count(VD))
            return true;
            
        ProcessedDecls.insert(VD);
            
        // Collect all references to this variable
        bool IsSafe = true;
        std::unordered_set<const Expr*> Visited;
        
        // Check variable initialization
        if (const Expr* Init = VD->getInit()) {
            std::string reason;
            if (!checkUseExpr(Init, Visited, &reason)) {
                if (reason.empty()) {
                    reason = "initialization value out of __fp16 range";
                }
                emitRangeDiagnostic(VD, reason);
                IsSafe = false;
            }
        }
        
        // If safe, collect the transformation
        if (IsSafe) {
            if (TypeSourceInfo *TSI = VD->getTypeSourceInfo()) {
                TypeLoc TL = TSI->getTypeLoc();
                SourceLocation Begin = TL.getBeginLoc();
                
                if (Begin.isValid()) {
                    bool Invalid = false;
                    const char* StartPtr = Context->getSourceManager().getCharacterData(Begin, &Invalid);
                    
                    if (!Invalid && StartPtr) {
                        size_t Len = strlen("float");
                        std::string TokenText(StartPtr, Len);
                        
                        if (TokenText == "float") {
                            Transformations.emplace_back(VD, Begin, Len);
                        }
                    }
                }
            }
        }
        
        return true;
    }
    
    void applyTransformations() {
        if (!Context || Transformations.empty())
            return;
            
        // Sort transformations in reverse order of source location
        std::sort(Transformations.begin(), Transformations.end(),
            [](const VarTransform& A, const VarTransform& B) {
                return A.Begin.getRawEncoding() > B.Begin.getRawEncoding();
            });
            
        SourceManager &SM = Context->getSourceManager();
        const LangOptions &LangOpts = Context->getLangOpts();
            
        // Apply transformations in reverse order
        for (const auto& Transform : Transformations) {
            if (!Transform.Begin.isValid()) 
                continue;
            // Skip if in a macro
            if (SM.isMacroBodyExpansion(Transform.Begin) || SM.isMacroArgExpansion(Transform.Begin))
                continue;
            // Get the token at this location
            Token Tok;
            if (Lexer::getRawToken(Transform.Begin, Tok, SM, LangOpts, true))
                continue; // Could not get token
            if (Tok.isNot(tok::identifier))
                continue;
            std::string TokSpelling = Lexer::getSpelling(Tok, SM, LangOpts);
            if (TokSpelling != "float")
                continue;
            // Replace the text
            TheRewriter.ReplaceText(Transform.Begin, TokSpelling.size(), "__fp16");
            emitDemotionDiagnostic(Transform.Decl);
        }
    }

private:
    bool checkUseExpr(const Expr* E, std::unordered_set<const Expr*>& Visited, std::string* Reason = nullptr) {
        if (!E || !Context)
            return true;
            
        if (Visited.count(E))
            return true;  // Already checked this expression
            
        Visited.insert(E);
        E = E->IgnoreParenCasts();
        
        return Fp16TypeChecker::canDemoteFloatExpr(E, Context, Reason);
    }
    
    void emitDemotionDiagnostic(VarDecl* Decl) {
        if (!Context || !Decl)
            return;
            
        DiagnosticsEngine &DE = Context->getDiagnostics();
        unsigned ID = DE.getCustomDiagID(DiagnosticsEngine::Warning,
            "Variable %0 has been safely demoted from float to __fp16");
        auto DB = DE.Report(Decl->getLocation(), ID);
        DB.AddString(Decl->getName());
    }
    
    void emitRangeDiagnostic(VarDecl* Decl, StringRef Reason) {
        if (!Context || !Decl)
            return;
            
        DiagnosticsEngine &DE = Context->getDiagnostics();
        unsigned ID = DE.getCustomDiagID(DiagnosticsEngine::Warning,
            "Cannot demote variable %0 to __fp16: %1");
        auto DB = DE.Report(Decl->getLocation(), ID);
        DB.AddString(Decl->getName());
        DB.AddString(Reason);
    }

    ASTContext *Context;
    Rewriter &TheRewriter;
    std::unordered_set<const VarDecl*> ProcessedDecls;
    std::vector<VarTransform> Transformations;
};

class Fp16DemotionASTConsumer : public ASTConsumer {
public:
    explicit Fp16DemotionASTConsumer(ASTContext *Context, Rewriter &R)
        : Visitor(Context, R) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        // Traverse the AST to collect transformations
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        
        // Apply all transformations in reverse order
        Visitor.applyTransformations();
    }

private:
    Fp16DemotionVisitor Visitor;
};

class Fp16DemotionPluginAction : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                  StringRef file) override {
        if (!EnableFp16Demotion) {
            llvm::errs() << "Warning: FP16 demotion is not enabled. Use -fprecision-demote=fp16 to enable.\n";
            return nullptr;
        }
        
        TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<Fp16DemotionASTConsumer>(&CI.getASTContext(),
                                                        TheRewriter);
    }

    bool ParseArgs(const CompilerInstance &CI,
                  const std::vector<std::string>& args) override {
        llvm::errs() << "FP16 plugin loaded\n"; 
        for (const auto &Arg : args) {
            if (Arg == "-fprecision-demote=fp16") {
                EnableFp16Demotion = true;
                llvm::outs() << "FP16 demotion enabled\n";
            }
        }
        return true;
    }

    ActionType getActionType() override {
        return PluginASTAction::AddBeforeMainAction;
    }

private:
    Rewriter TheRewriter;
    bool EnableFp16Demotion = false;
};

} // namespace

static FrontendPluginRegistry::Add<Fp16DemotionPluginAction>
X("fp16-demotion", "Demote float variables to __fp16 where safe");
