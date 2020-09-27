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
 *  \brief Base module implements
 *  \ref mcr_base - Libmacro first module
 */

#ifndef MCR_BASE_BASE_H_
#define MCR_BASE_BASE_H_

#include "mcr/base/mod_flags.h"
#include "mcr/base/iregistry.h"
#include "mcr/base/isignal.h"
#include "mcr/base/signal.h"
#include "mcr/base/dispatch.h"
#include "mcr/base/abs_dispatcher.h"
#include "mcr/base/itrigger.h"
#include "mcr/base/trigger.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Libmacro base module
 *  Everything needed to at least send and dispatch signals.
 *
 *  In cases of extreme complexity please break glass.
 */
struct mcr_base {
	/*! \ref mcr_ISignal registry */
	struct mcr_IRegistry *isignal_registry_pt;
	/*! \ref mcr_ITrigger registry */
	struct mcr_IRegistry *itrigger_registry_pt;
	/*! Lock while dispatching or modifying dispatchers, do not modify
	 *  dispatchers while dispatching. */
	mtx_t dispatch_lock;
	struct {
		/*! Each dispatcher index is the same as the ISignal id it is to receive. */
		struct mcr_IDispatcher **dispatchers;
		/*! Number of dispatchers for all ISignal types, not including the generic
		 *  dispatcher. */
		size_t dispatcher_count;
	};
	/*! If enabled, the generic dispatcher will be used for all signals. */
	bool generic_dispatcher_flag;
	/*! Generic dispatcher which receives all signals of any type. */
	struct mcr_IDispatcher *generic_dispatcher_pt;
};

/*! Dispatch, and then call the signal interface send function.
 *
 *  If dispatch blocks, the interface send function will not be called.
 *  \param signalPt \ref opt Signal to dispatch and send
 *  \return \ref reterr
 */
MCR_API int mcr_send(struct mcr_context *ctx, struct mcr_Signal *signalPt);

/*! Reference to Libmacro internal modifiers. */
MCR_API unsigned int *mcr_modifiers(struct mcr_context *ctx);
/*! Add modifiers to current
 *
 *  \param addMods Modifiers to add
 */
MCR_API void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods);
/*! Remove modifiers from current
 *
 *  \param remMods Modifiers to remove
 */
MCR_API void mcr_remove_modifiers(struct mcr_context *ctx, unsigned int remMods);

#ifdef __cplusplus
}
#endif

#endif
