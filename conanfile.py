from conans import ConanFile, CMake


class GrpcConan(ConanFile):
    name = "grpc"
    version = "1.27.3"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Grpc here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "use_pic": [True, False]
    }
    default_options = {"shared": False, "use_pic": False}
    requires = ["abseil/abseil@bitwyre/stable",
        "c-ares/1.16.0@bitwyre/stable",
        "openssl/1.1.1j@bitwyre/stable",
        "protobuf/3.11.4@bitwyre/stable",
        "ZLib/1.2.11@bitwyre/stable"]
    generators = "cmake"
    exports_sources = "*"
    no_copy_source = True

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['BUILD_SHARED_LIBS'] = self.options.shared
        cmake.definitions['BUILD_TESTING'] = False
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = self.options.use_pic
        cmake.definitions['gRPC_BUILD_TESTS'] = False
        cmake.definitions['gRPC_BUILD_CODEGEN'] = True
        cmake.definitions['gRPC_BUILD_CSHARP_EXT'] = False

        cmake.definitions['gRPC_ABSL_PROVIDER'] = "package"
        cmake.definitions['ABSL_ROOT_DIR'] = self.deps_cpp_info["abseil"].rootpath

        cmake.definitions['gRPC_SSL_PROVIDER'] = "package"
        cmake.definitions['OPENSSL_ROOT_DIR'] = self.deps_cpp_info["openssl"].rootpath

        cmake.definitions['gRPC_CARES_PROVIDER'] = "package"
        cmake.definitions['CARES_ROOT_DIR'] = self.deps_cpp_info["c-ares"].rootpath

        cmake.definitions['gRPC_PROTOBUF_PROVIDER'] = "package"
        cmake.definitions['gRPC_PROTOBUF_PACKAGE_TYPE'] = "CONFIG"
        cmake.definitions['PROTOBUF_ROOT_DIR'] = self.deps_cpp_info["protobuf"].rootpath

        cmake.definitions['gRPC_ZLIB_PROVIDER'] = "package"
        cmake.definitions['ZLIB_ROOT_DIR'] = self.deps_cpp_info["ZLib"].rootpath

        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["grpc"]
