/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "tmacro.h"

#include "mcr/defines.h"
#include "mcr/error.h"
#include "mcr/factory.h"
#include "mcr/libmacro.h"
#include "mcr/macro.h"
#include "mcr/signal/noop.h"

#include <chrono>
#include <thread>

#define THREAD_YIELD_MAX 100

static std::unique_ptr<mcr::Libmacro, mcr::Libmacro::Deleter> _ctx;

TEST_MAIN(TMacro)

void TMacro::initTestCase()
{
	_ctx = mcr::factory::createContext(false);
	_ctx->setEnabled(true);
}

void TMacro::cleanupTestCase()
{
	_ctx->setEnabled(false);
	_ctx.reset();
}

void TMacro::canDefaultState()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	QVERIFY(macro != nullptr);
	QCOMPARE(macro->context(), _ctx.get());
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::DISABLE);
	QVERIFY(!macro->enabled());
	QVERIFY(!macro->blocking());
	QVERIFY(!macro->sticky());
	QCOMPARE(macro->threadMax(), 1u);
	QCOMPARE(macro->threadCount(), 0);
	QCOMPARE(macro->queued(), 0u);
	QCOMPARE(macro->name(), "");
}

void TMacro::canSetName()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	macro->setName("test-macro");
	QCOMPARE(macro->name(), "test-macro");

	macro->setName("");
	QCOMPARE(macro->name(), "");

	QVERIFY_EXCEPTION_THROWN(macro->setName(nullptr), mcr::Error);
}

void TMacro::canSetBlocking()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QVERIFY(!macro->blocking());
	macro->setBlocking(true);
	QVERIFY(macro->blocking());
	macro->setBlocking(false);
	QVERIFY(!macro->blocking());
}

void TMacro::canSetSticky()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QVERIFY(!macro->sticky());
	macro->setSticky(true);
	QVERIFY(macro->sticky());
	macro->setSticky(false);
	QVERIFY(!macro->sticky());
}

void TMacro::canSetThreadMax()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QCOMPARE(macro->threadMax(), 1u);
	macro->setThreadMax(4);
	QCOMPARE(macro->threadMax(), 4u);
	macro->setThreadMax(MCR_THREAD_MAX);
	QCOMPARE(macro->threadMax(), MCR_THREAD_MAX);

	QVERIFY_EXCEPTION_THROWN(macro->setThreadMax(0), mcr::Error);
	QVERIFY_EXCEPTION_THROWN(macro->setThreadMax(MCR_THREAD_MAX + 1),
				 mcr::Error);
}

void TMacro::canSetInterruptor()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QCOMPARE(macro->interruptor(), mcr::IInterrupt::DISABLE);

	macro->setInterruptor(mcr::IInterrupt::CONTINUE);
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);

	macro->setInterruptor(mcr::IInterrupt::PAUSE);
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::PAUSE);

	macro->setInterruptor(mcr::IInterrupt::INTERRUPT);
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::INTERRUPT);

	macro->setInterruptor(mcr::IInterrupt::INTERRUPT_ALL);
	/* INTERRUPT_ALL resets to CONTINUE when no thread is running */
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);

	macro->setInterruptor(mcr::IInterrupt::DISABLE);
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::DISABLE);
}

void TMacro::canSetEnabled()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QVERIFY(!macro->enabled());
	macro->setEnabled(true);
	QVERIFY(macro->enabled());
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);
	macro->setEnabled(false);
	QVERIFY(!macro->enabled());
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::DISABLE);
}

void TMacro::canSetContext()
{
	auto ctx2 = mcr::factory::createContext(false);
	auto macro = mcr::factory::createMacro(_ctx.get());

	QCOMPARE(macro->context(), _ctx.get());
	macro->setContext(ctx2.get());
	QCOMPARE(macro->context(), ctx2.get());
	macro->setContext(_ctx.get());
	QCOMPARE(macro->context(), _ctx.get());
}

void TMacro::canSetActivators()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setActivators(&sig, 1);
	macro->clearActivators();

	macro->setActivators(nullptr, 0);
}

void TMacro::canSetSignals()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSignals(&sig, 1);
	macro->clearSignals();

	macro->setSignals(nullptr, 0);
}

void TMacro::canSetTriggers()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setTriggers(nullptr, 0);
}

