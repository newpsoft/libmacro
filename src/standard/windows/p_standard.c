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

#include "mcr/standard/windows/p_standard.h"
#include "mcr/libmacro.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define CALC_ABS_X(x) \
((x * 65535) / GetSystemMetrics(SM_CXVIRTUALSCREEN))

#define CALC_ABS_Y(y) \
((y * 65535) / GetSystemMetrics(SM_CYVIRTUALSCREEN))

/* alt, ctrl, shift, win modifiers, each with left and right key. alt ctrl
 * shift also have a separate code for key state winapi. */
static struct mcr_MapElement mcr_key_modifier_impl[4 + 7];
static struct mcr_MapElement mcr_modifier_key_impl[4];

MCR_API const struct mcr_MapElement *const mcr_key_modifier_defaults = mcr_key_modifier_impl;
MCR_API const size_t mcr_key_modifier_default_count = arrlen(mcr_key_modifier_impl);
MCR_API const struct mcr_MapElement *const mcr_modifier_key_defaults = mcr_modifier_key_impl;
MCR_API const size_t mcr_modifier_key_default_count = arrlen(mcr_modifier_key_impl);

/* Left, middle, right, x. down and up */
MCR_API const DWORD mcr_standard_echo_flag_defaults[] = { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP };
MCR_API size_t mcr_standard_echo_flag_default_count = arrlen(mcr_standard_echo_flag_defaults);

void mcr_cursor_position(struct mcr_context *ctx, mcr_SpacePosition buffer)
{
	POINT p;
	UNUSED(ctx);
	if (GetCursorPos(&p)) {
		buffer[MCR_X] = p.x;
		buffer[MCR_Y] = p.y;
		buffer[MCR_Z] = 0;
		mcr_err = 0;
	} else {
		mset_error(EINTR);
	}
}

int mcr_HidEcho_send_member(struct mcr_HidEcho *echoPt, struct mcr_context *ctx)
{
	dassert(echoPt);
	dassert(ctx);
	if (echoPt->echo < ctx->standard.platform_pt->echo_flag_count) {
		mouse_event(ctx->standard.platform_pt->echo_flags[echoPt->echo], 0, 0, 0, 0);
	}
	return 0;
}

size_t mcr_HidEcho_count(struct mcr_context *ctx)
{
	if (!ctx)
		return 0;
	dassert(ctx->standard.platform_pt);
	return ctx->standard.platform_pt->echo_flag_count;
}

int mcr_HidEcho_set_mouse_flags(struct mcr_context *ctx, DWORD *echoMouseEventFlags, size_t count)
{
	dassert(ctx);
	if (count)
		dassert(echoMouseEventFlags);
	ctx->standard.platform_pt->echo_flags = echoMouseEventFlags;
	ctx->standard.platform_pt->echo_flag_count = count;
	return 0;
}

int mcr_Key_send_member(struct mcr_Key *keyPt, struct mcr_context *ctx)
{
	UNUSED(ctx);
	if (keyPt->apply != MCR_UNSET) {
		keybd_event((BYTE) keyPt->key, (BYTE) keyPt->key,
					KEYEVENTF_EXTENDEDKEY, 0);
	}
	if (keyPt->apply != MCR_SET) {
		keybd_event((BYTE) keyPt->key, (BYTE) keyPt->key,
					KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
	return 0;
}

int mcr_MoveCursor_send_member(struct mcr_MoveCursor *mcPt, struct mcr_context *ctx)
{
	UNUSED(ctx);
	mouse_event(mcPt->justify_flag ? MOUSEEVENTF_MOVE :
				MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
				(DWORD) mcPt->position[MCR_X], (DWORD) mcPt->position[MCR_Y], 0, 0);
	return 0;
}

int mcr_Scroll_send_member(struct mcr_Scroll *scrPt, struct mcr_context *ctx)
{
	UNUSED(ctx);
	if (scrPt->dimensions[MCR_Y])
		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, (DWORD) scrPt->dimensions[MCR_Y], 0);
	if (scrPt->dimensions[MCR_X])
		mouse_event(MOUSEEVENTF_HWHEEL, 0, 0, (DWORD) scrPt->dimensions[MCR_X], 0);
	return 0;
}

static void initialize_key_modifiers()
{
	const unsigned int modifiers[] = { MCR_ALT, MCR_CTRL, MCR_SHIFT, MCR_WIN };
	const int setKeys[] = { VK_LMENU, VK_LCONTROL, VK_LSHIFT, VK_LWIN };
	const unsigned int addModifiers[] = { MCR_ALT, MCR_ALT, MCR_CTRL, MCR_CTRL, MCR_SHIFT, MCR_SHIFT, MCR_WIN };
	const int addKeys[] = { VK_RMENU, VK_MENU, VK_RCONTROL, VK_CONTROL, VK_RSHIFT, VK_SHIFT, VK_RWIN };
	const size_t COUNT = arrlen(modifiers), ADDCOUNT = arrlen(addModifiers);
	size_t i;
	dassert(arrlen(mcr_modifier_key_impl) == COUNT);
	dassert(arrlen(mcr_key_modifier_impl) == COUNT + ADDCOUNT);
	for (i = 0; i < COUNT; i++) {
		/* modifier => key */
		mcr_modifier_key_impl[i].first.u_integer = modifiers[i];
		mcr_modifier_key_impl[i].second.integer = setKeys[i];
		/* key => modifier */
		mcr_key_modifier_impl[i].first.integer = setKeys[i];
		mcr_key_modifier_impl[i].second.u_integer = modifiers[i];
	}
	/* Map additional key to modifiers. */
	for (i = 0; i < ADDCOUNT; i++) {
		/* key => add modifier */
		mcr_key_modifier_impl[i + COUNT].first.integer = addKeys[i];
		mcr_key_modifier_impl[i + COUNT].second.u_integer = addModifiers[i];
	}
	qsort(mcr_modifier_key_impl, arrlen(mcr_modifier_key_impl), sizeof(struct mcr_MapElement), mcr_unsigned_compare);
	qsort(mcr_key_modifier_impl, arrlen(mcr_key_modifier_impl), sizeof(struct mcr_MapElement), mcr_int_compare);
}

int mcr_standard_platform_initialize(struct mcr_context *context)
{
	// not thread safe
	initialize_key_modifiers();

	context->standard.platform_pt = malloc(sizeof(struct mcr_standard_platform));
	if (!context->standard.platform_pt)
		error_set_return(ENOMEM);
	context->standard.platform_pt->echo_flags = mcr_standard_echo_flag_defaults;
	context->standard.platform_pt->echo_flag_count = mcr_standard_echo_flag_default_count;

	return 0;
}

int mcr_standard_platform_deinitialize(struct mcr_context *context)
{
	free(context->standard.platform_pt);
	context->standard.platform_pt = NULL;
	return 0;
}

void mcr_standard_set_echo_flags(struct mcr_context *ctx, const DWORD *mouseEventFlags, size_t count)
{
	dassert(ctx);
	ctx->standard.platform_pt->echo_flags = mouseEventFlags;
	ctx->standard.platform_pt->echo_flag_count = count;
}
