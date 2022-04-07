// Clang Headers
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
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

// Namespaces
using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;

// Frontend Tool
namespace CommentInserter {

    /*  RecursiveASTVisitor  */
    class Visitor : public RecursiveASTVisitor<Visitor> {
        public:
            Visitor(Rewriter &rewriter) : rewriter(rewriter) {} 

            bool VisitFunctionDecl(FunctionDecl* decl) {
                if (decl->hasBody()) {
                    DeclarationName declName = decl->getNameInfo().getName();
                    string funcName = declName.getAsString();
                    SourceLocation location = decl->getSourceRange().getBegin();
                    stringstream caption;
                    caption << "// Start function " << funcName << "\n";
                    rewriter.InsertTextBefore(location, caption.str());
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
            unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& Compiler, StringRef) override {
                rewriter.setSourceMgr(Compiler.getSourceManager(), Compiler.getLangOpts());
                return make_unique<Consumer>(rewriter);
            }

            void EndSourceFileAction() override {
                const RewriteBuffer *RewriteBuf =
                    rewriter.getRewriteBufferFor(getCompilerInstance().getSourceManager().getMainFileID());
                ofstream instrFile;
                instrFile.open("instrumentedFile.cpp");
                instrFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
                instrFile.close();
            }

        private:
            Rewriter rewriter;
    };
}


int main(int argc, const char* argv[]) {
  
  cl::OptionCategory ToolCategory("Comment Inserter");
  Expected<CommonOptionsParser> OptionsParser = CommonOptionsParser::create(argc, argv, ToolCategory);
  ClangTool Tool(OptionsParser->getCompilations(),
                 OptionsParser->getSourcePathList());

  auto action = newFrontendActionFactory<CommentInserter::Action>();
  Tool.run(action.get());

  return 0;
}
