from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class VCPilotConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("spdlog/1.17.0")
        self.requires("fmt/12.1.0")
        self.test_requires("catch2/3.15.3")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.generate()