/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "tcreate.h"

#include "mcr/api.h"
#include "mcr/libmacro.h"
#include "mcr/factory.h"
#include "mcr/signal/modifier.h"

#include <QRandomGenerator>

static std::unique_ptr<mcr::Libmacro, mcr::Libmacro::Deleter> _ctx;

TEST_MAIN(TCreate)

void TCreate::canInitialize()
{
	auto ctx = mcr::factory::createContext(false);
	QVERIFY(ctx != nullptr);
	QVERIFY(!ctx->enabled());
	QVERIFY(ctx->genericDispatcher() != nullptr);
	QVERIFY(ctx->genericDispatcher()->empty());
	QVERIFY(mcr::Libmacro::hasInstance(ctx.get()));
}

void TCreate::canAllocate()
{
	auto ctx = mcr::factory::createContext(false);
	auto &sigReg = ctx->signalRegistry();
	mcr::factory::SignalAllocator alloc = {
		[]() -> mcr::Signal * { return new mcr::Modifier(nullptr); },
		[](mcr::Signal *p) { delete static_cast<mcr::Modifier *>(p); },
		nullptr
	};
	sigReg.map("Modifier", alloc);
	auto sig = sigReg.allocate("Modifier");
	QVERIFY(sig != nullptr);
	QCOMPARE(sig->name(), "Modifier");
	sigReg.deallocate(sig);
}

void TCreate::createNewContext()
{
	mcr::Libmacro *context;
	for (size_t i = 0; i < COUNT; i++) {
		std::unique_ptr<mcr::Libmacro, mcr::Libmacro::Deleter> context(
			mcr::factory::createContext(false));
		QVERIFY(context != nullptr);
		verifyContext(*context);
	}
}

void TCreate::verifyContext(mcr::Libmacro &context)
{
	(void)context.serial();
	(void)context.macroRegistry();
	(void)context.signalRegistry();
	(void)context.triggerRegistry();
	QVERIFY(context.genericDispatcher() != nullptr);
	QVERIFY(context.genericDispatcher()->empty());
	QCOMPARE(context.genericDispatcher()->count(), 0);
	QCOMPARE(context.modifiers(), (unsigned int)0);
	QCOMPARE(context.blockableFlag(), false);
	QCOMPARE(context.genericDispatchFlag(), false);
}
