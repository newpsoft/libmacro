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
# Defines GIT_EXECUTABLE, GIT_REVISION, GIT_BRANCH
include(config/git.cmake)
# Create documentation: build + install targets if BUILD_DOC
include(config/doxygen.cmake)

project(libmacro VERSION ${MCR_VER}.${GIT_REVISION} HOMEPAGE_URL "https://sites.google.com/view/newparadigmsoftware" LANGUAGES C CXX)
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
	-DVERSION=${MCR_VER}.${GIT_REVISION}
	-DGIT_BRANCH=${GIT_BRANCH}
	-DGIT_REVISION=${GIT_REVISION}
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
