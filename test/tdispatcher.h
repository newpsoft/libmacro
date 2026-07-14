/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#pragma once

#include "main.h"

#include <QtTest/QtTest>

namespace mcr
{
class Libmacro;
}

class TDispatcher : public QObject {
	Q_OBJECT
    public:
	DECL_TEST(TDispatcher)

    private slots:
	void initTestCase();
	void cleanupTestCase();

	void canRegister();
	void canReceive();
};
