option(BUILD_TESTING "Also build test, development, and debugging applications." ON)
if (BUILD_TESTING)
	enable_testing(true)
	include(CTest)

	if (TARGET tst_libmacro)
		add_test(tst_libmacro tst_libmacro)
	endif (TARGET tst_libmacro)

	set(CTEST_PROJECT_NAME "Libmacro")
	set(CTEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
	set(CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif (BUILD_TESTING)
