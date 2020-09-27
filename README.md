# Libmacro #

 Libmacro is a multi-platform, extendable macro and hotkey C library.  C++
 extras are also available in the "extras" module, included by default.

# License #

 Libmacro uses the LGPL v2.1 license.  Please refer to the [LICENSE](./LICENSE)
 or [lgpl-2.1.txt](./lgpl-2.1.txt) files for more information.

## Supported compilers ##

 * GCC 4.8+
 * Visual Studio 2015+
 * Clang needs testing for version 3.3+

## Supported cpack installer compilers ##

 * NSIS
 * More research required

## Building source ##

 Required build tools:

 * cmake
   * Optionally built with QtCreator or other IDE using cmake generators.
   Instructions not included.
   * Given build configuration \<BUILD_TYPE\> cmake and cpack may require the
   same command-line switch.
     * cmake: `cmake --config <BUILD_TYPE>`
     * cpack: `cpack -C <BUILD_TYPE>`
	 * May have to set environment variables Qt5_DIR or Qt5Core_DIR, such as
	 `export Qt5_DIR=<cmake-dir>/Qt5`
	 * Alternatively one may set CMAKE_PREFIX_PATH where cmake is in the
	 subdirectory \<CMAKE_PREFIX_PATH\>/lib/cmake/ and Qt5 cmake files are in
	 the cmake subdirectory.
	   * Example for windows, using QT 5.13.1 and MSVC 2017:
	   "CMAKE_PREFIX_PATH=\<QT directory\>/5.13.1/msvc2017_64"
 * All makefile generators and build tools for your environment and target
 * cmake modules
   * Threads: Always required until further notice.
   * Git: Required for versioning
   * OpenSSL: Required for extras
   * Qt5Core: Required for extras
   * Qt5Test: Required for testing

 Supported optional cmake arguments

 * Default custom cmake options: `-DMCR_NOEXTRAS=OFF -DMCR_NOQT=OFF
 -DBUILD_DOC=ON -DBUILD_PACKAGE=ON -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=ON`
   * For a debug build doc and packaging are turned off by default.
 * MCR_NOEXTRAS: Do not include extra functionality, or any C++.  Not yet
 possible on Windows because of threading.
 * MCR_NOQT: Do not include any QT functionality. Implied by the MCR_NOEXTRAS
 option.
 * BUILD_DOC: Also build doxygen documentation.
 * BUILD_PACKAGE: Build packaging materials to use with cpack.
 * BUILD_SHARED_LIBS: A library that is not shared is static.
 * BUILD_TESTING: Also build test, development and debugging
 applications.
 * OPENSSL_ROOT_DIR: If OpenSSL is installed cmake should find this
 automatically.
 * CMAKE_BUILD_TYPE: Debug or Release
 * CMAKE_CXX_COMPILER: GCC, Visual Compiler, or Clang executable
 * CMAKE_C_COMPILER: GCC, Visual Compiler, or Clang executable
 * CMAKE_INSTALL_PREFIX: Install location
 * CMAKE_PREFIX_PATH: Custom locations for headers, libraries, and QT version
 base directories.
 * Qt5_DIR, Qt5Core_DIR, Qt5Test_DIR: Qt cmake directories.

### Contributing ###

 Please follow style guides when it makes sense.  Astyle will be used for all
 style formatting, and manual formatting will not be kept.  The following are
 known style guidelines used in this project.
 * C and C++ style is Kernel Normal Form (KNF), which is similar to K&R.  Refer
 to Astyle documentation for more information.  The Astyle configuration file
 is [libmacro.astylerc](libmacro.astylerc).
 * C and C++ `#include` statements follow the Google style guide found at
 [https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
 * More of the Google style guide may be adopted in the future.

### Style source ###

 Astyle is required for correct formatting.  [style](./style) is a helper script
 that wraps Astyle with some options.  Run from any directory to format any C
 and C++ header and source files in that directory.  Run with `./style --help`
 for more information.
