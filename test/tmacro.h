/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#pragma once

#include "main.h"

#include <QtTest/QtTest>

class TMacro : public QObject {
	Q_OBJECT
    public:
	DECL_TEST(TMacro)

    private slots:
	void initTestCase();
	void cleanupTestCase();

	void canDefaultState();
	void canSetName();
	void canSetBlocking();
	void canSetSticky();
	void canSetThreadMax();
	void canSetInterruptor();
	void canSetEnabled();
	void canSetContext();
	void canSetActivators();
	void canSetSignals();
	void canSetTriggers();
	void canClearAll();
	void canCopy();
	void canReceive();
	void canAct();
	void canRejectSetSignalsWhileRunning();
	void pauseStopsDispatch();
	void interruptCancelsOneThread();
	void interruptAllCancelsRunning();
	void disablePreventsTrigger();
	void continueAllowsRetrigger();
	void invalidInterruptorThrows();
	void clearThreadsInterruptCleansUp();
	void canApplyDispatchFlag();
	void globalThreadLimitPreventsExcessiveThreads();
};
