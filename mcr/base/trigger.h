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
 *  \brief \ref mcr_Trigger - Receive dispatch and trigger actions.
 *
 *  Receive dispatch from \ref mcr_ITrigger.receive.
 */

#ifndef MCR_BASE_TRIGGER_H_
#define MCR_BASE_TRIGGER_H_

#include "mcr/base/itrigger.h"
#include "mcr/base/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Register to be dispatched by \ref mcr_Signal */
struct mcr_Trigger {
	/*! \ref mcr_Instance of \ref mcr_ITrigger */
	union {
		struct mcr_ITrigger *itrigger;
		struct mcr_Instance instance;
		struct mcr_Interface *interface;
	};
	/* Trigger specific */
	/*! Function to call when triggered.
	 *
	 *  This will be called from \ref mcr_ITrigger.receive
	 *  First argument is the \ref mcr_Trigger object.
	 */
	mcr_Trigger_receive_fnc trigger;
	/*! \ref opt Object which receives trigger action */
	void *actor;
};

/*! \pre \ref mcr_DispatchReceiver.receiver must be a \ref mcr_Trigger *
 *  \pre \ref mcr_DispatchReceiver.dispatch must be \ref mcr_Trigger_receive
 *  \brief \ref mcr_DispatchReceiver_fnc for triggers
 *
 *  This is the default receiver function, when none is provided to
 *  add dispatcher receiver.
 *  If \ref mcr_Trigger.trigger is null this will do nothing.
 *  \param triggerPt \ref mcr_Trigger *
 *  \param dispatchSignal \ref opt Intercepted signal
 *  \param mods Intercepted modifiers
 *  \return False to not block intercepted signal, otherwise do block
 */
MCR_API bool mcr_Trigger_receive(struct mcr_DispatchReceiver *dispReceiver,
								 struct mcr_Signal *dispatchSignal, unsigned int mods);
/*! ? */
MCR_API bool mcr_Macro_receive(struct mcr_DispatchReceiver *dispPt,
							   struct mcr_Signal *sigPt, unsigned int mods);

/*! \ref ctor
 *
 *  \param triggerPt \ref opt
 */
MCR_API void mcr_Trigger_init(struct mcr_Trigger *triggerPt);
/*! \ref mcr_Trigger_init and \ref mcr_Trigger_set_all
 *
 *  \param triggerPt \ref opt
 *  \param itrigPt \ref opt \ref mcr_Trigger.itrigger
 *  \param trigger \ref opt \ref mcr_Trigger.trigger
 *  \param actor \ref opt \ref mcr_Trigger.actor
 */
MCR_API void mcr_Trigger_ctor(struct mcr_Trigger *triggerPt,
							  struct mcr_ITrigger *itrigPt, mcr_Trigger_receive_fnc trigger, void *actor);
/*! \ref dtor
 */
MCR_API void mcr_Trigger_deinit(struct mcr_Trigger *trigPt);

/*! \ref mcr_Instance_copy, then copy trigger and actor
 *
 *  \param dstPt \ref mcr_Trigger * Destination to copy to
 *  \param srcPt \ref opt \ref mcr_Trigger * Source to copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Trigger_copy(struct mcr_Trigger *dstPt,
							 const struct mcr_Trigger *srcPt);
/*! Compare two triggers
 *
 *  \param lhsTriggerPt \ref opt mcr_Trigger *
 *  \param rhsTriggerPt \ref opt mcr_Trigger *
 *  \return \ref retcmp
 */
MCR_API int mcr_Trigger_compare(const struct mcr_Trigger *lhsTriggerPt,
								const struct mcr_Trigger *rhsTriggerPt);

/* Dispatch utilities */
/*! Add trigger as receiver and dispatch function into
 *  \ref mcr_IDispatcher_add
 *
 *  \param interceptPt \ref opt Signal to determine dispatch logic
 *  \return \ref reterr
 */
MCR_API int mcr_Trigger_add_dispatch(struct mcr_context *ctx,
									 struct mcr_Trigger *trigPt, struct mcr_Signal *interceptPt);
/*! Remove trigger from \ref mcr_IDispatcher_remove
 *
 *  \param isigPt \ref opt Signal interface to find dispatcher to remove from
 *  \return \ref reterr
 */
MCR_API void mcr_Trigger_remove_dispatch(struct mcr_context *ctx,
		struct mcr_Trigger *trigPt, struct mcr_ISignal *isigPt);

//! \ref mcr_Instance_data
#define mcr_Trigger_data(triggerPt) \
(triggerPt ? (triggerPt)->instance.data_member.data : NULL)

#ifdef __cplusplus
}
#endif

#endif
