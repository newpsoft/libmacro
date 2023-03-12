# TODO cache variables or PARENT_SCOPE?
set(MCR_VER_MAJ 0)
set(MCR_VER_MIN 2)
set(MCR_VER ${MCR_VER_MAJ}.${MCR_VER_MIN})
add_definitions(-DMCR_VER=${MCR_VER} -DMCR_VER_MAJ=${MCR_VER_MAJ}
	-DMCR_VER_MIN=${MCR_VER_MIN} -DMCR_VER=${MCR_VER})

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
	set(MCR_DEBUG true)
	# TODO: Should MCR_DEBUG not be defined outside of building Libmacro?
	add_definitions(-DMCR_DEBUG=1)
else ()
	unset(MCR_DEBUG)
endif ()

if (WIN32)
	set(MCR_PLATFORM windows)
elseif (APPLE)
	set(MCR_PLATFORM apple)
# LINUX is defined as UNIX AND NOT APPLE by cmake
elseif (UNIX)
	# Explicit LINUX definition, other UNIX platforms not supported.
	set(LINUX true)
	set(MCR_PLATFORM linux)
else ()
	set(MCR_PLATFORM none)
endif (WIN32)
string(TOUPPER ${MCR_PLATFORM} MCR_PLATFORM_UPPER)
set(${MCR_PLATFORM} true)
set(MCR_PLATFORM_${MCR_PLATFORM_UPPER} true)
add_definitions(-DMCR_PLATFORM=${MCR_PLATFORM}
	-DMCR_PLATFORM_${MCR_PLATFORM_UPPER}=1
	-DMCR_PLATFORM_DEFINES_H=\"mcr/${MCR_PLATFORM}/p_defines.h\"
	-DMCR_PLATFORM_H=\"mcr/${MCR_PLATFORM}/p_libmacro.h\"
	)
