/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/signal.h"

#include "mcr/dispatcher.h"
#include "mcr/libmacro.h"

namespace mcr
{
void SignalList::send(Libmacro *libmacroPtr) const
{
	unsigned int mods = libmacroPtr ? libmacroPtr->modifiers() : 0;
	for (mcr_index_t i = 0; i < count; i++) {
		auto &signal = array[i];
		auto *dispatcher = signal.dispatcherPtr;
		if (dispatcher)
			dispatcher->dispatch(&signal, mods);
		signal.send();
	}
}
}
