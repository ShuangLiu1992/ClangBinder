from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain
import os


class ClangBinderonan(ConanFile):
    name = "clang_binder"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("llvm/18.1.0-rc4@")
