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
 *  \brief The functions in this file are not intended for public use and
 *  should only be used by Libmacro itself.  All of these functions are
 *  used to initialize or deinitialize the context in \ref libmacro.h
 *
 *  In cases of extreme complexity, please break glass.
 */

#ifndef MCR_PRIVATE_H_
#define MCR_PRIVATE_H_

#include "mcr/globals.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_context;

extern MCR_API int mcr_base_initialize(struct mcr_context *ctx);
extern MCR_API int mcr_base_deinitialize(struct mcr_context *ctx);
extern MCR_API void mcr_base_trim(struct mcr_context *ctx);

extern MCR_API int mcr_standard_initialize(struct mcr_context *ctx);
extern MCR_API int mcr_standard_deinitialize(struct mcr_context *ctx);

extern MCR_API int mcr_intercept_initialize(struct mcr_context *ctx);
extern MCR_API int mcr_intercept_deinitialize(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
