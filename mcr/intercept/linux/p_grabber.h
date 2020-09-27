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
 *  \brief Grabber - Take exclusive access to a /dev/input event.
 */

#ifndef MCR_INTERCEPT_LINUX_P_GRABBER_H_
#define MCR_INTERCEPT_LINUX_P_GRABBER_H_

#include "mcr/standard/standard.h"
#include "mcr/standard/linux/p_standard.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Take exclusive access to a /dev/input event. */
struct mcr_Grabber {
	struct mcr_context *context;
	/*! Allow blocked grabbing, \ref EVIOCGRAB
	 *
	 *  Default false. */
	bool blocking;
	/*! File of input event to read */
	int fd;
	/*! File path of input event */
	const char *path;
	// \todo bool value for in-thread and reading?
};

/*! \ref ctor */
MCR_API void mcr_Grabber_init(struct mcr_Grabber *grabPt, struct mcr_context *ctx);
MCR_API void mcr_Grabber_ctor(struct mcr_Grabber *grabPt, struct mcr_context *ctx, bool blocking, const char *path);
MCR_API int mcr_Grabber_deinit(struct mcr_Grabber *grabPt);

/*! \ref mcr_Grabber.blocking */
MCR_API bool mcr_Grabber_blocking(struct mcr_Grabber *grabPt);
/*! \ref mcr_Grabber.blocking, and will also set EVIOCGRAB if currently
 *  enabled. */
MCR_API int mcr_Grabber_set_blocking(struct mcr_Grabber *grabPt, bool enable);
/*! \ref mcr_Grabber.path */
MCR_API const char *mcr_Grabber_path(struct mcr_Grabber *grabPt);
/*! \ref mcr_Grabber.path */
MCR_API int mcr_Grabber_set_path(struct mcr_Grabber *grabPt, const char *path);
/*! Get enabled state, and set the grabber enabled
 *  state to the same.
 */
MCR_API bool mcr_Grabber_enabled(struct mcr_Grabber *grabPt);
/*! Allow or disallow this object to function.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable);

#define MCR_EVENTINDEX(keyCode) \
((keyCode) / 8)

#define MCR_EVENTBIT(keyCode) \
(1 << ((keyCode) % 8))

#ifdef __cplusplus
}
#endif

#endif
