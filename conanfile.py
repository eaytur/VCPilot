from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class VCPilotConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    requires = (
        "spdlog/1.17.0",
        "fmt/12.1.0",
    )

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.generate()