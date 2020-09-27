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
 *  \brief Standard signal and trigger types
 *
 *  \ref mcr_standard - Standard signal and trigger types module
 *  \ref mcr_Signal data types: \ref mcr_HidEcho,
 *  \ref mcr_Key, \ref mcr_Modifier, \ref mcr_MoveCursor, \ref mcr_NoOp,
 *  and \ref mcr_Scroll \n
 *  \ref mcr_ISignal: \ref mcr_iHidEcho, \ref mcr_iKey,
 *  \ref mcr_iMoveCursor, \ref mcr_iNoOp, and \ref mcr_iScroll\n
 *  \n
 *  \ref mcr_Trigger date types: \ref mcr_Action\n
 *  \ref mcr_ITrigger: \ref mcr_iAction
 */

#ifndef MCR_STANDARD_STANDARD_H_
#define MCR_STANDARD_STANDARD_H_

#include "mcr/base/base.h"
#include "mcr/standard/hid_echo.h"
#include "mcr/standard/key.h"
#include "mcr/standard/modifier.h"
#include "mcr/standard/move_cursor.h"
#include "mcr/standard/noop.h"
#include "mcr/standard/scroll.h"
#include "mcr/standard/action.h"
#include "mcr/standard/abs_key_dispatcher.h"

#ifdef __cplusplus
extern "C" {
#endif

//! \ref mcr_platform
struct mcr_standard_platform;
/*! Standard signal and trigger types module
 *
 *  In cases of extreme complexity, please break glass. */
struct mcr_standard {
	/* Trigger types */
	struct mcr_ITrigger iaction;
	/* Signal types */
	struct mcr_ISignal ihid_echo;
	struct mcr_ISignal ikey;
	struct mcr_ISignal imodifier;
	struct mcr_ISignal imove_cursor;
	struct mcr_ISignal inoop;
	struct mcr_ISignal iscroll;

	/*! key => modifier */
	struct {
		const struct mcr_MapElement *key_modifiers;
		size_t key_modifier_count;
	};
	/*! modifier => key */
	struct {
		const struct mcr_MapElement *modifier_keys;
		size_t modifier_key_count;
	};

	/*! \ref mcr_platform */
	struct mcr_standard_platform *platform_pt;
};

/*! \ref mcr_platform */
extern MCR_API const struct mcr_MapElement *const mcr_key_modifier_defaults;
/*! \ref mcr_platform */
extern MCR_API const size_t mcr_key_modifier_default_count;
/*! \ref mcr_platform */
extern MCR_API const struct mcr_MapElement *const mcr_modifier_key_defaults;
/*! \ref mcr_platform */
extern MCR_API const size_t mcr_modifier_key_default_count;

MCR_API int mcr_modifier_key(struct mcr_context *ctx, unsigned int modifiers);
MCR_API unsigned int mcr_key_modifier(struct mcr_context *ctx, int key);

/* Some extra, possibly useful, utilities */
/*! For both positions, either may be 0, or the same coordinate
 *  array member must either be both negative, or both positive.
 */
MCR_API bool mcr_resembles_justified(const mcr_Dimensions first,
									 const mcr_Dimensions second);
/*! For both positions, the same coordinate array member must
 *  be valued within the measurementError.
 */
MCR_API bool mcr_resembles_absolute(const mcr_Dimensions first,
									const mcr_Dimensions second, const unsigned int measurementError);

/* Platform signal */
struct mcr_HidEcho;
struct mcr_Key;
struct mcr_MoveCursor;
struct mcr_Scroll;
MCR_API int mcr_HidEcho_send_member(struct mcr_HidEcho *dataPt, struct mcr_context *ctx);
MCR_API int mcr_Key_send_member(struct mcr_Key *dataPt, struct mcr_context *ctx);
MCR_API int mcr_MoveCursor_send_member(struct mcr_MoveCursor *dataPt, struct mcr_context *ctx);
MCR_API int mcr_Scroll_send_member(struct mcr_Scroll *dataPt, struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
