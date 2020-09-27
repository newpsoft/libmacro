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

void mcr_ISignal_init(struct mcr_ISignal *isigPt)
{
	if (isigPt) {
		mcr_Interface_init(&isigPt->interface);
		isigPt->dispatcher_pt = NULL;
		isigPt->send = NULL;
	}
}

void mcr_ISignal_ctor(struct mcr_ISignal *isigPt,
					  struct mcr_context *context, size_t dataSize, void *(*allocate)(),
					  void (*deallocate)(void *), mcr_data_fnc init, mcr_data_fnc deinit,
					  mcr_compare_fnc compare, mcr_copy_fnc copy, struct mcr_IDispatcher *dispPt,
					  mcr_signal_fnc sender)
{
	if (isigPt) {
		mcr_Interface_ctor(&isigPt->interface, context, dataSize, allocate, deallocate,
						   init, deinit, compare, copy);
		isigPt->dispatcher_pt = dispPt;
		isigPt->send = sender;
	}
}

void mcr_ISignal_deinit(struct mcr_ISignal *isigPt)
{
	if (isigPt)
		mcr_Interface_deinit(&isigPt->interface);
}

struct mcr_IRegistry *mcr_ISignal_registry(struct mcr_context *ctx)
{
	dassert(ctx);
	return ctx->base.isignal_registry_pt;
}

size_t mcr_ISignal_id(const struct mcr_ISignal *isigPt)
{
	return isigPt ? isigPt->interface.id : (size_t)~0;
}

struct mcr_ISignal *mcr_ISignal_from_id(struct mcr_context *ctx, size_t id)
{
	dassert(ctx);
	return (struct mcr_ISignal *)mcr_IRegistry_from_id(mcr_ISignal_registry(ctx),
			id);
}

const char *mcr_ISignal_name(struct mcr_context *ctx,
							 struct mcr_ISignal *isigPt)
{
	dassert(ctx);
	return mcr_IRegistry_name(mcr_ISignal_registry(ctx), &isigPt->interface);
}

struct mcr_ISignal *mcr_ISignal_from_name(struct mcr_context *ctx,
		const char *name)
{
	dassert(ctx);
	return (struct mcr_ISignal *)mcr_IRegistry_from_name(mcr_ISignal_registry(ctx),
			name);
}
