/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Raise a compiler warning.
 *  Usage:
 *  `#include "mcr/warn.h"`
 */

#ifdef __GNUC__
#warning Warning!
#elif defined(__clang__)
#pragma GCC warning "Warning!"
#else
/* MSVC-specific warning, but will at least print a message for
	* unknown compilers.
	*/
#pragma message "Warning Msg: Warning!"
#endif
