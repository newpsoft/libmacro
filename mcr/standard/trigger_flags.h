/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*! \file
 *  \brief \ref mcr_TriggerFlags - Enumerates behavior of triggering modifiers
 */

#ifndef MCR_STANDARD_TRIGGER_FLAGS_H_
#define MCR_STANDARD_TRIGGER_FLAGS_H_

#include "mcr/base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Logical triggering bahavior of modifiers.
 *
 *  This will define behaviour of how modifiers may or may not trigger a
 *  dispatch to receivers.\n
 *  These flags act as a function between the registered modifier,
 *  and triggering modifier.  The end result is a multi-function machine
 *  of the two modifier variables.\n
 *  Given none=001, some=010, all=100 the following is the function table:\n
 *  000 defaulted to all flags set\n
 *  001 none of\n
 *  010 some of\n
 *  011 none or some of\n
 *  100 all of\n
 *  101 none or all of\n
 *  110 some or all of\n
 *  111 none, some, or all of (trigger any)
 */
enum mcr_TriggerFlags {
	/*! Invalid flags */
	MCR_TF_UNDEFINED = 0,
	/*! None of this modifier may be included. */
	MCR_TF_NONE = 1,
	/*! Some flag of this modifier must be included, but not all. */
	MCR_TF_SOME = MCR_TF_NONE << 1,
	/*! Anything except an exact match */
	MCR_TF_INEQUAL = MCR_TF_NONE | MCR_TF_SOME,
	/*! All flags of this modifier must be included, and not any others. */
	MCR_TF_ALL = MCR_TF_SOME << 1,
	/*! All or nothing match */
	MCR_TF_ALL_OR_NOTHING = MCR_TF_NONE | MCR_TF_ALL,
	/*! At least one mod flag matches */
	MCR_TF_MATCH = MCR_TF_SOME | MCR_TF_ALL,
	/*! All flags set (-1 also works) */
	MCR_TF_ANY = MCR_TF_NONE | MCR_TF_SOME | MCR_TF_ALL,
	/*! At this number and above may be used to extend flag logic. */
	MCR_TF_USER = MCR_TF_ALL << 1
};
/*! Make valid \ref mcr_TriggerFlags from any number */
#define MCR_TF_MASK(number) (MCR_TF_ANY & (number))

/*! Remove valid \ref mcr_TriggerFlags from any number, leaving
 *  only user defined flags.
 */
#define MCR_TF_USER_MASK(number) ((number) & (~MCR_TF_ANY))

/*! Match modifiers using \ref mcr_TriggerFlags logic.
 *
 *  \param triggerFlags \ref mcr_TriggerFlags Logic flags
 *  \param lhs Modifiers that must be matched
 *  \param rhs Modifiers intercepted, or requesting match
 *  \return true if matching modifiers
 */
static MCR_INLINE bool mcr_TriggerFlags_match(unsigned int triggerFlags, unsigned int lhsFlags,
									unsigned int rhsFlags)
{
	switch (triggerFlags) {
	case MCR_TF_NONE:
		return !(lhsFlags & rhsFlags);
		break;
	case MCR_TF_SOME:
		return lhsFlags != rhsFlags && (lhsFlags & rhsFlags);
		break;
	case MCR_TF_INEQUAL:
		return lhsFlags != rhsFlags;
		break;
	case MCR_TF_ALL:
		return lhsFlags == rhsFlags;
		break;
	case MCR_TF_ALL_OR_NOTHING:
		return lhsFlags == rhsFlags || !(lhsFlags & rhsFlags);
		break;
	case MCR_TF_MATCH:
		return !!(lhsFlags & rhsFlags);
		break;
	case MCR_TF_ANY:
		return true;
		break;
	default:
//		mcr_dmsg;
//		fprintf(stderr, "Error: Invalid "
//				MCR_STR(enum mcr_TriggerFlags) "%u\n", triggerFlags);
		break;
	}
	return false;
}

#ifdef __cplusplus
}
#endif

#endif
