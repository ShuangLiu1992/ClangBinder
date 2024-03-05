#include <iostream>
#include <clang/AST/Comment.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

llvm::cl::OptionCategory category{"ClangBinder"};

struct BinderVisitor final : clang::RecursiveASTVisitor<BinderVisitor> {
    explicit BinderVisitor(const clang::CompilerInstance *ci) : ast_context_(ci->getASTContext()) {
    }

    static bool shouldVisitTemplateInstantiations() { return true; }

    static bool VisitFunctionDecl(const clang::FunctionDecl *F) {
        llvm::outs() << F->getName() << "\n";
        llvm::outs() << F->getQualifiedNameAsString() << "\n";
        return true;
    }

    clang::ASTContext &ast_context_;
};

struct BinderASTConsumer final : clang::ASTConsumer {
    BinderVisitor visitor_;
    // override the constructor in order to pass CI
    explicit BinderASTConsumer(clang::CompilerInstance *ci) : visitor_(ci) {
    }

    // override this to call our ExampleVisitor on the entire source file
    void HandleTranslationUnit(clang::ASTContext &context) override { visitor_.TraverseDecl(context.getTranslationUnitDecl()); }
};

struct BinderFrontendAction final : clang::ASTFrontendAction {
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &ci, const llvm::StringRef file) override {
        llvm::outs() << "Process input file " << file << "\n";
        return std::unique_ptr<clang::ASTConsumer>(new BinderASTConsumer(&ci));
    }
};

int main(int argc, const char **argv) {
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream &OS) {
        OS << "LLVM " << LLVM_VERSION_MAJOR << "." << LLVM_VERSION_MINOR << "." << LLVM_VERSION_PATCH << "\n";
    });
    std::string err;
    auto        databse = clang::tooling::CompilationDatabase::autoDetectFromDirectory(".", err);
    llvm::outs() << "sources: " << databse->getAllFiles().size() << "\n";
    return 0;
    if (auto eop = clang::tooling::CommonOptionsParser::create(argc, argv, category, llvm::cl::Optional)) {
        clang::tooling::ClangTool tool(eop->getCompilations(), eop->getSourcePathList());
        llvm::outs() << "sources: " << eop->getCompilations().getAllFiles().size() << "\n";
        tool.run(clang::tooling::newFrontendActionFactory<BinderFrontendAction>().get());
    } else {
        std::cout << "no args" << std::endl;
    }
    return 1;
}