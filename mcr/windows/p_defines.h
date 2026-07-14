/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Platform definitions for Windows (MSVC/MinGW).
 *
 *  Includes Win32 headers with WIN32_LEAN_AND_MEAN and NOMINMAX.
 *  Defines MCR_EXPORT/MCR_IMPORT using __declspec for DLL symbol
 *  visibility.
 */

#pragma once

/* Windows Version */
/*# define _WIN32_WINNT _WIN32_WINNT_WINXP      // 0x0501 */
#include <sdkddkver.h>

/* Exclude rarely-used stuff from Windows headers,
 * and we use min and max for naming. */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

// Use default MCR_EXPORT and MCR_IMPORT, detecting MSVC or Mingw.

/* Unused below here */

// /* For some reason hwheel is not always defined. */
// #ifndef MOUSEEVENTF_HWHEEL
//  #define MOUSEEVENTF_HWHEEL 0x01000
// #endif

// #ifndef WM_MOUSEHWHEEL
//  #define WM_MOUSEHWHEEL 0x020E
// #endif

// /*! Ignore this input because it is virtual.  Use mcr_send to dispatch and
//  *  possibly block before the member is sent.
//  */
// #define MCR_WM_IGNORE 0x1482

// #ifndef MCR_INTERCEPT_WAIT_MILLIS
//  #define MCR_INTERCEPT_WAIT_MILLIS 5000
// #endif


