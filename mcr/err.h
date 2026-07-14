/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Raise a compiler error.
 *  Usage:
 *  `#include "mcr/err.h"`
 */

#ifdef __GNUC__
#error Error!
#elif defined(__clang__)
#pragma GCC error "Error!"
#else
/* MSVC-specific error, but will at least print a message for
	* unknown compilers.
	*/
#pragma message "Error Msg: Error!"
#endif
