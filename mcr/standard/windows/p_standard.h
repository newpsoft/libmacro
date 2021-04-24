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

#ifndef MCR_STANDARD_WINDOWS_P_STANDARD_H_
#define MCR_STANDARD_WINDOWS_P_STANDARD_H_

#include "mcr/base/base.h"
#include "mcr/util/windows/p_util.h"

/*! Ignore this input because it is virtual.  Use mcr_send to dispatch and
 *  possibly block before the member is sent.
 */
#define MCR_WM_IGNORE 0x1482

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_standard_platform {
	/*! size_t echo index => mouse event flags */
	const DWORD *echo_flags;
	/*! Number of echo keys mapped to flags */
	size_t echo_flag_count;
};

/*! \ref mcr_platform
 */
extern MCR_API const DWORD mcr_standard_echo_flag_defaults[];
/*! \ref mcr_platform
 */
extern MCR_API size_t mcr_standard_echo_flag_default_count;

/*! Map echo as index to windows mouse event flags.
 *  \ref mcr_platform
 */
MCR_API void mcr_standard_set_echo_flags(struct mcr_context *ctx, const DWORD *mouseEventFlags, size_t count);

#ifdef __cplusplus
}
#endif

#endif
