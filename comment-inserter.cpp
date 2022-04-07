// Clang Headers
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Type.h"
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
#include <iostream>
#include <filesystem>

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
                    caption << "/*\n\t" << funcName;

                    if (decl->getNumParams() > 0) {
                       caption << "\n\t\tParameters:"; 
                    }

                    for (unsigned i = 0; i < decl->getNumParams(); i++) {
                        ParmVarDecl* parmDecl = decl->getParamDecl(i);
                        string parmDeclName = parmDecl->getNameAsString();
                        string parmDeclType = parmDecl->getType().getAsString(); 
                        caption << "\n\t\t\t" <<  parmDeclName << " (" << parmDeclType << ")";
                    }
                    caption << "\n*/\n";
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
            unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& Compiler, StringRef inFile) override {
                fileName = inFile.str();
                rewriter.setSourceMgr(Compiler.getSourceManager(), Compiler.getLangOpts());
                return make_unique<Consumer>(rewriter);
            }

            void EndSourceFileAction() override {
                const RewriteBuffer *RewriteBuf =
                    rewriter.getRewriteBufferFor(getCompilerInstance().getSourceManager().getMainFileID());
                    
                __fs::filesystem::path filePath(fileName);
                string originalFileName = filePath.stem().string();
                string originalFileExt = filePath.extension().string();
                stringstream instrFileName;
                instrFileName << originalFileName << ".instr" << originalFileExt;
                
                ofstream instrFile;
                instrFile.open(instrFileName.str());
                instrFile << string(RewriteBuf->begin(), RewriteBuf->end());
                instrFile.close();
            }

        private:
            string fileName;
            Rewriter rewriter;
    };
}


int main(int argc, const char* argv[]) {
  
    cl::OptionCategory ToolCategory("Comment Inserter");
    Expected<CommonOptionsParser> OptionsParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    ClangTool Tool(OptionsParser->getCompilations(),
                   OptionsParser->getSourcePathList());

    if (argc > 2) {
        cout << "Only takes 1 argument" << endl;
        exit(1);
    }
    const vector< string > sourceList = OptionsParser->getSourcePathList();
    string fileName = sourceList[0];

    auto action = newFrontendActionFactory<CommentInserter::Action>();
    Tool.run(action.get());

    return 0;
}
