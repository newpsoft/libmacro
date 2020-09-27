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

#include <stdio.h>
#include <errno.h>

static MCR_INLINE void mcr_dispatch_set_dispatcher(struct mcr_context *ctx,
		struct mcr_ISignal *isigPt, size_t idispatcherId)
{
	dassert(ctx);
	dassert(isigPt);
	isigPt->dispatcher_pt = mcr_IDispatcher_from_id(ctx, idispatcherId);
}

int mcr_dispatch_lock(struct mcr_context *ctx)
{
	dassert(ctx);
	int err = mtx_lock(&ctx->base.dispatch_lock);
	if (err) {
		err = mcr_thrd_errno(err);
		error_set_return(err);
	}
	mcr_err = 0;
	return 0;
}

int mcr_dispatch_unlock(struct mcr_context *ctx)
{
	dassert(ctx);
	int err = mtx_unlock(&ctx->base.dispatch_lock);
	if (err) {
		err = mcr_thrd_errno(err);
		error_set_return(err);
	}
	mcr_err = 0;
	return 0;
}

bool mcr_dispatch(struct mcr_context *ctx, struct mcr_Signal *sigPt)
{
	dassert(ctx);
	struct mcr_base *modBase = &ctx->base;
	struct mcr_IDispatcher *dispPt = sigPt && sigPt->isignal ? sigPt->isignal->dispatcher_pt : NULL;
	struct mcr_IDispatcher *genPt = modBase->generic_dispatcher_pt;
	bool isGen = genPt && modBase->generic_dispatcher_flag;
	unsigned int *modsPt = &ctx->modifiers;
	if (mcr_dispatch_lock(ctx)) {
		dmsg;
		return false;
	}
	/* If found only dispatch to enabled methods. */
	if (dispPt) {
		dassert(dispPt->dispatch);
		if (dispPt->dispatch(dispPt, sigPt, *modsPt)) {
			mcr_dispatch_unlock(ctx);
			return true;
		}
		if (dispPt->modifier)
			dispPt->modifier(dispPt, sigPt, modsPt);
	}
	if (isGen) {
		dassert(genPt->dispatch);
		if (genPt->dispatch(genPt, sigPt, *modsPt)) {
			mcr_dispatch_unlock(ctx);
			return true;
		}
		if (genPt->modifier)
			genPt->modifier(genPt, sigPt, modsPt);
	}
	mcr_dispatch_unlock(ctx);
	return false;
}

int mcr_dispatch_set_dispatchers(struct mcr_context *ctx, struct mcr_IDispatcher **dispatchers, size_t count)
{
	if (count && !dispatchers)
		error_set_return(EINVAL);
	/* Lock for array of dispatchers. */
	if (mcr_dispatch_lock(ctx)) {
		dmsg;
		return mcr_err;
	}
	ctx->base.dispatchers = dispatchers;
	ctx->base.dispatcher_count = count;
	return mcr_dispatch_unlock(ctx);
}

void mcr_dispatch_set_generic_dispatcher(struct mcr_context *ctx, struct mcr_IDispatcher *dispatcherPt)
{
	/* Just one pointer, no need to lock. */
	ctx->base.generic_dispatcher_pt = dispatcherPt;
}

size_t mcr_dispatch_count(struct mcr_context *ctx)
{
	dassert(ctx);
	return ctx->base.dispatcher_count;
}

bool mcr_dispatch_enabled(struct mcr_context *ctx,
							 struct mcr_ISignal *isigPt)
{
	dassert(ctx);
	return isigPt ? !!isigPt->dispatcher_pt :
		   ctx->base.generic_dispatcher_flag &&
		   ctx->base.generic_dispatcher_pt;
}

void mcr_dispatch_set_enabled(struct mcr_context *ctx,
							  struct mcr_ISignal *isigPt, bool enableFlag)
{
	dassert(ctx);
	bool lockFlag = !mcr_dispatch_lock(ctx);
	if (isigPt) {
		if (enableFlag) {
			mcr_dispatch_set_dispatcher(ctx, isigPt, mcr_ISignal_id(isigPt));
		} else {
			isigPt->dispatcher_pt = NULL;
		}
	} else {
		ctx->base.generic_dispatcher_flag = enableFlag;
	}
	if (lockFlag)
		mcr_dispatch_unlock(ctx);
}

void mcr_dispatch_set_enabled_all(struct mcr_context *ctx, bool enableFlag)
{
	size_t i, dispMaxi = ctx->base.dispatcher_count;
	struct mcr_ISignal *isigPt;
	dassert(dispMaxi <= mcr_IRegistry_count(ctx->base.isignal_registry_pt));
	bool lockFlag = !mcr_dispatch_lock(ctx);
	for (i = dispMaxi; i--;) {
		isigPt = mcr_ISignal_from_id(ctx, i);
		mcr_dispatch_set_dispatcher(ctx, isigPt, i);
	}
	if (lockFlag)
		mcr_dispatch_unlock(ctx);
	mcr_dispatch_set_enabled(ctx, NULL, enableFlag);
}

int mcr_dispatch_add(struct mcr_context *ctx,
					 struct mcr_Signal *interceptPt, void *receiver,
					 mcr_dispatch_receive_fnc receiveFnc)
{
	struct mcr_IDispatcher *dispPt =
		mcr_IDispatcher_from_id(ctx, mcr_ISignal_id(interceptPt->isignal));
	dassert(receiveFnc);
	/* mcr_Trigger_receive requires an object */
	if ((receiveFnc == mcr_Trigger_receive || receiveFnc == mcr_Macro_receive)
		&& !receiver) {
		error_set_return(EINVAL);
	}
	mcr_err = 0;
	if (dispPt && dispPt->add) {
		if (mcr_dispatch_lock(ctx)) {
			dmsg;
			return mcr_err;
		}
		mcr_err = dispPt->add(dispPt, interceptPt, receiver, receiveFnc);
		/* Not likely to error when locked */
		mcr_dispatch_unlock(ctx);
	}
	return mcr_err;
}

