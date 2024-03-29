from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain
import os


class ClangBinderonan(ConanFile):
    name = "clang_binder"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("llvm/18.1.0-rc4")
        self.requires("boost/1.84.0")
        self.requires("fmt/10.2.1")
        self.requires("range_v3/0.12.1")
