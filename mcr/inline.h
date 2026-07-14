/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief A set of inline-API functions, for optimization. Avoid code-bloat by not including this
 *  file in headers. Only include in source files.
 */

#pragma once

#include "mcr/defines.h"
#include "mcr/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Match modifiers using @ref mcr_TriggerMode logic.
 *
 *  @param triggerMode @ref mcr_TriggerMode logic flags.
 *  @param matchingFlags Modifiers that must be matched.
 *  @param incomingFlags Modifiers intercepted, or requesting match.
 *  @return true if the incoming flags match according to the trigger mode.
 */
static MCR_INLINE bool mcr_TriggerMode_match_inl(unsigned int triggerMode,
						 unsigned int matchingFlags,
						 unsigned int incomingFlags)
{
	// Boolean: Bitwise numbers must be converted to 0 or 1, exclusively.
	switch (triggerMode) {
	case MCR_TM_EQUAL:
		// Obviously?
		return matchingFlags == incomingFlags;
		break;
	case MCR_TM_ALL:
		// Equal or more. Equal++
		return (matchingFlags & incomingFlags) == matchingFlags;
		break;
	case MCR_TM_NONE:
		// At least none of these flags.
		return (matchingFlags & incomingFlags) == 0;
		break;
	case MCR_TM_EXCLUSIVE:
		// Filter incoming flags not contained in matchingFlags.
		// 0 can only exclusive 0
		return (matchingFlags | incomingFlags) == matchingFlags;
		break;
	case MCR_TM_INEQUAL:
		// Obviously?
		return matchingFlags != incomingFlags;
		break;
	// case MCR_TM_EQUAL_OR_NONE:
	//  // Equal or at least none of these flags
	//  return matchingFlags == incomingFlags || (matchingFlags & incomingFlags) == 0;
	//  break;
	// case MCR_TM_SOME:
	//  // Has some, not all. Inequal, exclusive, and has matching flags.
	//  // Note, 0 modifiers cannot "some" anything.
	//  return matchingFlags != incomingFlags &&
	//      (matchingFlags | incomingFlags) == matchingFlags &&
	//      (matchingFlags & incomingFlags) != 0;
	//  break;
	// case MCR_TM_MATCH:
	//  // Has some, or all. Exclusive and has matching flags.
	//  // Note, 0 modifiers cannot "match" to anything.
	//  return (matchingFlags | incomingFlags) == matchingFlags &&
	//      (matchingFlags & incomingFlags) != 0;
	//  break;
	case MCR_TM_ANY:
		return (matchingFlags & incomingFlags) != 0;
		break;
	default:
		break;
	}
	return false;
}

#ifdef __cplusplus
}
#endif

