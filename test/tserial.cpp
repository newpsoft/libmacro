/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "tserial.h"

#include "mcr/api.h"
#include "mcr/libmacro.h"
#include "mcr/factory.h"
#include "mcr/serial.h"
#include "mcr/signal/interrupt.h"

#include <memory>

static std::unique_ptr<mcr::Libmacro, mcr::Libmacro::Deleter> _ctx;

TEST_MAIN(TSerial)

void TSerial::builtinModifiers()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.modifiers("None"), (mcr_ModFlags)MCR_MF_NONE);
	QCOMPARE(serial.modifiers("Any"), MCR_MF_ANY);
	QCOMPARE(serial.modifiers("Alt"), MCR_ALT);
	QCOMPARE(serial.modifiers("Ctrl"), MCR_CTRL);
	QCOMPARE(serial.modifiers("Shift"), MCR_SHIFT);
	QCOMPARE(serial.modifiers("OS"), MCR_OS);
	QCOMPARE(serial.modifiers("User"), MCR_MF_USER);

	QCOMPARE(serial.modifiers("Option"), MCR_ALT);
	QCOMPARE(serial.modifiers("Control"), MCR_CTRL);
	QCOMPARE(serial.modifiers("Win"), MCR_OS);
	QCOMPARE(serial.modifiers("Mac"), MCR_OS);

	QCOMPARE(serial.modifiers("NonExistent"), MCR_MF_ANY);
}

void TSerial::builtinModifierReverse()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.modifiersName(MCR_MF_NONE), "None");
	QCOMPARE(serial.modifiersName(MCR_MF_ANY), "Any");
	QCOMPARE(serial.modifiersName(MCR_ALT), "Alt");
	QCOMPARE(serial.modifiersName(MCR_CTRL), "Ctrl");
	QCOMPARE(serial.modifiersName(MCR_SHIFT), "Shift");
	QCOMPARE(serial.modifiersName(MCR_OS), "OS");
}

void TSerial::builtinTriggerModes()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.triggerMode("Equal"), MCR_TM_EQUAL);
	QCOMPARE(serial.triggerMode("All"), MCR_TM_ALL);
	QCOMPARE(serial.triggerMode("None"), MCR_TM_NONE);
	QCOMPARE(serial.triggerMode("Exclusive"), MCR_TM_EXCLUSIVE);
	QCOMPARE(serial.triggerMode("Inequal"), MCR_TM_INEQUAL);
	QCOMPARE(serial.triggerMode("Any"), MCR_TM_ANY);
	QCOMPARE(serial.triggerMode("User"), MCR_TM_USER);
	QCOMPARE(serial.triggerMode("Match"), MCR_TM_MATCH);

	QCOMPARE(serial.triggerMode("NonExistent"), (mcr_TriggerMode)-1);

	QCOMPARE(serial.triggerModeName(MCR_TM_EQUAL), "Equal");
	QCOMPARE(serial.triggerModeName(MCR_TM_MATCH), "Match");
	QCOMPARE(serial.triggerModeName(MCR_TM_USER), "User");
}

void TSerial::builtinApplyValues()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.applyValue("Set"), MCR_SET);
	QCOMPARE(serial.applyValue("Unset"), MCR_UNSET);
	QCOMPARE(serial.applyValue("Both"), MCR_BOTH);
	QCOMPARE(serial.applyValue("Toggle"), MCR_TOGGLE);

	QCOMPARE(serial.applyValue("NonExistent"), (mcr_ApplyValue)-1);

	QCOMPARE(serial.applyValue(MCR_SET), "Set");
	QCOMPARE(serial.applyValue(MCR_TOGGLE), "Toggle");
}

void TSerial::builtinDimensions()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.dimension("X"), MCR_X);
	QCOMPARE(serial.dimension("Y"), MCR_Y);
	QCOMPARE(serial.dimension("Z"), MCR_Z);
	QCOMPARE(serial.dimension("W"), MCR_W);

	QCOMPARE(serial.dimension("NonExistent"), (mcr_Dimension)-1);

	QCOMPARE(serial.dimension(MCR_X), "X");
	QCOMPARE(serial.dimension(MCR_W), "W");
}

