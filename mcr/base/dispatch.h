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
 *  \brief Related to \ref idispatcher.h
 *
 *  Dispatchers are the consumers of modifiers.
 */

#ifndef MCR_BASE_DISPATCH_H_
#define MCR_BASE_DISPATCH_H_

#include "mcr/base/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Enter dispatch critical section.  Do not modify dispatchers or receivers
 *  in the middle of dispatch.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_dispatch_lock(struct mcr_context *ctx);
/*! Exit dispatch critical section.  Do not modify dispatchers or receivers
 *  in the middle of dispatch.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_dispatch_unlock(struct mcr_context *ctx);

/*! \pre \ref mcr_IDispatcher.dispatch must be set
 *  \brief Blocking signal intercept
 *
 *  Dispatch with signal and known modifiers into
 *  \ref mcr_IDispatcher.dispatch
 *  \param sigPt \ref opt Object to dispatch, and possibly block sending
 *  \return True to block
 */
MCR_API bool mcr_dispatch(struct mcr_context *ctx, struct mcr_Signal *sigPt);

/*! Set \ref mcr_base.dispatchers and \ref mcr_base.dispatcher_count.
 *
 *  Will lock dispatch in this function.
 *  \return \ref reterr */
MCR_API int mcr_dispatch_set_dispatchers(struct mcr_context *ctx, struct mcr_IDispatcher **dispatchers, size_t count);
/*! Set \ref mcr_base.generic_dispatcher_pt */
MCR_API void mcr_dispatch_set_generic_dispatcher(struct mcr_context *ctx, struct mcr_IDispatcher *dispatcherPt);

/*! Get the number of registered dispatchers, including null values
 *
 *  \return \ref retind
 */
MCR_API size_t mcr_dispatch_count(struct mcr_context *ctx);
/*! Dispatching enabled for a signal type
 *
 *  \param isigPt \ref opt Signal interface
 *  \return True if signal interface has a dispatcher
 */
MCR_API bool mcr_dispatch_enabled(struct mcr_context *ctx,
									 struct mcr_ISignal *isigPt);
/*! Enable dispatching from the given signal type.
 *
 *  \param typePt \ref opt Signal interface to enable dispatch
 *  \param enable True to set the correct dispatcher for the signal interface.
 *  Otherwise set the signal interface dispatcher to null.
 */
MCR_API void mcr_dispatch_set_enabled(struct mcr_context *ctx,
									  struct mcr_ISignal *typePt, bool enableFlag);
/*! Set dispatching enabled for all registered dispatchers
 *
 *  \param enable True to set the correct dispatcher for all signal interfaces.
 *  Otherwise set the signal interface dispatchers are set to null.
 */
MCR_API void mcr_dispatch_set_enabled_all(struct mcr_context *ctx,
		bool enableFlag);
/*! Add a receiver to the dispatch for given signal.
 *
 *  \param interceptPt \ref opt Used for dispatch logic and to find the dispatcher
 *  to add to
 *  \param receiver \ref opt If null the receiverFnc will be sent with
 *  a null pointer.  This must be set for null receiver function, or
 *  mcr_Trigger_receive
 *  \param receiveFnc \ref opt If null \ref mcr_Trigger_receive will be
 *  used.  If receiver is not set, then this cannot be used with
 *  \ref mcr_Trigger_receive.
 *  \return \ref reterr
 */
MCR_API int mcr_dispatch_add(struct mcr_context *ctx,
							 struct mcr_Signal *interceptPt, void *receiver,
							 mcr_dispatch_receive_fnc receiveFnc);
/*! Add a receiver to the generic dispatcher
 *
 *  The generic dispatcher is dispatched for all signals.  If a signal is
 *  provided the address will be used for dispatch logic.
 *  \param interceptPt \ref opt Used for dispatch logic
 *  \param receiver \ref opt If null the receiverFnc will be sent with
 *  a null pointer.  This must be set for null receiver function, or
 *  mcr_Trigger_receive
 *  \param receiveFnc \ref opt If null \ref mcr_Trigger_receive will be
 *  used.  If receiver is not set, then this cannot be used with
 *  \ref mcr_Trigger_receive.
 *  \return \ref reterr
 */
MCR_API int mcr_dispatch_add_generic(struct mcr_context *ctx,
									 struct mcr_Signal *interceptPt, void *receiver,
									 mcr_dispatch_receive_fnc receiveFnc);
/*! Remove all receivers for a signal type
 *
 *  \param isigPt \ref opt Signal type to remove receivers for
 *  \return \ref reterr
 */
MCR_API void mcr_dispatch_clear(struct mcr_context *ctx,
								struct mcr_ISignal *isigPt);
/*! Remove all receivers for all registered dispatchers
 *
 *  \return \ref reterr
 */
MCR_API void mcr_dispatch_clear_all(struct mcr_context *ctx);
/*! \pre \ref mcr_IDispatcher.dispatch must be set
 *  \brief Modify modifiers without dispatching.
 *
 *  If mcr_base.generic_dispatcher_flag then the generic dispatcher will
 *  be used as well.
 *  \ref mcr_IDispatcher.modifier
 *  \param sigPt \ref opt Find mcr_IDispatcher and use in modifier function.
 */
MCR_API void mcr_dispatch_modify(struct mcr_context *ctx,
								 struct mcr_Signal *sigPt,
								 unsigned int *modifiersPt);
/*! Remove a receiver callback for a signal type.
 *
 *  \param typePt \ref opt Signal type to remove from
 *  \param remReceiver \ref opt The receiver callback to be removed
 *  \return \ref reterr
 */
MCR_API void mcr_dispatch_remove(struct mcr_context *ctx,
								 struct mcr_ISignal *typePt, void *remReceiver);
/*! Remove a receiver callback for all signal types.
 *
 *  \param remReceiver \ref opt The receiver object to be removed
 *  \return \ref reterr
 */
MCR_API void mcr_dispatch_remove_all(struct mcr_context *ctx,
									 void *remReceiver);
/*! Minimize allocation for a signal type.
 *
 *  \param isigPt \ref opt Signal type to minimize
 *  \return \ref reterr
 */
MCR_API void mcr_dispatch_trim(struct mcr_context *ctx,
							   struct mcr_ISignal *isigPt);
/*! Minimize allocation for all dispatchers.
 *
 *  \return \ref reterr
 */
MCR_API void mcr_dispatch_trim_all(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
