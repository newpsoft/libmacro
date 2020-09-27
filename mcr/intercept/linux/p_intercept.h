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
 *  \brief Read from grabbers and dispatch signals. This may block
 *  incoming events.
 */

#ifndef MCR_INTERCEPT_LINUX_P_INTERCEPT_H_
#define MCR_INTERCEPT_LINUX_P_INTERCEPT_H_

#include "mcr/intercept/linux/p_grabber.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Linux intercept platform structure */
struct mcr_intercept_platform {
	/*! Intercept critical sections */
	mtx_t lock;
	struct {
		/*! Ordered list so each grab can remove itself */
		struct mcr_Grabber *grabbers;
		size_t grabber_count;
	};
	struct {
		/*! Set of input event paths to try grabbing */
		const char * const*grab_paths;
		size_t grab_path_count;
	};
	struct {
		/*! int key => echo event code (index of echo_events) */
		const struct mcr_MapElement *key_echos[2];
		size_t key_echo_count[2];
	};
	/*! Get key pressed values from a device
	 *
	 *  KEY_CNT / 8 is a floor value, and may have remainder of some keys. */
	char bit_retrieval[MCR_EVENTINDEX(KEY_CNT) + 1];
};

/* Use MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT */
#define MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT (MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT / 2)
extern MCR_API const struct mcr_MapElement *const mcr_intercept_key_echo_defaults[2];

/* All use /dev/input/eventX files to read from */
MCR_API int mcr_intercept_set_grabs(struct mcr_context *ctx, const char *const*allGrabPaths, size_t pathCount);
MCR_API void mcr_intercept_set_key_echo_map(struct mcr_context *ctx, const struct mcr_MapElement *keyEchos, size_t count, enum mcr_ApplyType applyType);

#ifdef __cplusplus
}
#endif

#endif
