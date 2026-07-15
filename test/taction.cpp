/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "taction.h"

#include "expect_actor.h"
#include "mcr/api.h"
#include "mcr/inline.h"
#include "mcr/libmacro.h"
#include "mcr/factory.h"
#include "mcr/signal/noop.h"
#include "mcr/trigger/action.h"
#include "mcr/types.h"

#include <QRandomGenerator>

static std::unique_ptr<mcr::Libmacro, mcr::Libmacro::Deleter> _ctx;

TEST_MAIN(TAction)

void TAction::initTestCase()
{
	_ctx = mcr::factory::createContext(false);
	_ctx->setEnabled(true);
}

void TAction::cleanupTestCase()
{
	_ctx->setEnabled(false);
	_ctx.reset();
}

static bool match(unsigned int triggerMode, unsigned int lhsFlags,
		  unsigned int rhsFlags)
{
	return mcr_TriggerMode_match_inl(triggerMode, lhsFlags, rhsFlags);
}

void TAction::canMatchTriggerMode()
{
	unsigned int rhs;
	for (rhs = 1; rhs < 0x10; rhs++) {
		// Equal
		QVERIFY(!match(MCR_TM_EQUAL, rhs - 1, rhs));
		QVERIFY(match(MCR_TM_EQUAL, rhs, rhs));
		QVERIFY(!match(MCR_TM_EQUAL, rhs + 1, rhs));
		// All
		QVERIFY(!match(MCR_TM_ALL, ~rhs, rhs));
		QVERIFY(match(MCR_TM_ALL, rhs, rhs));
		QVERIFY(match(MCR_TM_ALL, rhs, rhs + 0x20));
		// None
		QVERIFY(!match(MCR_TM_NONE, 0x1F, rhs));
		QVERIFY(match(MCR_TM_NONE, 0x1F, (rhs << 8)));
		// Exclusive
		QVERIFY(match(MCR_TM_EXCLUSIVE, rhs, rhs));
		QVERIFY(!match(MCR_TM_EXCLUSIVE, rhs, rhs + 0x20));
		// Inequal
		QVERIFY(match(MCR_TM_INEQUAL, rhs - 1, rhs));
		QVERIFY(!match(MCR_TM_INEQUAL, rhs, rhs));
		QVERIFY(match(MCR_TM_INEQUAL, rhs + 1, rhs));
		// Any
		if (rhs)
			QVERIFY(match(MCR_TM_ANY, rhs, rhs));
		else
			QVERIFY(!match(MCR_TM_ANY, rhs, ~0));
	}
	// Edge cases
	// Equal
	QVERIFY(match(MCR_TM_EQUAL, 0, 0));
	QVERIFY(!match(MCR_TM_EQUAL, 0, -1));
	QVERIFY(!match(MCR_TM_EQUAL, -1, -0));
	// All
	QVERIFY(match(MCR_TM_ALL, 0, 0));
	QVERIFY(!match(MCR_TM_ALL, 1, 0));
	QVERIFY(!match(MCR_TM_ALL, 1, 2));
	// None
	QVERIFY(match(MCR_TM_NONE, 0x0, 0x0));
	QVERIFY(match(MCR_TM_NONE, 0x0, 0x1));
	QVERIFY(match(MCR_TM_NONE, 0x1, 0x0));
	QVERIFY(match(MCR_TM_NONE, 0x6, 0x1));
	QVERIFY(match(MCR_TM_NONE, 0x1F, 0x20));
	// Exclusive
	QVERIFY(match(MCR_TM_EXCLUSIVE, 0, 0));
	QVERIFY(match(MCR_TM_EXCLUSIVE, 1, 0));
	QVERIFY(!match(MCR_TM_EXCLUSIVE, 0, 1));
	// Inequal
	QVERIFY(match(MCR_TM_INEQUAL, -1, 0));
	QVERIFY(!match(MCR_TM_INEQUAL, 0, 0));
	QVERIFY(match(MCR_TM_INEQUAL, 1, 0));
	// Any
	QVERIFY(match(MCR_TM_ANY, ~0, 1));
	QVERIFY(!match(MCR_TM_ANY, 0, 0));
	QVERIFY(!match(MCR_TM_ANY, 0, ~0));
	QVERIFY(match(MCR_TM_ANY, 1, ~0));
}

