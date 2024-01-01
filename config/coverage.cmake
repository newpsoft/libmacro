option(BUILD_TESTING "Also build test, development, and debugging applications." ON)

# Tests use QT for test framework and CLI options
if (BUILD_TESTING)
	# Using QtTest
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	find_package(Qt6 COMPONENTS Core Test REQUIRED)

	enable_testing(true)
	include(CTest)

	file(GLOB_RECURSE TEST_SRC test/*.h test/*.cpp)
	list(FILTER TEST_SRC EXCLUDE REGEX test/tst_.*\.cpp)
	add_executable(tst_libmacro ${TEST_SRC})
	add_dependencies(tst_libmacro ${LIBMACRO_TARGET})
	target_link_libraries(tst_libmacro ${LIBMACRO_TARGET} Qt6::Core Qt6::Test)
	target_compile_definitions(tst_libmacro PRIVATE ${COMMON_DEFINES})

	add_test(tst_libmacro tst_libmacro)

	add_executable(tst_intercept test/tst_intercept.h test/tst_intercept.cpp)
	add_dependencies(tst_intercept ${LIBMACRO_TARGET})
	target_link_libraries(tst_intercept ${LIBMACRO_TARGET} OpenSSL::Crypto Threads::Threads Qt6::Core)
	target_compile_definitions(tst_intercept PRIVATE ${COMMON_DEFINES})

	set(CTEST_PROJECT_NAME "Libmacro")
	set(CTEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
	set(CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif (BUILD_TESTING)
