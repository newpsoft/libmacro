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

#define _CRT_SECURE_NO_WARNINGS

#include "mcr/libmacro.h"

#include <string.h>

/* thread_local not available for MSVC.  Check new versions? */
#ifdef MCR_PLATFORM_WINDOWS
	/* Dll workaround, mcr_err is not dll exported. */
	#undef mcr_err
	extern int mcr_err;
#else
	/* Shared and exported thread_local.  Fast and thread-safe access. */
	MCR_API
	#ifdef __GNUC__
		__thread
	#else
		thread_local
	#endif
#endif
int mcr_err;

int *mcr_err_tls()
{
	return &mcr_err;
}

int mcr_read_err()
{
	int ret = mcr_err;
	if (ret)
		mcr_err = 0;
	return ret;
}

const char *mcr_error_str(int errorNumber)
{
	return strerror(errorNumber);
}