void TSerial::builtinInterrupts()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.interrupt("Continue"),
		 mcr::IInterrupt::CONTINUE);
	QCOMPARE(serial.interrupt("Pause"), mcr::IInterrupt::PAUSE);
	QCOMPARE(serial.interrupt("Interrupt One"),
		 mcr::IInterrupt::INTERRUPT);
	QCOMPARE(serial.interrupt("Interrupt All"),
		 mcr::IInterrupt::INTERRUPT_ALL);
	QCOMPARE(serial.interrupt("Disable"), mcr::IInterrupt::DISABLE);

	QCOMPARE(serial.interrupt("NonExistent"),
		 (mcr::IInterrupt::Value)-1);

	QCOMPARE(serial.interruptName(mcr::IInterrupt::CONTINUE), "Continue");
	QCOMPARE(serial.interruptName(mcr::IInterrupt::DISABLE), "Disable");
}

void TSerial::setModifierName()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	serial.setModifierName("CustomMod", MCR_MF_USER);
	QCOMPARE(serial.modifiers("CustomMod"), MCR_MF_USER);

	serial.setModifierName("AnotherMod", MCR_ALT);
	QCOMPARE(serial.modifiers("AnotherMod"), MCR_ALT);
}

void TSerial::setModifierValueName()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	serial.setModifierValueName(MCR_MF_USER, "User Defined");
	QCOMPARE(serial.modifiersName(MCR_MF_USER), "User Defined");
}

void TSerial::removeModifierName()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	serial.setModifierName("TempMod", MCR_SHIFT);
	QCOMPARE(serial.modifiers("TempMod"), MCR_SHIFT);

	serial.removeModifierName("TempMod");
	QCOMPARE(serial.modifiers("TempMod"), MCR_MF_ANY);
}

void TSerial::overrideBuiltinModifier()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.modifiers("Alt"), MCR_ALT);

	serial.setModifierName("Alt", MCR_SHIFT);
	QCOMPARE(serial.modifiers("Alt"), MCR_SHIFT);

	serial.removeModifierName("Alt");
	QCOMPARE(serial.modifiers("Alt"), MCR_ALT);
}

void TSerial::setTriggerModeName()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	serial.setTriggerModeName("CustomMode", MCR_TM_USER);
	QCOMPARE(serial.triggerMode("CustomMode"), MCR_TM_USER);

	serial.setTriggerModeName("OverrideEqual", MCR_TM_ALL);
	QCOMPARE(serial.triggerMode("OverrideEqual"), MCR_TM_ALL);
}

void TSerial::setTriggerModeValueName()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	serial.setTriggerModeValueName(MCR_TM_USER, "Custom Trigger");
	QCOMPARE(serial.triggerModeName(MCR_TM_USER), "Custom Trigger");
}

void TSerial::removeTriggerModeName()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	serial.setTriggerModeName("TempMode", MCR_TM_EXCLUSIVE);
	QCOMPARE(serial.triggerMode("TempMode"), MCR_TM_EXCLUSIVE);

	serial.removeTriggerModeName("TempMode");
	QCOMPARE(serial.triggerMode("TempMode"), (mcr_TriggerMode)-1);
}

void TSerial::nullSafety()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.modifiers(nullptr), MCR_MF_ANY);
	QCOMPARE(serial.triggerMode(nullptr), (mcr_TriggerMode)-1);
	QCOMPARE(serial.applyValue(nullptr), (mcr_ApplyValue)-1);
	QCOMPARE(serial.dimension(nullptr), (mcr_Dimension)-1);
	QCOMPARE(serial.interrupt(nullptr), (mcr::IInterrupt::Value)-1);

	serial.setModifierName(nullptr, MCR_MF_USER);
	serial.setModifierValueName(MCR_MF_USER, nullptr);
	serial.removeModifierName(nullptr);
	serial.setTriggerModeName(nullptr, MCR_TM_USER);
	serial.setTriggerModeValueName(MCR_TM_USER, nullptr);
	serial.removeTriggerModeName(nullptr);
}

void TSerial::caseInsensitiveLookup()
{
	auto ctx = mcr::factory::createContext(false);
	auto &serial = ctx->serial();

	QCOMPARE(serial.modifiers("alt"), MCR_ALT);
	QCOMPARE(serial.modifiers("ALT"), MCR_ALT);
	QCOMPARE(serial.modifiers("Alt"), MCR_ALT);

	QCOMPARE(serial.modifiers("control"), MCR_CTRL);

	QCOMPARE(serial.triggerMode("equal"), MCR_TM_EQUAL);
	QCOMPARE(serial.triggerMode("EXCLUSIVE"), MCR_TM_EXCLUSIVE);

	serial.setModifierName("Custom", MCR_SHIFT);
	QCOMPARE(serial.modifiers("custom"), MCR_SHIFT);
	QCOMPARE(serial.modifiers("CUSTOM"), MCR_SHIFT);
}
