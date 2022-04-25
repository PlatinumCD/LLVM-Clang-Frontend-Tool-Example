// Clang Headers
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h" 
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// LLVM Headers
#include "llvm/Support/CommandLine.h"

// Extra Headers
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//#include <filesystem>

// Namespaces
using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;

// Frontend Tool
namespace MiniInstrumentor {

    /*  RecursiveASTVisitor  */
    class Visitor : public RecursiveASTVisitor<Visitor> {
        public:
            Visitor(Rewriter &rewriter) : rewriter(rewriter) {} 

            bool VisitFunctionDecl(FunctionDecl* decl) {
                if (decl->hasBody()) {

                    stringstream openingCaption;
                    DeclarationName declName = decl->getNameInfo().getName();
                    string funcName = declName.getAsString();
                    string funcDeclReturnType = decl->getReturnType().getAsString();
                    openingCaption << "\n\tTAU_PROFILE_TIMER(tautimer, \"" << funcDeclReturnType << " "; 
                    openingCaption << funcName << "(";

                    for (unsigned i = 0; i < decl->getNumParams(); i++) {
                        ParmVarDecl* parmDecl = decl->getParamDecl(i);
                        string parmDeclName = parmDecl->getNameAsString();
                        string parmDeclType = parmDecl->getType().getAsString(); 
                        openingCaption << parmDeclType;
                        if (i != decl->getNumParams() - 1) {
                            openingCaption << ", ";
                        }   
                    }   
    
                    string mainString("main");
                    if (mainString.compare(funcName) == 0) {
                        openingCaption << ")\", \" \", TAU_DEFAULT);\n";
                        openingCaption << "\tTAU_INIT(&argc, &argv);\n";
                        openingCaption << "\tTAU_PROFILE_SET_NODE(0);\n";
                        openingCaption << "\tTAU_PROFILE_START(tautimer);\n";
                    } else {
                        openingCaption << ")\", \" \", TAU_USER);\n\tTAU_PROFILE_START(tautimer);\n";
                    }   

                    Stmt *body = decl->getBody();
                    auto compound = dyn_cast<CompoundStmt>(body);

                    SourceLocation openingLocation = compound->getLBracLoc().getLocWithOffset(1);
                    rewriter.InsertText(openingLocation, openingCaption.str(), false, true);

                    SourceLocation closingLocation;
                    string retString("ReturnStmt");

                    // Iterate through function children
                    for (auto *child : compound->body()) {
                        string className(child->getStmtClassName());
                        // If child is a return statement
                        if (retString.compare(className) == 0) {
                            // Iterate through return statement children
                            for (auto *exprChild : child->children()) {
                                // If return statement children is an operator or function call
                                if (isa<BinaryOperator>(exprChild) || isa<CallExpr>(exprChild)) {

                                    // Get the return statement logic
                                    CharSourceRange exprRange;
                                    exprRange.setBegin(exprChild->getBeginLoc());
                                    exprRange.setEnd(exprChild->getEndLoc().getLocWithOffset(2));
                                    string returnedExpr = Lexer::getSourceText(exprRange, rewriter.getSourceMgr(), LangOptions(), 0).str();
                                    
                                    // Return the new variable
                                    string returnExpr = "\treturn __clang_ret;\n";
                                    rewriter.InsertTextBefore(child->getBeginLoc(), returnExpr);

                                    // Insert tracer
                                    stringstream closingCaption;
                                    closingCaption << "\n\tTAU_PROFILE_STOP(tautimer);\n";
                                    rewriter.InsertTextBefore(child->getBeginLoc(), closingCaption.str());

                                    // Make a new variable
                                    string updatedReturnExpr = funcDeclReturnType + " __clang_ret = " + returnedExpr + "\n";
                                    rewriter.InsertTextBefore(child->getBeginLoc(), updatedReturnExpr);

                                    // Remove old return statement
                                    int length = child->getEndLoc().getRawEncoding() - child->getBeginLoc().getRawEncoding();
                                    rewriter.RemoveText(child->getBeginLoc(), length + 3);
                                    return true;
                                }
                            }

                            // Get location of return statement
                            closingLocation = child->getBeginLoc();   

                            // Insert tracer 
                            stringstream closingCaption;
                            closingCaption << "\n\tTAU_PROFILE_STOP(tautimer);\n";
                            rewriter.InsertTextBefore(closingLocation, closingCaption.str());
                            return true;
                        }
                    }

                    // If no return statement exists, set location at end of function 
                    closingLocation = compound->getRBracLoc();
                    
                    // Insert tracer
                    stringstream closingCaption;
                    closingCaption << "\n\tTAU_PROFILE_STOP(tautimer);\n";
                    rewriter.InsertTextBefore(closingLocation, closingCaption.str());
                }

                return true;
            }

        private:
            Rewriter &rewriter;
     };


    /*   ASTConsumer   */
    class Consumer : public ASTConsumer {
        public:
            Consumer(Rewriter &rewriter) : visitor(rewriter) {}

            void HandleTranslationUnit(clang::ASTContext &Context) {
              visitor.TraverseDecl(Context.getTranslationUnitDecl());
            }

        private:
            Visitor visitor; 
    };


    /*   Frontend Action    */
    class Action : public ASTFrontendAction {
        public:
            unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& Compiler, StringRef inFile) override {
                fileName = inFile.str();
                rewriter.setSourceMgr(Compiler.getSourceManager(), Compiler.getLangOpts());
                return make_unique<Consumer>(rewriter);
            }

            void EndSourceFileAction() override {

                FileID fileid = rewriter.getSourceMgr().getMainFileID();
                SourceLocation includeLocation = rewriter.getSourceMgr().getLocForStartOfFile(fileid);
                string includeStatement("#include <TAU.h>\n");                
                rewriter.InsertText(includeLocation, includeStatement);

                const RewriteBuffer *RewriteBuf =
                    rewriter.getRewriteBufferFor(getCompilerInstance().getSourceManager().getMainFileID());
 
                ofstream instrFile;
                instrFile.open("example.instr.c");
                instrFile << string(RewriteBuf->begin(), RewriteBuf->end());
                instrFile.close();
            }

        private:
            string fileName;
            Rewriter rewriter;
    };
}

  
int main(int argc, const char* argv[]) {
    cl::OptionCategory ToolCategory("Mini Instrumentor");
    Expected<CommonOptionsParser> OptionsParser = CommonOptionsParser::create(argc, argv, ToolCategory);

    if (!OptionsParser) {
       return 1; 
    }

    ClangTool Tool(OptionsParser->getCompilations(),
                   OptionsParser->getSourcePathList());

    if (argc > 2) {
        cout << "Only takes 1 argument" << endl;
        exit(1);
    }

    const vector< string > sourceList = OptionsParser->getSourcePathList();
    auto action = newFrontendActionFactory<MiniInstrumentor::Action>();
    Tool.run(action.get());

    return 0;
}
