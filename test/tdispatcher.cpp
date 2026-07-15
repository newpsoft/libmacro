/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "tdispatcher.h"

#include <qtestcase.h>
#include <cassert>

#include "expect_receiver.h"
#include "mcr/api.h"
#include "mcr/libmacro.h"
#include "mcr/factory.h"
#include "mcr/signal/noop.h"
#include "mcr/types.h"

static std::unique_ptr<mcr::Libmacro, mcr::Libmacro::Deleter> _ctx;

TEST_MAIN(TDispatcher)

void TDispatcher::initTestCase()
{
	_ctx = mcr::factory::createContext(false);
}

void TDispatcher::cleanupTestCase()
{
	_ctx.reset();
}

void TDispatcher::canRegister()
{
	auto dispatcher = _ctx->genericDispatcher();
	ExpectReceiver recv1, recv2;
	mcr::NoOp sig;

	QVERIFY(dispatcher->empty());
	QCOMPARE(dispatcher->count(), 0);

	dispatcher->add(&sig, &recv1);
	QVERIFY(!dispatcher->empty());
	QCOMPARE(dispatcher->count(), 1);

	dispatcher->add(&sig, &recv2);
	QCOMPARE(dispatcher->count(), 2);

	dispatcher->remove(&recv1);
	QCOMPARE(dispatcher->count(), 1);

	dispatcher->clear();
	QVERIFY(dispatcher->empty());
	QCOMPARE(dispatcher->count(), 0);
}

namespace
{
struct TestReceiver final : public mcr::IReceive {
	mcr::Signal *signalActual = nullptr;
	bool received = false;
	bool blocking = false;

	void reset()
	{
		signalActual = nullptr;
		received = false;
	}
	virtual bool receive(mcr::Signal *signalPtr, unsigned int) override
	{
		signalActual = signalPtr;
		received = true;
		return blocking;
	}
};
}

void TDispatcher::canReceive()
{
	auto dispatcher = _ctx->genericDispatcher();
	TestReceiver recv;
	mcr::NoOp sig;

	bool blocked = dispatcher->dispatch(&sig, 0);
	QVERIFY(!blocked);

	dispatcher->add(&sig, &recv);
	blocked = dispatcher->dispatch(&sig, 0);
	QVERIFY(recv.received);
	QCOMPARE(recv.signalActual, &sig);
	QVERIFY(!blocked);

	recv.reset();
	recv.blocking = true;
	blocked = dispatcher->dispatch(&sig, 0);
	QVERIFY(recv.received);
	QVERIFY(blocked);

	recv.reset();
	dispatcher->remove(&recv);
	blocked = dispatcher->dispatch(&sig, 0);
	QVERIFY(!recv.received);
	QVERIFY(!blocked);
}

static mcr_Press_t opp(mcr_Press_t press)
{
	return press == MCR_PRESS ? MCR_UNPRESS : MCR_PRESS;
}
