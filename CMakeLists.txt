cmake_minimum_required(VERSION 3.5)

# Export mcr::Libmacro as the library target.

option(MCR_NOEXTRAS "Do not include extra functionality, or any C++.  Not yet possible on Windows because of threading." OFF)
# stl_off not supported

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]el" OR CMAKE_BUILD_TYPE MATCHES "^[Mm]in")
	option(BUILD_PACKAGE "Also build installer or package requirements." ON)
else ()
	option(BUILD_PACKAGE "Also build installer or package requirements." OFF)
endif ()
option(BUILD_SHARED_LIBS "Build all libraries shared." ON)
if (BUILD_SHARED_LIBS)
	unset(MCR_STATIC)
else ()
	set(MCR_STATIC ON)
endif (BUILD_SHARED_LIBS)

# Defines MCR_VER, MCR_VER_MAJ, MCR_VER_MIN, MCR_DEBUG, MCR_PLATFORM
include(config/libmacro.cmake)
# Defines GIT_EXECUTABLE, GIT_REVISION, GIT_BRANCH
include(config/git.cmake)
# Create documentation: build + install targets if BUILD_DOC
include(config/doxygen.cmake)

project(libmacro VERSION ${MCR_VER}.${GIT_REVISION} HOMEPAGE_URL "https://sites.google.com/view/newparadigmsoftware" LANGUAGES C CXX)
# lib is not prepended to library file name on windows
if (windows)
	set(LIBMACRO_TARGET libmacro)
	set(LIBMACRO_TARGET libmacro PARENT_SCOPE)
else ()
	set(LIBMACRO_TARGET macro)
	set(LIBMACRO_TARGET macro PARENT_SCOPE)
endif (windows)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Openssl static
if (MCR_STATIC)
	set(OPENSSL_USE_STATIC_LIBS ON)
	add_definitions(-DMCR_STATIC)
endif (MCR_STATIC)
# Openssl use multithreaded
set(OPENSSL_MSVC_STATIC_RT ON)

# TODO Thread optional with event loop
find_package(Threads REQUIRED)
if (NOT MCR_NOEXTRAS)
	find_package(OpenSSL REQUIRED)
endif (NOT MCR_NOEXTRAS)

# Lib sources
file(GLOB LIBMACRO_SRC
	src/util/*.c
	src/base/*.c
	src/standard/*.c
	src/intercept/*.c
	src/*.c
	src/*/${MCR_PLATFORM}/*.c
	src/extras/*.cpp
	src/extras/references/*.cpp
	src/extras/signals/*.cpp
	src/extras/triggers/*.cpp
	src/extras/util/*.cpp
	src/extras/${MCR_PLATFORM}/*.cpp
	src/extras/ssl/p_safe_string.cpp
	)
file(GLOB_RECURSE COMMON_HEADERS mcr/*.h)
file(GLOB_RECURSE LIBMACRO_ASSETS assets/*)
file(GLOB_RECURSE LIBMACRO_BIN bin/*)
set(COMMON_DEFINES
	-DVERSION=${MCR_VER}.${GIT_REVISION}
	-DGIT_BRANCH=${GIT_BRANCH}
	-DGIT_REVISION=${GIT_REVISION}
	)

# Windows requires C++ threads. Try not to spin threads in C because of this.
if (windows)
	# Mingw and VC <= 2017 do not have C threads
	list(APPEND LIBMACRO_SRC src/util/cppthread.cpp)
endif (windows)

# Remove extras module
if (MCR_NOEXTRAS)
	list(FILTER LIBMACRO_SRC EXCLUDE REGEX
		src/extras/
		)
endif (MCR_NOEXTRAS)

# Library config files
if (windows)
	if (MCR_STATIC)
		set(TARGET_FILENAME libmacro.lib)
	else ()
		set(TARGET_FILENAME libmacro.dll)
	endif (MCR_STATIC)
	set(LIBMACRO_EXTRA_FILES "${CMAKE_CURRENT_BINARY_DIR}/libmacro.rc")
	configure_file(libmacro.rc.in "${LIBMACRO_EXTRA_FILES}" @ONLY)
else ()
	set(LIBMACRO_EXTRA_FILES "")
endif ()

# Compile
add_library(${LIBMACRO_TARGET} ${LIBMACRO_SRC}
	${COMMON_HEADERS} "${LIBMACRO_EXTRA_FILES}"
	)
add_library(mcr::Libmacro ALIAS ${LIBMACRO_TARGET})
# TODO PUBLIC_HEADER on OSX
set_target_properties(${LIBMACRO_TARGET} PROPERTIES
	SOVERSION ${MCR_VER})
# Export includes
target_include_directories(${LIBMACRO_TARGET} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Library link
if (NOT MCR_NOEXTRAS)
	target_link_libraries(${LIBMACRO_TARGET} OpenSSL::Crypto Threads::Threads)
endif (NOT MCR_NOEXTRAS)
# Be aware msvc may require linking to Advapi32 and user32 (reference mingw)

# Library defines
target_compile_definitions(${LIBMACRO_TARGET} PRIVATE -DLIBMACRO_LIBRARY ${COMMON_DEFINES})

# Tests use QT for test framework and CLI options
if (BUILD_TESTING)
	# Using Qt and QtTest
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	find_package(Qt5 COMPONENTS Core Test REQUIRED)

	file(GLOB_RECURSE TEST_SRC test/*.h test/*.cpp)
	list(FILTER TEST_SRC EXCLUDE REGEX test/tst_.*\.cpp)
	add_executable(tst_libmacro ${TEST_SRC})
	add_dependencies(tst_libmacro ${LIBMACRO_TARGET})
	target_link_libraries(tst_libmacro ${LIBMACRO_TARGET} Qt5::Core Qt5::Test)
	target_compile_definitions(tst_libmacro PRIVATE ${COMMON_DEFINES})

	add_executable(tst_intercept test/tst_intercept.h test/tst_intercept.cpp)
	add_dependencies(tst_intercept ${LIBMACRO_TARGET})
	target_link_libraries(tst_intercept ${LIBMACRO_TARGET} OpenSSL::Crypto Threads::Threads Qt5::Core)
	target_compile_definitions(tst_intercept PRIVATE ${COMMON_DEFINES})
endif (BUILD_TESTING)

# Make files visible in editor, and use astyle in-editor.
add_custom_target(DISTFILES SOURCES
	libmacro.astylerc
	README.md
	todo.txt
	${LIBMACRO_BIN}
	${LIBMACRO_ASSETS}
	)

# Windows package everything to base directory, otherwise lib is separate.
if (windows)
	install(TARGETS ${LIBMACRO_TARGET} DESTINATION /)
else ()
	install(TARGETS ${LIBMACRO_TARGET} DESTINATION lib)
endif (windows)
install(DIRECTORY mcr DESTINATION include)

# Option to build tests
include(config/coverage.cmake)

# Platform variables and MCR_DEBUG required from libmacro.cmake
include(config/cpack.cmake)
