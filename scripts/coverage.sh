#!/bin/bash

if [ ! -x ./tst_libmacro ]; then
	echo "Error: Need the test executable, tst_libmacro" >&2
	exit -1
fi

./tst_libmacro
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --remove coverage.info '*/moc_*.cpp' --output-file coverage.info
lcov --remove coverage.info '*/test/*' --output-file coverage.info
genhtml -o coverage coverage.info