void TMacro::canClearAll()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSignals(&sig, 1);
	macro->setActivators(&sig, 1);
	macro->clearAll();
}

void TMacro::canCopy()
{
	auto original = mcr::factory::createMacro(_ctx.get());
	original->setName("original");
	original->setBlocking(true);
	original->setSticky(true);
	original->setThreadMax(4);
	original->setEnabled(true);

	auto copy = mcr::factory::createMacro(_ctx.get());
	copy->copy(*original);

	QCOMPARE(copy->name(), "original");
	QVERIFY(copy->blocking());
	QVERIFY(copy->sticky());
	QCOMPARE(copy->threadMax(), 4u);
	QVERIFY(copy->enabled());
}

void TMacro::canReceive()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	QVERIFY(!macro->blocking());
	macro->setEnabled(true);
	macro->setSignals(&sig, 1);

	macro->receive(&sig, 0);
}

void TMacro::canAct()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QVERIFY(!macro->blocking());
	macro->setEnabled(true);

	bool result = macro->act();
	QVERIFY(!result);

	macro->setBlocking(true);
	result = macro->act();
	QVERIFY(result);
}

void TMacro::canRejectSetSignalsWhileRunning()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSticky(true);
	macro->setBlocking(true);
	macro->setSignals(&sig, 1);
	macro->setEnabled(true);

	std::thread runner([&macro]() { macro->act(); });

	while (!macro->running())
		std::this_thread::yield();

	QVERIFY_EXCEPTION_THROWN(macro->setSignals(&sig, 1), mcr::Error);
	QVERIFY_EXCEPTION_THROWN(macro->clearSignals(), mcr::Error);

	macro->setInterruptor(mcr::IInterrupt::DISABLE);
	runner.join();
}

void TMacro::pauseStopsDispatch()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSticky(true);
	macro->setBlocking(true);
	macro->setSignals(&sig, 1);
	macro->setEnabled(true);

	std::thread runner([&macro]() { macro->act(); });
	while (!macro->running())
		std::this_thread::yield();

	macro->setInterruptor(mcr::IInterrupt::PAUSE);
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::PAUSE);

	macro->setInterruptor(mcr::IInterrupt::CONTINUE);
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);

	macro->setInterruptor(mcr::IInterrupt::DISABLE);
	runner.join();
}

void TMacro::interruptCancelsOneThread()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSticky(true);
	macro->setBlocking(true);
	macro->setSignals(&sig, 1);
	macro->setEnabled(true);

	std::thread runner([&macro]() { macro->act(); });
	while (!macro->running())
		std::this_thread::yield();

	macro->setInterruptor(mcr::IInterrupt::INTERRUPT);
	runner.join();
	while (macro->running())
		std::this_thread::yield();
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);
}

void TMacro::interruptAllCancelsRunning()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSticky(true);
	macro->setBlocking(true);
	macro->setSignals(&sig, 1);
	macro->setEnabled(true);

	std::thread runner([&macro]() { macro->act(); });
	while (!macro->running())
		std::this_thread::yield();

	macro->setInterruptor(mcr::IInterrupt::INTERRUPT_ALL);
	while (macro->running())
		std::this_thread::yield();
	int timeout = THREAD_YIELD_MAX;
	while (macro->interruptor() != mcr::IInterrupt::CONTINUE &&
	       --timeout > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);
	runner.join();
}

void TMacro::disablePreventsTrigger()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSignals(&sig, 1);
	macro->setEnabled(true);
	QVERIFY(macro->enabled());

	macro->setEnabled(false);
	QVERIFY(!macro->enabled());

	macro->receive(&sig, 0);
	QCOMPARE(macro->threadCount(), 0);
}

void TMacro::continueAllowsRetrigger()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSignals(&sig, 1);
	macro->setEnabled(false);
	QVERIFY(!macro->enabled());

	macro->setEnabled(true);
	QVERIFY(macro->enabled());
	QCOMPARE(macro->interruptor(), mcr::IInterrupt::CONTINUE);

	macro->receive(&sig, 0);
}

void TMacro::invalidInterruptorThrows()
{
	auto macro = mcr::factory::createMacro(_ctx.get());

	QVERIFY_EXCEPTION_THROWN(
		macro->setInterruptor((mcr::IInterrupt::Value)99), mcr::Error);
}

