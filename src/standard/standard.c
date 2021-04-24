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
#include "mcr/platform.h"
#include "mcr/private.h"

#include <stdlib.h>

int mcr_modifier_key(struct mcr_context *ctx, unsigned int modifiers)
{
	dassert(ctx);
	struct mcr_MapElement *found;
	if (ctx->standard.modifier_key_count) {
		dassert(ctx->standard.modifier_keys);
		found = bsearch(&modifiers, ctx->standard.modifier_keys,
						ctx->standard.modifier_key_count,
						sizeof(*found), mcr_unsigned_compare);
		if (found)
			return found->second.integer;
	}
	return MCR_KEY_ANY;
}

unsigned int mcr_key_modifier(struct mcr_context *ctx, int key)
{
	dassert(ctx);
	struct mcr_MapElement *found;
	if (ctx->standard.key_modifier_count) {
		dassert(ctx->standard.key_modifiers);
		found = bsearch(&key, ctx->standard.key_modifiers,
						ctx->standard.key_modifier_count,
						sizeof(*found), mcr_int_compare);
		if (found)
			return found->second.u_integer;
	}
	return 0;
}

bool mcr_resembles_justified(const mcr_Dimensions first,
							 const mcr_Dimensions second)
{
	dassert(first);
	dassert(second);
	int i;
	for (i = MCR_DIMENSION_COUNT; i--;) {
		if (first[i] == 0 || second[i] == 0)
			continue;
		if ((first[i] > 0) != (second[i] > 0))
			return false;
	}
	return true;
}

bool mcr_resembles_absolute(const mcr_Dimensions first,
							const mcr_Dimensions second, const unsigned int measurementError)
{
	dassert(first);
	dassert(second);
	long long err;
	int i;
	for (i = MCR_DIMENSION_COUNT; i--;) {
		err = first[i] - second[i];
		if (err < 0)
			err *= -1;
		if (err > measurementError)
			return false;
	}
	return true;
}

#define SIZE(s) sizeof(struct s)
#define SEND(s) s ## _send
int mcr_standard_initialize(struct mcr_context *ctx)
{
	struct mcr_ISignal *isigs[] = {
		mcr_iHidEcho(ctx), mcr_iKey(ctx), mcr_iModifier(ctx),
		mcr_iMoveCursor(ctx), mcr_iNoOp(ctx), mcr_iScroll(ctx)
	};
	size_t sigSizes[] = {
		SIZE(mcr_HidEcho), SIZE(mcr_Key),
		SIZE(mcr_Modifier), SIZE(mcr_MoveCursor),
		SIZE(mcr_NoOp), SIZE(mcr_Scroll)
	};
	mcr_signal_fnc sendFns[] = {
		SEND(mcr_HidEcho), SEND(mcr_Key), SEND(mcr_Modifier),
		SEND(mcr_MoveCursor), SEND(mcr_NoOp), SEND(mcr_Scroll)
	};
	/* signals */
	size_t i;
	for (i = 0; i < arrlen(isigs); i++) {
		mcr_ISignal_ctor(isigs[i], ctx, sigSizes[i], NULL, NULL, NULL, NULL,
						 NULL, NULL, NULL, sendFns[i]);
	}

	/* triggers */
	mcr_ITrigger_ctor(mcr_iAction(ctx), ctx, SIZE(mcr_Action), NULL, NULL,
					  mcr_Action_init_, NULL, NULL, NULL, mcr_Action_receive);

	if (mcr_standard_platform_initialize(ctx))
		return mcr_err;

	ctx->standard.key_modifiers = mcr_key_modifier_defaults;
	ctx->standard.key_modifier_count = mcr_key_modifier_default_count;
	ctx->standard.modifier_keys = mcr_modifier_key_defaults;
	ctx->standard.modifier_key_count = mcr_modifier_key_default_count;
	return 0;
}
#undef SIZE
#undef SEND

int mcr_standard_deinitialize(struct mcr_context *ctx)
{
	return mcr_standard_platform_deinitialize(ctx);
}
