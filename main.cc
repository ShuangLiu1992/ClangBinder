#include <clang/AST/Comment.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <range/v3/view.hpp>

struct Function {
    std::string name_;
};
struct Include {
    bool        angled_{};
    std::string file_;
};

struct Writer {
    virtual void write(std::ofstream &out, std::vector<Function> functions, std::vector<Include> includes) = 0;
};

struct LuaSol2Writer : Writer {
    void write(std::ofstream &out, std::vector<Function> functions, std::vector<Include> includes) override;
};

void LuaSol2Writer::write(std::ofstream &out, std::vector<Function> functions, std::vector<Include> includes) {
    out << R"(
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
)";
    for (auto f : includes) {
        if (f.angled_) {
            out << fmt::format("#include <{}>", f.file_);
        } else {
            out << fmt::format("#include \"{}\"", f.file_);
        }
    }

    out << R"(
void lua_bind() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
)";
    for (auto f : functions) {
        out << fmt::format("    lua.set_function(\"{}\", {});\n", f.name_, f.name_);
    }
    out << R"(
}
)";
}

struct Exporter {
    explicit Exporter(const std::filesystem::path &path);
    void     exportFunction(const clang::FunctionDecl *F);
    void     addInclude(const std::string &filename, bool angled);
    ~        Exporter();

  private:
    std::ofstream         out_;
    std::vector<Function> functions_;
    std::vector<Include>  includes_;
};

Exporter::Exporter(const std::filesystem::path &path) { out_ = std::ofstream(path); }

void Exporter::exportFunction(const clang::FunctionDecl *F) { functions_.emplace_back(F->getName().str()); }

void Exporter::addInclude(const std::string &filename, bool angled) { includes_.emplace_back(angled, filename); }

Exporter::~Exporter() { std::make_shared<LuaSol2Writer>()->write(out_, functions_, includes_); }

std::shared_ptr<Exporter> exporter;

struct BinderVisitor final : clang::RecursiveASTVisitor<BinderVisitor> {
    explicit BinderVisitor(const clang::CompilerInstance *ci) : ast_context_(ci->getASTContext()) {}

    static bool shouldVisitTemplateInstantiations() { return true; }

    static bool VisitFunctionDecl(const clang::FunctionDecl *F) {
        if (F->isThisDeclarationADefinition()) {
            exporter->exportFunction(F);
        }
        return true;
    }

    clang::ASTContext &ast_context_;
};

struct BinderASTConsumer final : clang::ASTConsumer {
    BinderVisitor visitor_;
    // override the constructor in order to pass CI
    explicit BinderASTConsumer(clang::CompilerInstance *ci) : visitor_(ci) {}

    // override this to call our ExampleVisitor on the entire source file
    void HandleTranslationUnit(clang::ASTContext &context) override { visitor_.TraverseDecl(context.getTranslationUnitDecl()); }
};

struct Find_Includes : clang::PPCallbacks {
    void InclusionDirective(clang::SourceLocation HashLoc, const clang::Token &IncludeTok, llvm::StringRef FileName, bool IsAngled,
                            clang::CharSourceRange FilenameRange, clang::OptionalFileEntryRef File, llvm::StringRef SearchPath,
                            llvm::StringRef RelativePath, const clang::Module *Imported,
                            clang::SrcMgr::CharacteristicKind FileType) override {
        exporter->addInclude(FileName.str(), IsAngled);
    }
};

struct BinderFrontendAction final : clang::ASTFrontendAction {
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &ci, const llvm::StringRef file) override {
        return std::unique_ptr<clang::ASTConsumer>(new BinderASTConsumer(&ci));
    }
    bool BeginSourceFileAction(clang::CompilerInstance &CI) override {
        CI.getPreprocessor().addPPCallbacks(std::make_unique<Find_Includes>());
        return true;
    }
    void EndSourceFileAction() override {}
};

int main(int argc, const char **argv) {
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream &OS) {
        OS << "LLVM " << LLVM_VERSION_MAJOR << "." << LLVM_VERSION_MINOR << "." << LLVM_VERSION_PATCH << "\n";
    });
    std::string err;
    auto        databse = clang::tooling::CompilationDatabase::autoDetectFromDirectory("./test_export", err);
    if (databse) {
        exporter = std::make_shared<Exporter>("./lua.cpp");
        clang::tooling::ClangTool tool(*databse, databse->getAllFiles());
        tool.run(clang::tooling::newFrontendActionFactory<BinderFrontendAction>().get());
    }
    return 0;
}