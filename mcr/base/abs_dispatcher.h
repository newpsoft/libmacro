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
 *  \brief \ref mcr_AbsDispatcher
 */

#ifndef MCR_BASE_ABS_DISPATCHER_H_
#define MCR_BASE_ABS_DISPATCHER_H_

#include "mcr/base/idispatcher.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Dispatcher of any signal type
 *
 *  In cases of extreme complexity please break glass.
 */
struct mcr_AbsDispatcher {
	/*! self */
	struct mcr_IDispatcher idispatcher;
	/*! Receivers that always receive. */
	struct {
		struct mcr_DispatchReceiver *receivers;
		size_t receiver_count;
	};
	/*! Receivers of a specific signal reference.
	 *
	 *  signal reference -> array of receivers
	 *  \pre Must be sorted by pointer to signal. e.g. \ref mcr_ref_compare */
	struct {
		struct mcr_ReceiverMapElement *signal_receivers;
		size_t signal_receiver_count;
	};
};

/*! \ref ctor
 *
 *  \post Object is zero'd and the only the dispatch function is set.
 *  Implementation must set the rest of the interface.
 */
MCR_API void mcr_AbsDispatcher_ctor(struct mcr_AbsDispatcher *genericDispatcherPt, struct mcr_context *ctx, void (*deinit)(struct mcr_IDispatcher *), mcr_IDispatcher_add_fnc add, void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_modify_fnc modifier, mcr_IDispatcher_remove_fnc remove, void (*trim)(struct mcr_IDispatcher *));

/*! \ref mcr_IDispatcher.dispatch */
MCR_API bool mcr_AbsDispatcher_dispatch(struct mcr_IDispatcher *dispPt, struct mcr_Signal *sigPt, unsigned int mods);

/*! Set receivers that always receive.
 *
 *  \pre Must be locked by \ref mcr_base.distpatch_lock
 */
MCR_API void mcr_AbsDispatcher_set_receivers(struct mcr_AbsDispatcher *genDispPt, struct mcr_DispatchReceiver *receiverList, size_t receiverCount);
/*! Set receivers of specific signals.
 *
 *  \pre sortedReferenceReceiverList must be sorted by signal pointer.
 *  e.g. \ref mcr_ref_compare
 *  \pre Must be locked by \ref mcr_base.distpatch_lock
 */
MCR_API void mcr_AbsDispatcher_set_signal_receivers(struct mcr_AbsDispatcher *genDispPt, struct mcr_ReceiverMapElement *sortedReferenceReceiverList, size_t referenceReceiverCount);

#ifdef __cplusplus
}
#endif

#endif
