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

#ifndef MCR_STANDARD_LINUX_P_STANDARD_H_
#define MCR_STANDARD_LINUX_P_STANDARD_H_

#include "mcr/standard/linux/p_device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_standard_platform {
	mcr_SpacePosition cursor;
	struct {
		/*! echo code index => input_event to send (EV_KEY) */
		const struct input_event *echo_events;
		size_t echo_event_count;
	};
	struct mcr_Device gen_dev;
	struct mcr_Device abs_dev;
	__s32 abs_resolution;
	const char *uinput_path;
	const char *event_path;
	mtx_t device_lock;
};

/* Common array length for standard and intercept echo events */
#define MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT 90
extern MCR_API const struct input_event *const mcr_standard_echo_event_defaults;

extern MCR_API void mcr_standard_set_echo_events(struct mcr_context *ctx, const struct input_event *echoEvents, size_t count);

#ifdef __cplusplus
}
#endif

#endif
