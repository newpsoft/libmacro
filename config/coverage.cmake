option(BUILD_TESTING "Also build test, development, and debugging applications." ON)
option(TEST_TERMINAL "Automated tests have access to a terminal. Not all tests will be run if no terminal is available." ON)

# Tests use QT for test framework and CLI options
if (BUILD_TESTING)
	# Using QtTest
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
	find_package(Qt6 COMPONENTS Core Test REQUIRED)

	enable_testing(true)
	include(CTest)

	if (TEST_TERMINAL)
		set(TEST_DEFINES -DTEST_TERMINAL=1)
	endif(TEST_TERMINAL)

	# All automatic tests, unit and integration
	file(GLOB TEST_SRC
		test/*.h
		test/*.cpp
		test/${MCR_PLATFORM}/*.cpp
		)
	list(FILTER TEST_SRC EXCLUDE REGEX test/tst_.*\.cpp)
	add_executable(tst_libmacro ${TEST_SRC})
	add_dependencies(tst_libmacro ${LIBMACRO_TARGET})
	target_include_directories(tst_libmacro PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/test")
	target_link_libraries(tst_libmacro ${LIBMACRO_TARGET} Qt6::Core Qt6::Test)
	target_compile_definitions(tst_libmacro PRIVATE ${COMMON_DEFINES} ${TEST_DEFINES})
	if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options(tst_libmacro INTERFACE -O0 -g --coverage)
		target_compile_options(${LIBMACRO_TARGET} INTERFACE -O0 -g --coverage)
		if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
			target_link_options(tst_libmacro INTERFACE --coverage)
			target_link_options(${LIBMACRO_TARGET} INTERFACE --coverage)
		else()
			target_link_libraries(tst_libmacro INTERFACE --coverage)
			target_link_libraries(${LIBMACRO_TARGET} INTERFACE --coverage)
		endif()
	endif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

	# Each individual automatic test
	foreach(case IN ITEMS taction tcreate tdispatcher tmacro tserial)
		file(GLOB TEST_SRC
			test/main.h
			test/sendkey.h
			test/sendkey.cpp
			test/${case}.h
			test/${case}.cpp
			test/${MCR_PLATFORM}/p_${case}.cpp
		)
		add_executable(${case} ${TEST_SRC})
		add_dependencies(${case} ${LIBMACRO_TARGET})
		target_include_directories(${case} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/test")
		target_link_libraries(${case} ${LIBMACRO_TARGET} Qt6::Core Qt6::Test)
		target_compile_definitions(${case} PRIVATE ${COMMON_DEFINES} ${TEST_DEFINES} -DTEST_INDIVIDUAL)
		add_test(${case} ${case})
	endforeach()

	# add_executable(tst_intercept test/tst_intercept.h test/tst_intercept.cpp)
	# add_dependencies(tst_intercept ${LIBMACRO_TARGET})
	# target_link_libraries(tst_intercept ${LIBMACRO_TARGET} Threads::Threads Qt6::Core)
	# target_compile_definitions(tst_intercept PRIVATE ${COMMON_DEFINES})

	set(CTEST_PROJECT_NAME "Libmacro")
	set(CTEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
	set(CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif (BUILD_TESTING)
