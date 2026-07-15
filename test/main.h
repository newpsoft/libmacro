/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#pragma once

#include <QtCore>

#ifdef TEST_MAIN
#include "mcr/err.h"
#pragma message "Error: TEST_MAIN is defined"
#endif

#ifdef TESTME
#include "mcr/err.h"
#pragma message "Error: TESTME is defined"
#endif

#ifdef TEST_INDIVIDUAL
// Create a main function without GUI
#define TEST_MAIN(TestClass) QTEST_GUILESS_MAIN(TestClass)
// Do not add this to the global test list
#define TESTME
#else
// Custom main function of all tests
#define TEST_MAIN(TestClass) static TestClass local_test_case;
#define TESTME ::mcr::test::addTest(this)

namespace mcr::test
{
void addTest(QObject *);
}
#endif

#define DECL_TEST(TestClass)                     \
	explicit TestClass(QObject *p = nullptr) \
		: QObject(p)                     \
	{                                        \
		setObjectName(#TestClass);       \
		TESTME;                          \
	}
