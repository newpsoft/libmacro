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
 *  \brief \ref mcr_IDispatcher - Blocking dispatcher, intercept signals before
 *  sending.
 *
 *  Dispatchers are the consumers of modifiers.
 */

#ifndef MCR_BASE_IDISPATCHER_H_
#define MCR_BASE_IDISPATCHER_H_

#include "mcr/base/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Compare \ref mcr_DispatchReceiver, only compare receivers. */
#define mcr_DispatchReceiver_compare mcr_ref_compare

/*! Function on dispatcher with no arguments.
 *
 *  \param dispPt \ref mcr_IDispatcher *
 *  \return \ref reterr
 */
typedef int (*mcr_IDispatcher_fnc)(struct mcr_IDispatcher *idispPt);
/*! Add dispatching receiver for a signal.
 *
 *  \param idispPt \ref mcr_IDispatcher * Object which registers dispatch
 *  logic, and dispatches signals with modifiers
 *  \param sigPt \ref opt Signal-specific logic for what to intercept
 *  \param receiver \ref opt If null the receiverFnc will be sent with
 *  a null pointer.  This must be set for null receiver function, or
 *  mcr_Trigger_receive
 *  \param receiverFnc \ref opt If null \ref mcr_Trigger_receive will be
 *  used.  If receiver is not set, then this cannot be used with
 *  \ref mcr_Trigger_receive.
 *  \return \ref reterr
 */
typedef int (*mcr_IDispatcher_add_fnc)(struct mcr_IDispatcher *idispPt,
									   struct mcr_Signal *sigPt, void *receiver,
									   mcr_dispatch_receive_fnc receiverFnc);
/*! Remove receiver
 *
 *  \param idispPt \ref mcr_IDispatcher *
 *  \param remReceiver \ref opt Receiver to remove
 *  \return \ref reterr
 */
typedef void (*mcr_IDispatcher_remove_fnc)(struct mcr_IDispatcher *idispPt,
		void *remReceiver);
/*! Dispatch signal and modifiers
 *
 *  \param idispPt \ref mcr_IDispatcher *
 *  \param sigPt \ref opt Intercepted signal
 *  \param mods Intercepted modifiers
 *  \return True to block sending signal
 */
typedef bool(*mcr_IDispatcher_dispatch_fnc)(struct mcr_IDispatcher *idispPt,
		struct mcr_Signal *sigPt, unsigned int mods);
/*! Modify modifiers from given signal
 *
 *  \param idispPt \ref mcr_IDispatcher *
 *  \param sigPt \ref opt Intercepted signal
 *  \param modsPt The modifiers passed in will be modified directly.
 */
typedef void (*mcr_IDispatcher_modify_fnc)(struct mcr_IDispatcher *idispPt,
		struct mcr_Signal *sigPt, unsigned int *modsPt);

/*! Blocking Dispatcher, dispatches into \ref mcr_DispatchReceiver.
 *
 *  This is just the function table.  Other members are intended to exist
 *  elsewhere.
 */
struct mcr_IDispatcher {
	/*! \ref opt */
	struct mcr_context *ctx;

	/*! Notify being removed. */
	void (*deinit)(struct mcr_IDispatcher *);
	/*! Add to receivers depending on a signal data. */
	mcr_IDispatcher_add_fnc add;
	/*! Clear all receivers. */
	void (*clear)(struct mcr_IDispatcher *);
	/*! Dispatch and return blocking status. */
	mcr_IDispatcher_dispatch_fnc dispatch;
	/*! Change modifiers from the given signal. */
	mcr_IDispatcher_modify_fnc modifier;
	/*! Remove given receiver. */
	mcr_IDispatcher_remove_fnc remove;
	/*! Minimize allocated data */
	void (*trim)(struct mcr_IDispatcher *);
};

/*! \ref ctor
 *
 *  \param ctx \ref opt \ref mcr_IDispatcher.ctx
 *  \param add \ref opt \ref mcr_IDispatcher.add
 *  \param clear \ref opt \ref mcr_IDispatcher.clear
 *  \param dispatch \ref opt \ref mcr_IDispatcher.dispatch
 *  \param modifier \ref opt \ref mcr_IDispatcher.modifier
 *  \param remove \ref opt \ref mcr_IDispatcher.remove
 *  \param trim \ref opt \ref mcr_IDispatcher.trim
 */
MCR_API void mcr_IDispatcher_ctor(struct mcr_IDispatcher *idispPt,
								  struct mcr_context *ctx,
								  void (*deinit)(struct mcr_IDispatcher *),
								  mcr_IDispatcher_add_fnc add,
								  void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_dispatch_fnc dispatch,
								  mcr_IDispatcher_modify_fnc modifier, mcr_IDispatcher_remove_fnc remove,
								  void (*trim)(struct mcr_IDispatcher *));

/*! Get a dispatcher from a signal id.
 *
 *  \param signalTypeId Id of the signal type
 *  \return Reference to a dispatcher registered for
 *  given signal type's id
 */
MCR_API struct mcr_IDispatcher *mcr_IDispatcher_from_id(struct mcr_context
		*ctx, size_t signalTypeId);

#ifdef __cplusplus
}
#endif

#endif
