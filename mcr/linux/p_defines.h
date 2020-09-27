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

#ifndef MCR_LINUX_P_DEFINES_H_
#define MCR_LINUX_P_DEFINES_H_

#include <strings.h>		/* strcasecmp */
#include <unistd.h>			/* read/write (used in macros) */ // \todo remove inline
#include <linux/input.h>	/* input_event (inline read/write) */ // \todo remove inline
#include <linux/uinput.h>	/* uinput_user_dev (inline write) */ // \todo remove inline

#define mcr_snprintf snprintf
#define mcr_casecmp strcasecmp
#define mcr_ncasecmp strncasecmp

#ifndef MCR_UINPUT_PATH_
	/*! Default value for uinput node when library is built. */
	#define MCR_UINPUT_PATH "/dev/uinput"
#endif
#ifndef MCR_EVENT_PATH_
	/*! Default value for device input event path when library is built. */
	#define MCR_EVENT_PATH "/dev/input"
#endif
#ifndef MCR_ABS_RESOLUTION
	/*! Default value for \ref mcr_AbsDev resolution when library is built.
	*  Default is 4095, or 0x0FFF
	*/
	#define MCR_ABS_RESOLUTION 0x0FFF
#endif
#ifndef MCR_FILE_TRY_COUNT
#define MCR_FILE_TRY_COUNT 5
#endif

#ifndef MCR_POLL_TIMEOUT
	#define MCR_POLL_TIMEOUT 3000
#endif

#ifndef MCR_GRAB_SET_LENGTH
#define MCR_GRAB_SET_LENGTH 16
#endif

#define MCR_GRAB_SET_SIZE (sizeof(struct input_event) * \
	MCR_GRAB_SET_LENGTH)

#endif
