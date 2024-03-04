#include <clang/AST/Comment.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

llvm::cl::OptionCategory category{"ClangBinder"};

struct BinderVisitor : clang::RecursiveASTVisitor<BinderVisitor>
{
    explicit BinderVisitor(clang::CompilerInstance* ci) : ast_context_(ci->getASTContext())
    {
    }

    bool shouldVisitTemplateInstantiations() const { return true; }

    virtual bool VisitFunctionDecl(clang::FunctionDecl* F)
    {
        llvm::outs() << F->getName() << "\n";
        return true;
    }

    clang::ASTContext& ast_context_;
};

struct BinderASTConsumer : clang::ASTConsumer
{
    BinderVisitor visitor_;
    // override the constructor in order to pass CI
    explicit BinderASTConsumer(clang::CompilerInstance* ci) : visitor_(ci)
    {
    }

    // override this to call our ExampleVisitor on the entire source file
    void HandleTranslationUnit(clang::ASTContext& context) override
    {
        visitor_.TraverseDecl(context.getTranslationUnitDecl());
    }
};

struct BinderFrontendAction : clang::ASTFrontendAction
{
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& ci, llvm::StringRef file)
    {
        llvm::outs() << "Process input file " << file << "\n";
        return std::unique_ptr<clang::ASTConsumer>(new BinderASTConsumer(&ci));
    }
};


int main(int argc, const char** argv)
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& OS)
    {
        OS << "LLVM " << LLVM_VERSION_MAJOR << "." << LLVM_VERSION_MINOR <<
            "." << LLVM_VERSION_PATCH << "\n";
    });
    if (auto eop = clang::tooling::CommonOptionsParser::create(
        argc, argv, category))
    {
        clang::tooling::ClangTool tool(eop->getCompilations(), eop->getSourcePathList());
        for (auto s : eop->getSourcePathList())
        {
            llvm::outs() << s << "\n";
        }
        tool.run(clang::tooling::newFrontendActionFactory<BinderFrontendAction>().get());
    }
    return 1;
}
