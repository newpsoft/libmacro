cmake_minimum_required(VERSION 3.10)

# Export mcr::Libmacro as the library target.

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
# Defines GIT_EXECUTABLE, GIT_BRANCH
include(config/git.cmake)
# Create documentation: build + install targets if BUILD_DOC
include(config/doxygen.cmake)

project(libmacro VERSION ${MCR_VER} HOMEPAGE_URL "https://sites.google.com/view/newparadigmsoftware" LANGUAGES C CXX)
# lib is not prepended to library file name on windows
get_directory_property(hasParent PARENT_DIRECTORY)
if (windows)
	set(LIBMACRO_TARGET libmacro)
	if (hasParent)
		set(LIBMACRO_TARGET libmacro PARENT_SCOPE)
	endif (hasParent)
else ()
	set(LIBMACRO_TARGET macro)
	if (hasParent)
		set(LIBMACRO_TARGET macro PARENT_SCOPE)
	endif (hasParent)
endif (windows)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO Thread optional with event loop
find_package(Threads REQUIRED)

# Lib sources — explicit list (no GLOB) so new files are reliably tracked.
set(LIBMACRO_SRC
	src/api.cpp
	src/dispatcher.cpp
	src/libmacro.cpp
	src/macro.cpp
	src/macro_registry.cpp
	src/serial.cpp
	src/signal.cpp
	src/signal_registry.cpp
	src/trigger.cpp
	src/trigger_registry.cpp
	src/signal/interrupt.cpp
	src/signal/modifier.cpp
	src/signal/noop.cpp
	src/trigger/action.cpp
	src/template/list.cpp
	)
# Platform-specific sources (globbing is fine here — porting is an intentional action).
file(GLOB LIBMACRO_PLATFORM_SRC src/${MCR_PLATFORM}/*.cpp)
list(APPEND LIBMACRO_SRC ${LIBMACRO_PLATFORM_SRC})
file(GLOB_RECURSE COMMON_HEADERS mcr/*.h mcr/*.hpp mcr/*.hxx mcr/*.inl)
file(GLOB_RECURSE LIBMACRO_ASSETS assets/*)
file(GLOB_RECURSE LIBMACRO_BIN bin/*)
set(COMMON_DEFINES
	-DVERSION=${MCR_VER}
	-DGIT_BRANCH=${GIT_BRANCH}
	)

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
if (LINUX)
	target_link_libraries(${LIBMACRO_TARGET} ${LIBEVDEV_LIBRARIES})
endif (LINUX)
# Be aware msvc may require linking to Advapi32 and user32 (reference mingw)

# Library defines
target_compile_definitions(${LIBMACRO_TARGET} PRIVATE -DLIBMACRO_LIBRARY ${COMMON_DEFINES})

# Make files visible in editor, and use a formatter in-editor.
add_custom_target(DISTFILES SOURCES
	.clang-format
	libmacro.astylerc
	README.md
	TODO.md
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

# ── Python bindings (optional) ───────────────────────────────────────────
option(BUILD_PYTHON "Build Python bindings via pybind11" OFF)
if (BUILD_PYTHON)
	# Python include directory — prefer sysconfig, fall back to extracted
	# CPython headers (e.g. CI without python3-dev).
	find_package(Python3 COMPONENTS Interpreter REQUIRED)
	execute_process(
		COMMAND "${Python3_EXECUTABLE}" -c
			"import sysconfig; print(sysconfig.get_path('include'))"
		OUTPUT_VARIABLE _py_include
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if (NOT EXISTS "${_py_include}/Python.h")
		set(_py_include "/tmp/libpy3dev_extracted/usr/include/python3.11")
		set(_py_include_multiarch
			"/tmp/libpy3dev_extracted/usr/include")
	endif ()
	# Seed the old-variable form so pybind11's find_package(PythonLibs) works.
	set(PYTHON_INCLUDE_DIR "${_py_include}" CACHE PATH "")

	# pybind11 — vendored as a git submodule.
	set(PYBIND11_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pybind11"
		CACHE PATH "Path to pybind11 source directory")
	if (NOT EXISTS "${PYBIND11_SOURCE_DIR}/CMakeLists.txt")
		message(FATAL_ERROR
			"pybind11 submodule not found at ${PYBIND11_SOURCE_DIR}. "
			"Run: git submodule update --init --recursive")
	endif ()
	add_subdirectory(${PYBIND11_SOURCE_DIR} pybind11 EXCLUDE_FROM_ALL)

	set(LIBMACRO_PYTHON_MODULE "_libmacro")
	add_library(${LIBMACRO_PYTHON_MODULE} MODULE
		python/libmacro.cpp)
	target_link_libraries(${LIBMACRO_PYTHON_MODULE} PRIVATE
		${LIBMACRO_TARGET}
		pybind11::headers)
	target_include_directories(${LIBMACRO_PYTHON_MODULE} PRIVATE
		${_py_include})
	if (_py_include_multiarch)
		target_include_directories(${LIBMACRO_PYTHON_MODULE} PRIVATE
			${_py_include_multiarch})
	endif ()
	target_compile_features(${LIBMACRO_PYTHON_MODULE} PRIVATE
		cxx_std_17)
	target_compile_definitions(${LIBMACRO_PYTHON_MODULE} PRIVATE
		-DLIBMACRO_LIBRARY)
	# Detect Python extension suffix.
	execute_process(
		COMMAND "${Python3_EXECUTABLE}" -c
			"import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
		OUTPUT_VARIABLE _py_suffix
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	# Prevent "lib" prefix; use Python extension suffix.
	set_target_properties(${LIBMACRO_PYTHON_MODULE} PROPERTIES
		PREFIX ""
		SUFFIX "${_py_suffix}"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python")
endif ()
