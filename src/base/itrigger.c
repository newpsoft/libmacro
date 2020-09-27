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

#include "mcr/libmacro.h"

void mcr_ITrigger_init(struct mcr_ITrigger *itrigPt)
{
	if (itrigPt) {
		mcr_Interface_init(&itrigPt->interface);
		itrigPt->receive = NULL;
	}
}

void mcr_ITrigger_ctor(struct mcr_ITrigger *itrigPt,
					   struct mcr_context *context, size_t dataSize, void *(*allocate)(),
					   void (*deallocate)(void *), mcr_data_fnc init, mcr_data_fnc deinit,
					   mcr_compare_fnc compare, mcr_copy_fnc copy, mcr_Trigger_receive_fnc receiveFnc)
{
	if (itrigPt) {
		mcr_Interface_ctor(&itrigPt->interface, context, dataSize, allocate, deallocate,
						   init, deinit, compare, copy);
		itrigPt->receive = receiveFnc;
	}
}

void mcr_ITrigger_deinit(struct mcr_ITrigger *itrigPt)
{
	if (itrigPt)
		mcr_Interface_deinit(&itrigPt->interface);
}

struct mcr_IRegistry *mcr_ITrigger_registry(struct mcr_context *ctx)
{
	dassert(ctx);
	return ctx->base.itrigger_registry_pt;
}

size_t mcr_ITrigger_id(struct mcr_ITrigger *itrigPt)
{
	return itrigPt ? itrigPt->interface.id : (size_t)~0;
}

struct mcr_ITrigger *mcr_ITrigger_from_id(struct mcr_context *ctx, size_t id)
{
	dassert(ctx);
	return (struct mcr_ITrigger *)mcr_IRegistry_from_id(mcr_ITrigger_registry(ctx),
			id);
}

const char *mcr_ITrigger_name(struct mcr_context *ctx,
							  struct mcr_ITrigger *itrigPt)
{
	dassert(ctx);
	return mcr_IRegistry_name(mcr_ITrigger_registry(ctx), &itrigPt->interface);
}

struct mcr_ITrigger *mcr_ITrigger_from_name(struct mcr_context *ctx,
		const char *name)
{
	dassert(ctx);
	return (struct mcr_ITrigger *)mcr_IRegistry_from_name(mcr_ITrigger_registry(
				ctx), name);
}
