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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

const char *mcr_timestamp()
{
	time_t curtime;
	time(&curtime);
	return ctime(&curtime);
}

size_t mcr_bit_index(uintmax_t bitval)
{
	/* No on bit gives no index information.
	 * If not 0, there will definitely be some bit value contained */
	size_t index = 0;
	if (bitval == 0)
		return (size_t)~0;
	while ((bitval & 0x01) == 0) {
		++index;
		bitval >>= 1;
	}
	return index;
}

int mcr_thrd_errno(int thrdError)
{
	switch (thrdError) {
	case thrd_success:
		return 0;
	case thrd_nomem:
		return ENOMEM;
	case thrd_busy:
		return EBUSY;
	case thrd_timedout:
		return ETIMEDOUT;
	case thrd_error:
		return EINTR;
	}
	return -1;
}

/*! \todo If thread created but not detached, calling function may free
 *  resources currently in use by new thread. */
int mcr_thrd(thrd_start_t func, void *arg)
{
	int thrdErr;
	thrd_t trd;
	mcr_err = 0;
	if ((thrdErr = thrd_create(&trd, func, arg)) == thrd_success &&
		(thrdErr = thrd_detach(trd)) == thrd_success) {
		return 0;
	}
	mcr_err = mcr_thrd_errno(thrdErr);
	mcr_dexit;
	return mcr_err;
}
