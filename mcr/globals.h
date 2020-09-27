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

#ifndef MCR_GLOBALS_H_
#define MCR_GLOBALS_H_

#include "mcr/defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MCR_PLATFORM_WINDOWS
/*! Thread-local storage not available in MSVC? */
extern MCR_API int *mcr_err_tls();
#define mcr_err (*mcr_err_tls())
#else
extern MCR_API
#ifdef __GNUC__
__thread
#else
thread_local
#endif
int mcr_err;
#endif

/*! Get current \ref mcr_err and reset mcr_err to 0. */
MCR_API int mcr_read_err();
/*! strerror of errorNumber */
MCR_API const char *mcr_error_str(int errorNumber);

#ifdef __cplusplus
}
#endif

#endif // GLOBALS_H
