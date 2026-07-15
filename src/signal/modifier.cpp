/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/signal/modifier.h"

#include "mcr/libmacro.h"

namespace mcr
{
void Modifier::send()
{
	auto mods = context->modifiers();
	switch (apply) {
	case MCR_SET:
		context->setModifiers(mods | modifiers);
		break;
	case MCR_UNSET:
	case MCR_BOTH:
		context->setModifiers(mods & (~modifiers));
		break;
	case MCR_TOGGLE:
		if ((mods & modifiers) == modifiers) {
			context->setModifiers(mods & (~modifiers));
		} else {
			context->setModifiers(mods | modifiers);
		}
		break;
	}
}
}
