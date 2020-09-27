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

#include "mcr/standard/linux/p_standard.h"
#include "mcr/libmacro.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct mcr_MapElement mcr_key_modifier_impl[7 + 5];
static struct mcr_MapElement mcr_modifier_key_impl[7];

MCR_API const struct mcr_MapElement *const mcr_key_modifier_defaults = mcr_key_modifier_impl;
MCR_API const size_t mcr_key_modifier_default_count = arrlen(mcr_key_modifier_impl);
MCR_API const struct mcr_MapElement *const mcr_modifier_key_defaults = mcr_modifier_key_impl;
MCR_API const size_t mcr_modifier_key_default_count = arrlen(mcr_modifier_key_impl);

/* Press + Release */
#define LOCAL_ELEMENT(codeValue) \
{ .type = EV_KEY, .code = codeValue, .value = 1 }, \
{ .type = EV_KEY, .code = codeValue, .value = 0 }

static const struct input_event mcr_standard_echo_event_impl[MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT] = {
	LOCAL_ELEMENT(BTN_LEFT), LOCAL_ELEMENT(BTN_MIDDLE),
	LOCAL_ELEMENT(BTN_RIGHT), LOCAL_ELEMENT(BTN_0),
	LOCAL_ELEMENT(BTN_1), LOCAL_ELEMENT(BTN_2),
	LOCAL_ELEMENT(BTN_3), LOCAL_ELEMENT(BTN_4),
	LOCAL_ELEMENT(BTN_5), LOCAL_ELEMENT(BTN_6),
	LOCAL_ELEMENT(BTN_7), LOCAL_ELEMENT(BTN_8),
	LOCAL_ELEMENT(BTN_9), LOCAL_ELEMENT(BTN_SIDE),
	LOCAL_ELEMENT(BTN_EXTRA), LOCAL_ELEMENT(BTN_FORWARD),
	LOCAL_ELEMENT(BTN_BACK), LOCAL_ELEMENT(BTN_TASK),
	LOCAL_ELEMENT(BTN_TRIGGER), LOCAL_ELEMENT(BTN_THUMB),
	LOCAL_ELEMENT(BTN_THUMB2), LOCAL_ELEMENT(BTN_TOP),
	LOCAL_ELEMENT(BTN_TOP2), LOCAL_ELEMENT(BTN_PINKIE),
	LOCAL_ELEMENT(BTN_BASE), LOCAL_ELEMENT(BTN_BASE2),
	LOCAL_ELEMENT(BTN_BASE3), LOCAL_ELEMENT(BTN_BASE4),
	LOCAL_ELEMENT(BTN_BASE5), LOCAL_ELEMENT(BTN_BASE6),
	LOCAL_ELEMENT(BTN_A /* BTN_SOUTH */), LOCAL_ELEMENT(BTN_B /* BTN_EAST */),
	LOCAL_ELEMENT(BTN_C), LOCAL_ELEMENT(BTN_X /* BTN_NORTH */),
	LOCAL_ELEMENT(BTN_Y /* BTN_WEST */), LOCAL_ELEMENT(BTN_Z),
	LOCAL_ELEMENT(BTN_TL), LOCAL_ELEMENT(BTN_TR),
	LOCAL_ELEMENT(BTN_TL2), LOCAL_ELEMENT(BTN_TR2),
	LOCAL_ELEMENT(BTN_SELECT), LOCAL_ELEMENT(BTN_START),
	LOCAL_ELEMENT(BTN_MODE), LOCAL_ELEMENT(BTN_THUMBL),
	LOCAL_ELEMENT(BTN_THUMBR)
};
#undef LOCAL_ELEMENT

MCR_API const struct input_event *const mcr_standard_echo_event_defaults = mcr_standard_echo_event_impl;

void mcr_standard_set_echo_events(struct mcr_context *ctx, const struct input_event *echoEvents, size_t count)
{
	ctx->standard.platform_pt->echo_events = echoEvents;
	ctx->standard.platform_pt->echo_event_count = count;
}

void mcr_cursor_position(struct mcr_context *ctx, mcr_SpacePosition buffer)
{
	int i;
	for (i = MCR_DIMENSION_COUNT; i--;)
		buffer[i] = ctx->standard.platform_pt->cursor[i];
}

int mcr_HidEcho_send_member(struct mcr_HidEcho *echoPt, struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	if (echoPt->echo < standardPt->echo_event_count) {
		if (mcr_Device_single(&standardPt->gen_dev, standardPt->echo_events + echoPt->echo)) {
			dmsg;
			return mcr_err;
		}
		return mcr_Device_sync(&standardPt->gen_dev);
	} else {
		error_set_return(EFAULT);
	}
	return (mcr_err = 0);
}

size_t mcr_HidEcho_count(struct mcr_context *ctx)
{
	if (!ctx)
		return 0;
	dassert(ctx->standard.platform_pt);
	return ctx->standard.platform_pt->echo_event_count;
}

