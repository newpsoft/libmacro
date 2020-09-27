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
 *  \brief Read and dispatch signals. This may block incoming events.
 */

#ifndef MCR_INTERCEPT_WIN_NINTERCEPT_H_
#define MCR_INTERCEPT_WIN_NINTERCEPT_H_

#include "mcr/standard/standard.h"
#include "mcr/standard/windows/p_standard.h"

#ifdef __cplusplus
extern "C" {
#endif

// Last index is count - 1
#define MCR_WM_HIDECHO_COUNT (WM_MOUSELAST - WM_MOUSEFIRST + 1)
#define MCR_WM_HIDECHO_INDEX(wParam) (wParam - WM_MOUSEFIRST)

struct mcr_intercept_platform {
	/*! Map WPARAM hook windows message to size_t echo code.
	 *  There is a min and max windows mouse message, so this is a
	 *  statically typed array.  Get the index by WPARAM - WM_MOUSEFIRST
	 *  or \ref
	 */
	size_t wm_echos[MCR_WM_HIDECHO_COUNT];
};

/*! \ref mcr_platform
 */
extern MCR_API const size_t *const mcr_intercept_wm_echo_defaults;

MCR_API bool mcr_intercept_key_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_key_set_enabled(struct mcr_context *ctx, bool enable);
MCR_API bool mcr_intercept_mouse_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_mouse_set_enabled(struct mcr_context *ctx, bool enable);
MCR_API int mcr_intercept_set_wm_echo(struct mcr_context *ctx, WPARAM wm, size_t echoCode);

#ifdef __cplusplus
}
#endif

#endif
