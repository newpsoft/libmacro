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

class TCreate : public QObject {
	Q_OBJECT
    public:
	const static size_t COUNT = 5;

	DECL_TEST(TCreate)

    private slots:
	void canInitialize();
	void canAllocate();
	void createNewContext();

    private:
	void verifyContext(mcr::Libmacro &context);
};
