/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/api.h"

#ifdef MCR_DEBUG
#include <assert.h>
#endif

#include "mcr/libmacro.h"
#include "mcr/types.h"

unsigned int mcr_flags_combine(const unsigned int *flagsArray,
			       mcr_index_t length)
{
	unsigned int ret = 0;
	if (!flagsArray || length == (mcr_index_t)~0 || !length)
		return MCR_MF_NONE;
	while (length--) {
		ret |= flagsArray[length];
	}
	return ret;
}

unsigned int mcr_flags_add(unsigned int flags, unsigned int newMod)
{
	return flags | newMod;
}

bool mcr_flags_has(unsigned int flags, unsigned int modVal)
{
	return (flags & modVal) == modVal;
}

unsigned int mcr_flags_remove(unsigned int flags, unsigned int delMod)
{
	return flags & (~delMod);
}