int mcr_dispatch_add_generic(struct mcr_context *ctx,
							 struct mcr_Signal *interceptPt,
							 void *receiver, mcr_dispatch_receive_fnc receiveFnc)
{
	struct mcr_IDispatcher *genPt = ctx->base.generic_dispatcher_pt;
	dassert(receiveFnc);
	/* mcr_Trigger_receive requires an object */
	if ((receiveFnc == mcr_Trigger_receive || receiveFnc == mcr_Macro_receive)
		&& !receiver) {
		error_set_return(EINVAL);
	}
	mcr_err = 0;
	if (genPt && genPt->add) {
		if (mcr_dispatch_lock(ctx)) {
			dmsg;
			return mcr_err;
		}
		mcr_err = genPt->add(genPt, interceptPt, receiver, receiveFnc);
		/* Not likely to error when locked */
		mcr_dispatch_unlock(ctx);
	}
	return mcr_err;
}

void mcr_dispatch_clear(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_IDispatcher *dispPt =
		mcr_IDispatcher_from_id(ctx, mcr_Interface_id(&isigPt->interface));
	bool lockFlag;
	if (dispPt && dispPt->clear) {
		lockFlag = !mcr_dispatch_lock(ctx);
		dispPt->clear(dispPt);
		if (lockFlag)
			mcr_dispatch_unlock(ctx);
	}
}

void mcr_dispatch_clear_all(struct mcr_context *ctx)
{
	struct mcr_IDispatcher *dispPt;
	struct mcr_IDispatcher *genPt = ctx->base.generic_dispatcher_pt;
	size_t i = mcr_dispatch_count(ctx);
	bool lockFlag = !mcr_dispatch_lock(ctx);
	while (i--) {
		dispPt = mcr_IDispatcher_from_id(ctx, i);
		if (dispPt && dispPt->clear)
			dispPt->clear(dispPt);
	}
	if (genPt && genPt->clear)
		genPt->clear(genPt);
	if (lockFlag)
		mcr_dispatch_unlock(ctx);
}

void mcr_dispatch_modify(struct mcr_context *ctx,
						  struct mcr_Signal *sigPt,
						  unsigned int *modifiersPt)
{
	dassert(ctx);
	struct mcr_base *modBase = &ctx->base;
	struct mcr_IDispatcher *dispPt =
			sigPt && sigPt->isignal ? sigPt->isignal->dispatcher_pt : NULL, *genPt =
				modBase->generic_dispatcher_pt;
	bool isGen = genPt && modBase->generic_dispatcher_flag;
	if (mcr_dispatch_lock(ctx)) {
		dmsg;
		return;
	}
	/* If found only dispatch to enabled methods. */
	if (dispPt && dispPt->modifier)
		dispPt->modifier(dispPt, sigPt, modifiersPt);
	if (isGen && genPt->modifier)
		genPt->modifier(genPt, sigPt, modifiersPt);
	mcr_dispatch_unlock(ctx);
}

void mcr_dispatch_remove(struct mcr_context *ctx,
						 struct mcr_ISignal *isigPt, void *remReceiver)
{
	struct mcr_IDispatcher *dispPt =
		mcr_IDispatcher_from_id(ctx, mcr_Interface_id(&isigPt->interface));
	bool lockFlag;
	if (dispPt && dispPt->remove) {
		lockFlag = !mcr_dispatch_lock(ctx);
		dispPt->remove(dispPt, remReceiver);
		if (lockFlag)
			mcr_dispatch_unlock(ctx);
	}
}

void mcr_dispatch_remove_all(struct mcr_context *ctx, void *remReceiver)
{
	struct mcr_IDispatcher *dispPt;
	struct mcr_IDispatcher *genPt = ctx->base.generic_dispatcher_pt;
	size_t i = mcr_dispatch_count(ctx);
	bool lockFlag = !mcr_dispatch_lock(ctx);
	while (i--) {
		dispPt = mcr_IDispatcher_from_id(ctx, i);
		if (dispPt && dispPt->remove)
			dispPt->remove(dispPt, remReceiver);
	}
	if (genPt && genPt->remove)
		genPt->remove(genPt, remReceiver);
	if (lockFlag)
		mcr_dispatch_unlock(ctx);
}

/* Trim does not remove, so no need to lock. */
void mcr_dispatch_trim(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_IDispatcher *dispPt =
		mcr_IDispatcher_from_id(ctx, mcr_Interface_id(&isigPt->interface));
	if (dispPt && dispPt->trim)
		dispPt->trim(dispPt);
}

void mcr_dispatch_trim_all(struct mcr_context *ctx)
{
	struct mcr_IDispatcher *dispPt;
	struct mcr_IDispatcher *genPt = ctx->base.generic_dispatcher_pt;
	size_t i = mcr_dispatch_count(ctx);
	while (i--) {
		dispPt = mcr_IDispatcher_from_id(ctx, i);
		if (dispPt && dispPt->trim)
			dispPt->trim(dispPt);
	}
	if (genPt && genPt->trim)
		genPt->trim(genPt);
}
