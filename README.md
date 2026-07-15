# Libmacro

Libmacro is a multi-platform, extendable macro and hotkey C library.

# License

Libmacro uses the LGPL v2.1 license. Please refer to the [LICENSE](./LICENSE)
or [lgpl-2.1.txt](./lgpl-2.1.txt) files for more information.

## Supported compilers

- GCC 4.8+
- Visual Studio 2015+
- Clang needs testing for version 3.3+

## Supported cpack installer compilers

- NSIS
- More research required

## Building source

Required build tools:

- cmake
  - Optionally built with QtCreator or other IDE using cmake generators.
    Instructions not included.
  - Given build configuration \<BUILD_TYPE\> cmake and cpack may require the
    same command-line switch.
    - cmake: `cmake --config <BUILD_TYPE>`
    - cpack: `cpack -C <BUILD_TYPE>`
  - May have to set environment variables Qt6_DIR or Qt6Core_DIR, such as
    `export Qt6_DIR=<cmake-dir>/Qt6`
  - Alternatively one may set CMAKE_PREFIX_PATH where cmake is in the
    subdirectory \<CMAKE_PREFIX_PATH\>/lib/cmake/ and Qt6 cmake files are in
    the cmake subdirectory.
    - Example for windows, using QT 5.13.1 and MSVC 2017:
      "CMAKE_PREFIX_PATH=\<QT directory\>/5.13.1/msvc2017_64"
- All makefile generators and build tools for your environment and target
- cmake modules
  - Threads: Always required until further notice. Message loops may be an
    alternative in the future.
  - Git: Required for versioning
  - Qt6Core: Required for testing
  - Qt6Test: Required for testing

Supported optional cmake arguments

- Default custom cmake options: `-DBUILD_DOC=ON
-DBUILD_PACKAGE=ON -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=ON`
  - For a debug build doc and packaging are turned off by default.
- BUILD_DOC: Also build doxygen documentation.
- BUILD_PACKAGE: Build packaging materials to use with cpack.
- BUILD_SHARED_LIBS: A library that is not shared is static.
- BUILD_TESTING: Also build test, development and debugging
  applications.
  automatically.
- CMAKE_BUILD_TYPE: Debug or Release
- CMAKE_CXX_COMPILER: GCC, Visual Compiler, or Clang executable
- CMAKE_C_COMPILER: GCC, Visual Compiler, or Clang executable
- CMAKE_INSTALL_PREFIX: Install location
- CMAKE_PREFIX_PATH: Custom locations for headers, libraries, and QT version
  base directories.
- Qt6_DIR, Qt6Core_DIR, Qt6Test_DIR: Qt cmake directories.
- TEST_TERMINAL: Automated tests have access to a terminal. Not all tests will be run if no terminal is available.

### Contributing

Please follow style guides when it makes sense. clang-format will be used for all
style formatting and manual formatting will not be kept. The following are
known style guidelines used in this project.

- C and C++ style is the Linux Kernel Normal Form (KNF), which is similar to K&R. The
  clang-format config file is [.clang-format](.clang-format).
- C and C++ `#include` statements will be sorted alphabetically (Use vim `:sort` or `:sort u`).
  They may also follow the Google style guide found at
  [https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
- More of the Google style guide may be adopted in the future.

#### Style source

clang-format is required for correct formatting. [scripts/style](scripts/style) runs clang-format for
all source files, with KNF options. Run from any directory to format any C
and C++ header and source files in that directory. Run with `scripts/style --help`
for more information.

#### Platform Style Guides

- Refer to [docs/platform.md](docs/platform.md)
- Platform files use "p\_" as a naming prefix.

#### Internal API

Symbols in the `mcr::internal` namespace are library-internal and not part of the
public API. Consumers must not depend on them.
