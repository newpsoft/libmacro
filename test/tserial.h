/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#pragma once

#include "main.h"

#include <QtTest/QtTest>

class TSerial : public QObject {
	Q_OBJECT
    public:
	DECL_TEST(TSerial)

    private slots:
	void builtinModifiers();
	void builtinModifierReverse();
	void builtinTriggerModes();
	void builtinApplyValues();
	void builtinDimensions();
	void builtinInterrupts();
	void setModifierName();
	void setModifierValueName();
	void removeModifierName();
	void overrideBuiltinModifier();
	void setTriggerModeName();
	void setTriggerModeValueName();
	void removeTriggerModeName();
	void nullSafety();
	void caseInsensitiveLookup();
};