int mcr_Key_send_member(struct mcr_Key *keyPt, struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	struct input_event events[2] = {{.type = EV_KEY, .code = keyPt->key}, {.type = EV_SYN, .code = SYN_REPORT}};
	mcr_err = 0;
	if (keyPt->apply != MCR_UNSET) {
		events[0].value = 1;
		if (mcr_Device_send(&standardPt->gen_dev, events, sizeof(events)))
			return mcr_err;
	}
	if (keyPt->apply != MCR_SET) {
		events[0].value = 0;
		if (mcr_Device_send(&standardPt->gen_dev, events, sizeof(events)))
			return mcr_err;
	}
	return 0;
}

int mcr_MoveCursor_send_member(struct mcr_MoveCursor *mcPt, struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	struct input_event events[MCR_DIMENSION_COUNT + 1] = {};
	events[MCR_X].value = mcPt->position[MCR_X];
	events[MCR_Y].value = mcPt->position[MCR_Y];
	events[MCR_Z].value = mcPt->position[MCR_Z];
	events[MCR_DIMENSION_COUNT].type = EV_SYN;
	events[MCR_DIMENSION_COUNT].code = SYN_REPORT;
	if (mcPt->justify_flag) {
		events[0].type = events[1].type = events[2].type = EV_REL;
		events[MCR_X].code = REL_X;
		events[MCR_Y].code = REL_Y;
		events[MCR_Z].code = REL_Z;
		if (mcr_Device_send(&standardPt->gen_dev, events, sizeof(events)))
			return mcr_err;
	} else {
		events[0].type = events[1].type = events[2].type = EV_ABS;
		events[MCR_X].code = ABS_X;
		events[MCR_Y].code = ABS_Y;
		events[MCR_Z].code = ABS_Z;
		if (mcr_Device_send(&standardPt->abs_dev, events, sizeof(events)))
			return mcr_err;
		standardPt->cursor[MCR_X] = mcPt->position[MCR_X];
		standardPt->cursor[MCR_Y] = mcPt->position[MCR_Y];
		standardPt->cursor[MCR_Z] = mcPt->position[MCR_Z];
	}
	return 0;
}

int mcr_Scroll_send_member(struct mcr_Scroll *scrPt, struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	struct input_event events[MCR_DIMENSION_COUNT + 1] = {};
	events[0].type = events[1].type = events[2].type = EV_REL;
	events[MCR_X].code = REL_HWHEEL;
	events[MCR_Y].code = REL_WHEEL;
	events[MCR_Z].code = REL_DIAL;
	events[MCR_X].value = scrPt->dimensions[MCR_X];
	events[MCR_Y].value = scrPt->dimensions[MCR_Y];
	events[MCR_Z].value = scrPt->dimensions[MCR_Z];
	events[MCR_DIMENSION_COUNT].type = EV_SYN;
	events[MCR_DIMENSION_COUNT].code = SYN_REPORT;
	return mcr_Device_send(&standardPt->gen_dev, events, sizeof(events));
}

static void initialize_key_modifiers()
{
	/* Right extras */
	const unsigned int modifiers[] = { MCR_ALT, MCR_COMPOSE, MCR_CTRL, MCR_FN, MCR_FRONT, MCR_SHIFT, MCR_META };
	const int setKeys[] = { KEY_LEFTALT, KEY_COMPOSE, KEY_LEFTCTRL, KEY_FN, KEY_FRONT, KEY_LEFTSHIFT, KEY_LEFTMETA };
	/* Note: Alt and option are the same, but we use alt explicitely to avoid confusion. */
	const unsigned int addModifiers[] = { MCR_ALT, MCR_ALT, MCR_CTRL, MCR_SHIFT, MCR_META };
	const int addKeys[] = { KEY_OPTION, KEY_RIGHTALT, KEY_RIGHTCTRL, KEY_RIGHTSHIFT, KEY_RIGHTMETA };
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
	struct mcr_standard_platform *standardPt = malloc(sizeof(struct mcr_standard_platform));
	int mtxErr;

	initialize_key_modifiers();

	if (!standardPt)
		error_set_return(ENOMEM);
	ZERO_PTR(standardPt);

	standardPt->echo_events = mcr_standard_echo_event_defaults;
	standardPt->echo_event_count = MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT;
	standardPt->abs_resolution = MCR_ABS_RESOLUTION;
	standardPt->uinput_path = MCR_UINPUT_PATH;
	standardPt->event_path = MCR_EVENT_PATH;

	if ((mtxErr = mtx_init(&standardPt->device_lock, mtx_plain)) != thrd_success) {
		mtxErr = mcr_thrd_errno(mtxErr);
		free(standardPt);
		error_set_return(mtxErr);
	}

	context->standard.platform_pt = standardPt;
	return mcr_Device_initialize(context);
}

int mcr_standard_platform_deinitialize(struct mcr_context *context)
{
	struct mcr_standard_platform *standardPt = context->standard.platform_pt;
	int mtxErr;
	if (mcr_Device_deinitialize(context))
		return mcr_err;

	mtxErr = mtx_lock(&standardPt->device_lock);
	if (mtxErr == thrd_success)
		mtx_unlock(&standardPt->device_lock);
	mtx_destroy(&standardPt->device_lock);

	context->standard.platform_pt = NULL;
	free(standardPt);
	return 0;
}
