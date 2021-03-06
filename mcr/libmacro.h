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
 *  \brief Include all Libmacro functionality.
 *
 *  \todo Create a list of missing functionality when MCR_NOEXTRAS is defined.
 */

/*! \namespace mcr
 *  \brief Libmacro, by Jonathan Pelletier, New Paradigm Software. Alpha version.
 *
 *  1. \ref mcr_Signal is dispatched to \ref mcr_IDispatcher using \ref mcr_dispatch.\n
 *  		1.0.a Disable dispatch for a signal by setting \ref mcr_Signal.dispatch_flag to false.\n
 *  		1.0.b Disable dispatch for all of an ISignal type by setting \ref mcr_ISignal.dispatcher to NULL.\n
 *  		1.0.c Disable Libmacro generic dispatch (receive all types) by setting \ref mcr_context.base.generic_dispatcher_flag to false.\n
 *  	1.1 Dispatching may be received by \ref mcr_DispatchReceiver.\n
 *  	1.2 \ref mcr_Trigger_receive may be used to dispatch into \ref mcr_Trigger.\n
 *  	1.3 Triggered action may be a \ref mcr_Macro, which sends a list of \ref mcr_Signal.\n
 *  2. If signal is not blocked by dispatching, it is then sent to cause an action.\n
 */

//! \todo Separate context and functions to context.h?

#ifndef MCR_LIBMACRO_H_
#define MCR_LIBMACRO_H_

// Includes all modules
#include "mcr/context.h"

#ifdef __cplusplus
#ifndef MCR_NOEXTRAS
	#include "mcr/extras/extras.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! \ref malloc and \ref mcr_initialize a \ref mcr_context
 *
 *  Will not load contracts.
 *  Will set \ref mcr_err.
 *  \return Dynamic and initialized Libmacro context, or NULL on error
 */
extern MCR_API struct mcr_context *mcr_allocate();
/*! \ref mcr_deinitialize and \ref free.
 *
 *  Only use with a context created by malloc or \ref mcr_allocate.
 *  Because of threading do not deallocate in a deconstructor or on program
 *  exit.
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
extern MCR_API int mcr_deallocate(struct mcr_context *ctx);
/*! Initialize Libmacro resources
 *
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
extern MCR_API int mcr_initialize(struct mcr_context *ctx);
/*! Clean all resources used by Libmacro.
 *
 *  Because of threading do not deinitialize in a deconstructor or on program
 *  exit.
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
extern MCR_API int mcr_deinitialize(struct mcr_context *ctx);
/*! Register standard types and map their given names.
 *
 *  \pre context must be initialized
 *  \ref mcr_base.isignal_registry_pt
 *  \ref mcr_base.itrigger_registry_pt
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
extern MCR_API int mcr_load_contracts(struct mcr_context *ctx);
/*! Minimize allocation used by Libmacro.
 *
 *  \param ctx Libmacro context
 */
extern MCR_API void mcr_trim(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