void TMacro::clearThreadsInterruptCleansUp()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	macro->setSticky(true);
	macro->setBlocking(true);
	macro->setSignals(&sig, 1);
	macro->setEnabled(true);

	std::thread runner([&macro]() { macro->act(); });
	while (!macro->running())
		std::this_thread::yield();

	macro->setInterruptor(mcr::IInterrupt::DISABLE);
	while (macro->running())
		std::this_thread::yield();
	QCOMPARE(macro->threadCount(), 0);
	runner.join();
}

void TMacro::canApplyDispatchFlag()
{
	auto macro = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;

	// Default is true
	QVERIFY(macro->applyDispatchEnabled());

	// Roundtrip: set false, verify false
	macro->setApplyDispatchEnabled(false);
	QVERIFY(!macro->applyDispatchEnabled());

	// Set back to true
	macro->setApplyDispatchEnabled(true);
	QVERIFY(macro->applyDispatchEnabled());

	// Safe to toggle while macro is running (no throw)
	macro->setSignals(&sig, 1);
	macro->setSticky(true);
	macro->setBlocking(true);
	macro->setEnabled(true);

	std::thread runner([&macro]() { macro->act(); });
	while (!macro->running())
		std::this_thread::yield();

	// Toggle while running — no exception
	macro->setApplyDispatchEnabled(false);
	QVERIFY(!macro->applyDispatchEnabled());
	macro->setApplyDispatchEnabled(true);
	QVERIFY(macro->applyDispatchEnabled());

	macro->setInterruptor(mcr::IInterrupt::DISABLE);
	runner.join();

	// copy() transfers the flag
	auto original = mcr::factory::createMacro(_ctx.get());
	original->setApplyDispatchEnabled(false);
	QVERIFY(!original->applyDispatchEnabled());

	auto copy = mcr::factory::createMacro(_ctx.get());
	QVERIFY(copy->applyDispatchEnabled()); // default true
	copy->copy(*original);
	QVERIFY(!copy->applyDispatchEnabled());
}

void TMacro::globalThreadLimitPreventsExcessiveThreads()
{
	_ctx->setGlobalThreadLimit(2);

	auto macro1 = mcr::factory::createMacro(_ctx.get());
	auto macro2 = mcr::factory::createMacro(_ctx.get());
	mcr::NoOp sig;
	sig.milliseconds = 40;

	macro1->setSignals(&sig, 1);
	macro1->setEnabled(true);
	macro1->setThreadMax(2);
	macro1->setBlocking(true);

	macro2->setSignals(&sig, 1);
	macro2->setEnabled(true);
	macro2->setThreadMax(2);
	macro2->setBlocking(true);

	macro1->act();
	macro2->act();

	while (!macro1->running() || !macro2->running())
		std::this_thread::yield();

	QCOMPARE(macro1->threadCount(), 1);
	QCOMPARE(macro2->threadCount(), 1);

	// Now attempt to spawn a third thread globally. This should fail.
	auto macro3 = mcr::factory::createMacro(_ctx.get());
	macro3->setSignals(&sig, 1);
	macro3->setEnabled(true);
	macro3->setThreadMax(2);

	macro3->receive(&sig, 0);
	QCOMPARE(macro3->threadCount(), 0);

	// Test that setting thread max > global limit throws
	try {
		macro3->setThreadMax(3);
		QFAIL("setThreadMax(3) should have thrown");
	} catch (const mcr::Error &e) {
		QVERIFY(true);
	}

	macro1->setInterruptor(mcr::IInterrupt::DISABLE);
	macro2->setInterruptor(mcr::IInterrupt::DISABLE);
	int timeout = THREAD_YIELD_MAX;
	while (macro3->globalActiveThreadCount() != 0 && --timeout > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	QCOMPARE(macro3->globalActiveThreadCount(), 0);

	// Now that we have slots, macro3 should be able to spawn a thread
	macro3->setBlocking(true);
	macro3->receive(&sig, 0);
	timeout = THREAD_YIELD_MAX;
	while (macro3->threadCount() == 0 && --timeout > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	QCOMPARE(macro3->globalActiveThreadCount(), 1);
	QCOMPARE(macro3->threadCount(), 1);
	macro3->setInterruptor(mcr::IInterrupt::DISABLE);
}
