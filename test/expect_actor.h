/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#pragma once

#include "mcr/trigger.h"

#include <QtTest/QtTest>

class ExpectActor final : public mcr::IActor {
    public:
	mcr::Signal *signalActual = nullptr;
	bool received = false;
	bool blockingFlag = false;

	MCR_DECL_INTERFACE(ExpectActor)

	void expect(mcr::Signal *signalExpected)
	{
		QCOMPARE(signalActual, signalExpected);
		QVERIFY(received);
		reset();
	}
	void notExpected()
	{
		QCOMPARE(signalActual, nullptr);
		QVERIFY(!received);
		reset();
	}
	void reset()
	{
		signalActual = nullptr;
		received = false;
	}

	virtual bool act() override
	{
		received = true;
		return blockingFlag;
	}
};
