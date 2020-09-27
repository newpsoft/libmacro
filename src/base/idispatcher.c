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
#include <string.h>

void mcr_IDispatcher_ctor(struct mcr_IDispatcher *idispPt,
						  struct mcr_context *ctx, void (*deinit)(struct mcr_IDispatcher *),
						  mcr_IDispatcher_add_fnc add,
						  void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_dispatch_fnc dispatch,
						  mcr_IDispatcher_modify_fnc modifier, mcr_IDispatcher_remove_fnc remove,
						  void (*trim)(struct mcr_IDispatcher *))
{
	if (idispPt) {
		*idispPt = (struct mcr_IDispatcher) {
			.ctx = ctx,
			.deinit = deinit,
			.add = add,
			.clear = clear,
			.dispatch = dispatch,
			.modifier = modifier,
			.remove = remove,
			.trim = trim
		};
	}
}

/* Dispatchers */
struct mcr_IDispatcher *mcr_IDispatcher_from_id(struct mcr_context *ctx,
		size_t signalTypeId)
{
	dassert(ctx);
	mcr_err = 0;
	if (signalTypeId == (size_t) ~ 0)
		return ctx->base.generic_dispatcher_pt;
	if (signalTypeId >= ctx->base.dispatcher_count) {
		mset_error(EINVAL);
		return NULL;
	}
	return ctx->base.dispatchers[signalTypeId];
}
