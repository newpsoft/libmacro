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

#include <errno.h>
#include <stdio.h>

int mcr_send(struct mcr_context *ctx, struct mcr_Signal *sigPt)
{
	if (!sigPt)
		return 0;
	if (sigPt->dispatch_flag && mcr_dispatch(ctx, sigPt))
		return 0;
	return sigPt->isignal ? sigPt->isignal->send(sigPt) : 0;
}

/* Public access modifiers */
unsigned int *mcr_modifiers(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->modifiers;
}

void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods)
{
	dassert(ctx);
	ctx->modifiers |= addMods;
}

void mcr_remove_modifiers(struct mcr_context *ctx, unsigned int remMods)
{
	dassert(ctx);
	ctx->modifiers &= (~remMods);
}

int mcr_base_initialize(struct mcr_context *ctx)
{
	struct mcr_base *modBase = &ctx->base;
	int err;
	*modBase = (struct mcr_base) {
		0
	};
	err = mtx_init(&modBase->dispatch_lock, mtx_plain);
	if (err) {
		err = mcr_thrd_errno(err);
		error_set_return(err);
	}
	mcr_err = 0;
	modBase->generic_dispatcher_flag = true;
	return 0;
}

int mcr_base_deinitialize(struct mcr_context *ctx)
{
	struct mcr_base *modBase = &ctx->base;
	dassert(ctx);
	mcr_err = 0;
	for (size_t i = 0; i < modBase->dispatcher_count; i++) {
		if (modBase->dispatchers[i]->deinit)
			modBase->dispatchers[i]->deinit(modBase->dispatchers[i]);
	}
	mcr_IRegistry_deinit(modBase->itrigger_registry_pt);
	mcr_IRegistry_deinit(modBase->isignal_registry_pt);
	mtx_destroy(&modBase->dispatch_lock);
	return mcr_err;
}

void mcr_base_trim(struct mcr_context *ctx)
{
	mcr_IRegistry_trim(mcr_ISignal_registry(ctx));
	mcr_IRegistry_trim(mcr_ITrigger_registry(ctx));
	mcr_dispatch_trim_all(ctx);
}
