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
 *  \brief Utilities for Libmacro
 */

#ifndef MCR_UTIL_UTIL_H_
#define MCR_UTIL_UTIL_H_

#include "mcr/util/c11threads.h"
#include "mcr/util/comparison.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! time(NULL) and ctime */
MCR_API const char *mcr_timestamp();
/*! For given bit return the corresponding array index.
 *  Requires at least one "on" bit
 *
 *  See \ref mcr_index_bit
 *  \return \ref retind Array index of bit value
 */
MCR_API size_t mcr_bit_index(uintmax_t bitval);
/*! For given array index return the corresponding bit value.
 *
 *  See \ref mcr_bit_index
 *  \return Bit value of array index
 */
#define mcr_index_bit(index) (1 << index)
/*! Get an \c errno.h code from a threading error number. */
MCR_API int mcr_thrd_errno(int thrdError);
/*! \ref thrd_create and thrd_detach
 *
 *	\return \ref reterr
 */
MCR_API int mcr_thrd(thrd_start_t func, void *arg);

/*! Disable privileges, and remove the ability to
 *  enable them.
 *
 *  \ref mcr_platform
 *  \return \ref reterr
 */
MCR_API int mcr_privileges_deactivate();

#ifdef __cplusplus
}
#endif

#endif
