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
 *  \brief \ref mcr_ITrigger - Interface for triggers
 *
 *  Use \ref mcr_DispatchReceiver_fnc to dispatch into triggers.
 */

#ifndef MCR_BASE_ITRIGGER_H_
#define MCR_BASE_ITRIGGER_H_

#include "mcr/base/interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*mcr_Trigger_receive_fnc)(struct mcr_Trigger *triggerPt,
										struct mcr_Signal *dispatchSignal, unsigned int mods);
/*! Interface for \ref mcr_Trigger */
struct mcr_ITrigger {
	/*! \ref mcr_Trigger interface */
	struct mcr_Interface interface;
	/*! Filter dispatch before triggering.
	 *
	 *  This is to dispatch and do required logic before the final
	 *  trigger.  It cannot be NULL.  First argument is \ref mcr_Trigger. */
	mcr_Trigger_receive_fnc receive;
};

/*! \ref ctor
 *
 *  \param itrigPt \ref opt
 */
MCR_API void mcr_ITrigger_init(struct mcr_ITrigger *itrigPt);
/*! \ref ctor
 *
 *  \param itrigPt \ref opt
 *  \param context \ref opt \ref mcr_Interface.context
 *  \param dataSize \ref opt \ref mcr_Interface.data_size
 *  \param allocate \ref opt \ref mcr_Interface.allocate
 *  \param deallocate \ref opt \ref mcr_Interface.deallocate
 *  \param init \ref opt \ref mcr_Interface.init
 *  \param deinit \ref opt \ref mcr_Interface.deinit
 *  \param compare \ref opt \ref mcr_Interface.compare
 *  \param copy \ref opt \ref mcr_Interface.copy
 *  \param receiveFnc \ref mcr_ITrigger.receive_fnc
 */
MCR_API void mcr_ITrigger_ctor(struct mcr_ITrigger *itrigPt,
							   struct mcr_context *context, size_t dataSize, void *(*allocate)(),
							   void (*deallocate)(void *), mcr_data_fnc init, mcr_data_fnc deinit,
							   mcr_compare_fnc compare, mcr_copy_fnc copy, mcr_Trigger_receive_fnc receiveFnc);
/*! \ref dtor
 *
 *  Removes self from registry, if the registry is set.
 *  \param itrigPt \ref opt
 */
MCR_API void mcr_ITrigger_deinit(struct mcr_ITrigger *itrigPt);

/*! Get the \ref mcr_IRegistry of \ref mcr_ITrigger */
MCR_API struct mcr_IRegistry *mcr_ITrigger_registry(struct mcr_context *ctx);
/*! Get the id of a trigger interface.
 *
 *  \param itrigPt \ref opt
 *  \return \ref retid
 */
MCR_API size_t mcr_ITrigger_id(struct mcr_ITrigger *itrigPt);
/*! Get a trigger interface from its id
 *
 *  \param id Id of the trigger interface
 *  \return Trigger interface, or null if not found
 */
MCR_API struct mcr_ITrigger *mcr_ITrigger_from_id(struct mcr_context *ctx,
		size_t id);
/*! Get the name of a trigger interface.
 *
 *  \param itrigPt \ref opt
 *  \return Name of the trigger interface, or null if not found
 */
MCR_API const char *mcr_ITrigger_name(struct mcr_context *ctx,
									  struct mcr_ITrigger *itrigPt);
/*! Get a trigger interface from its name
 *
 *  \param name \ref opt Name of the trigger interface
 *  \return Trigger interface, or null if not found
 */
MCR_API struct mcr_ITrigger *mcr_ITrigger_from_name(struct mcr_context *ctx,
		const char *name);

#ifdef __cplusplus
}
#endif

#endif
