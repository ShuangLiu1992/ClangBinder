#include <clang/AST/Comment.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>

int main(int argc, const char** argv)
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& OS)
    {
        OS << "LLVM " << LLVM_VERSION_MAJOR << "." << LLVM_VERSION_MINOR <<
            "." << LLVM_VERSION_PATCH << "\n";
    });
    llvm::cl::OptionCategory category{"ClangBinder"};
    llvm::Expected<clang::tooling::CommonOptionsParser> eop = clang::tooling::CommonOptionsParser::create(
        argc, argv, category);
    return 1;
}
