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
 */

#ifndef MCR_STANDARD_KEY_DISPATCHER_H_
#define MCR_STANDARD_KEY_DISPATCHER_H_

#include "mcr/standard/key.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_AbsKeyDispatcher {
	struct mcr_IDispatcher idispatcher;
	/* down, up, generic or MCR_BOTH is both
	 * apply type + key => array of receivers */
	struct {
		struct mcr_ReceiverMapElement *key_receivers[2];
		size_t key_receiver_count[2];
	};
};

/*! \ref ctor
 *
 *  \post Object is zero'd and only the dispatch and modifier functions are set.
 *  Implementation must set the rest of the interface.
 */
MCR_API void mcr_AbsKeyDispatcher_ctor(struct mcr_AbsKeyDispatcher *keyDispatcherPt,
									struct mcr_context *ctx, void (*deinit)(struct mcr_IDispatcher *),
									mcr_IDispatcher_add_fnc add,
									void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_remove_fnc remove,
									void (*trim)(struct mcr_IDispatcher *));

/*! \ref mcr_IDispatcher.dispatch */
MCR_API bool mcr_AbsKeyDispatcher_dispatch(struct mcr_IDispatcher *idispPt,
										struct mcr_Signal *signalPt, unsigned int mods);
/*! \ref mcr_IDispatcher.modifier */
MCR_API void mcr_AbsKeyDispatcher_modifier(struct mcr_IDispatcher *idispPt,
										struct mcr_Signal *signalPt, unsigned int *modsPt);

/*! Set receivers that always receive.
 *
 *  \pre Must be locked by \ref mcr_base.distpatch_lock
 */
MCR_API void mcr_AbsKeyDispatcher_set_receivers(struct mcr_AbsKeyDispatcher
		*keyDispatcher, enum mcr_ApplyType forApplyType,
		struct mcr_ReceiverMapElement *receivers, size_t receiverCount);

#ifdef __cplusplus
}
#endif

#endif
