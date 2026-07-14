/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#pragma once

#include "mcr/defines.h"
#include "mcr/dispatcher.h"
#include "mcr/signal.h"

#include <QtTest/QtTest>

struct ExpectReceiver final : public mcr::IReceive {
	mcr::Signal *signalActual = nullptr;
	bool received = false;
	bool blocking = false;

	MCR_DECL_INTERFACE(ExpectReceiver)

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
	virtual bool receive(mcr::Signal *signalPtr, unsigned int) override
	{
		signalActual = signalPtr;
		return blocking;
	}
};
