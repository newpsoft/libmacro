find_package(Git)
if (!Git_FOUND)
	set(GIT_EXECUTABLE git)
endif (!Git_FOUND)
# Master exists for rev-list
execute_process(
	COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
	rev-parse --quiet --verify refs/heads/master
	OUTPUT_VARIABLE GIT_MASTER_REF)
if ("${GIT_MASTER_REF}" STREQUAL "")
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
		branch master origin/master)
#else ()
#	execute_process(
#		COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
#		update-ref refs/heads/master refs/remotes/origin/master
#		)
endif ()

execute_process(
	COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}" rev-list --count --first-parent refs/heads/master
	OUTPUT_VARIABLE GIT_REVISION
	)
string(STRIP "${GIT_REVISION}" GIT_REVISION)
execute_process(
	COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}" symbolic-ref --short HEAD
	OUTPUT_VARIABLE GIT_BRANCH
	)
string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
