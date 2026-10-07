find_package(Git)
if (!Git_FOUND)
	set(GIT_EXECUTABLE git)
endif (!Git_FOUND)

execute_process(
	COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}" symbolic-ref --short HEAD
	OUTPUT_VARIABLE GIT_BRANCH
	)
string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
if ("${GIT_BRANCH}" STREQUAL "")
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}" rev-parse --short HEAD
		OUTPUT_VARIABLE GIT_BRANCH
		)
endif ()
string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