void TAction::canFilterActions_data()
{
	QTest::addColumn<unsigned int>("matchModifier");
	QTest::addColumn<unsigned int>("setModifier");
	QTest::addColumn<mcr_TriggerMode>("triggerMode");
	QTest::addColumn<bool>("expectDispatch");

	// Equal
	QTest::newRow("0 = 0")
		<< (unsigned int)0 << (unsigned int)0 << MCR_TM_EQUAL << true;
	QTest::newRow("0 != 1")
		<< (unsigned int)0 << (unsigned int)1 << MCR_TM_EQUAL << false;
	// All
	QTest::newRow("0 is all of 0")
		<< (unsigned int)0 << (unsigned int)0 << MCR_TM_ALL << true;
	QTest::newRow("1 is all of 0")
		<< (unsigned int)0 << (unsigned int)1 << MCR_TM_ALL << true;
	QTest::newRow("0 is not all of 1")
		<< (unsigned int)1 << (unsigned int)0 << MCR_TM_ALL << false;
	QTest::newRow("1 is all of 1")
		<< (unsigned int)1 << (unsigned int)1 << MCR_TM_ALL << true;
	QTest::newRow("2 is not all of 1")
		<< (unsigned int)1 << (unsigned int)2 << MCR_TM_ALL << false;
	QTest::newRow("3 is all of 1")
		<< (unsigned int)1 << (unsigned int)3 << MCR_TM_ALL << true;
	// None
	QTest::newRow("0 is none of 0")
		<< (unsigned int)0 << (unsigned int)0 << MCR_TM_NONE << true;
	QTest::newRow("1 is none of 0")
		<< (unsigned int)1 << (unsigned int)0 << MCR_TM_NONE << true;
	QTest::newRow("0 is none of 1")
		<< (unsigned int)0 << (unsigned int)1 << MCR_TM_NONE << true;
	QTest::newRow("1 is not none of 1")
		<< (unsigned int)1 << (unsigned int)1 << MCR_TM_NONE << false;
	QTest::newRow("1 is none of 2")
		<< (unsigned int)2 << (unsigned int)1 << MCR_TM_NONE << true;
	// Exclusive
	QTest::newRow("0 is exclusively in 0")
		<< (unsigned int)0 << (unsigned int)0 << MCR_TM_EXCLUSIVE
		<< true;
	QTest::newRow("1 is not exclusively in 0")
		<< (unsigned int)0 << (unsigned int)1 << MCR_TM_EXCLUSIVE
		<< false;
	QTest::newRow("0 is exclusively in 1")
		<< (unsigned int)1 << (unsigned int)0 << MCR_TM_EXCLUSIVE
		<< true;
	QTest::newRow("1 is exclusively in 1")
		<< (unsigned int)1 << (unsigned int)1 << MCR_TM_EXCLUSIVE
		<< true;
	QTest::newRow("3 is not exclusively in 1")
		<< (unsigned int)1 << (unsigned int)3 << MCR_TM_EXCLUSIVE
		<< false;
	// Inequal
	QTest::newRow("0 not != 0") << (unsigned int)0 << (unsigned int)0
				    << MCR_TM_INEQUAL << false;
	QTest::newRow("0 != 1")
		<< (unsigned int)0 << (unsigned int)1 << MCR_TM_INEQUAL << true;
	QTest::newRow("1 != 0")
		<< (unsigned int)1 << (unsigned int)0 << MCR_TM_INEQUAL << true;
	QTest::newRow("1 not != 1") << (unsigned int)1 << (unsigned int)1
				    << MCR_TM_INEQUAL << false;
	// Any
	QTest::newRow("0 is any of 0")
		<< (unsigned int)0 << (unsigned int)0 << MCR_TM_ANY << false;
	QTest::newRow("0 is any of 1")
		<< (unsigned int)1 << (unsigned int)0 << MCR_TM_ANY << false;
	QTest::newRow("1 is any of 0")
		<< (unsigned int)0 << (unsigned int)1 << MCR_TM_ANY << false;
}

void TAction::canFilterActions()
{
	QFETCH(unsigned int, matchModifier);
	QFETCH(unsigned int, setModifier);
	QFETCH(mcr_TriggerMode, triggerMode);
	QFETCH(bool, expectDispatch);

	ExpectActor actor;
	mcr::Action action;
	mcr::NoOp sig;

	action.modifiers = matchModifier;
	action.triggerMode = triggerMode;
	action.actorPtr = &actor;

	action.receive(&sig, setModifier);
	QCOMPARE(actor.received, expectDispatch);
}
