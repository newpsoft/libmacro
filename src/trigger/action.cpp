/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/trigger/action.h"
#include "mcr/inline.h"

namespace mcr
{
bool Action::receive(Signal *signalPtr, unsigned int mods)
{
	if (!mcr_TriggerMode_match_inl(triggerMode, modifiers, mods))
		return false;
	return trigger(signalPtr, mods); // blocking?
}
}
