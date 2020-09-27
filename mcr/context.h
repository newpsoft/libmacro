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
 *  \brief \ref mcr_context Library context forward declaration.
 */

#ifndef MCR_CONTEXT_H_
#define MCR_CONTEXT_H_

#include "mcr/util/util.h"
#include "mcr/base/base.h"
#include "mcr/standard/standard.h"
#include "mcr/intercept/intercept.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Libmacro library context, required for Libmacro functions
 *
 *  In cases of extreme complexity please break glass.
 */
struct mcr_context {
	/*! All modifiers known by Libmacro to be set */
	unsigned int modifiers;
	struct mcr_base base;
	struct mcr_standard standard;
	struct mcr_intercept intercept;
};

#ifdef __cplusplus
}
#endif

#endif
