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

#include "mcr/libmacro.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void mcr_Signal_init(struct mcr_Signal *signalPt)
{
	if (signalPt)
		ZERO_PTR(signalPt);
}

void mcr_Signal_deinit(struct mcr_Signal *signalPt)
{
	mcr_Instance_deinit(&signalPt->instance);
}

int mcr_Signal_copy(struct mcr_Signal *dstPt, const struct mcr_Signal *srcPt)
{
	dassert(dstPt);
	if (mcr_Instance_copy(&dstPt->instance, (const struct mcr_Instance *)srcPt))
		return mcr_err;
	if (dstPt)
		dstPt->dispatch_flag = srcPt ? srcPt->dispatch_flag : false;
	return 0;
}

int mcr_Signal_compare(const struct mcr_Signal *lhsSignalPt,
					   const struct mcr_Signal *rhsSignalPt)
{
	if (rhsSignalPt) {
		if (lhsSignalPt) {
			if (lhsSignalPt == rhsSignalPt)
				return 0;
			int valHolder =
				mcr_Instance_compare(&lhsSignalPt->instance, &rhsSignalPt->instance);
			return valHolder ? valHolder :
				   MCR_CMP(lhsSignalPt->dispatch_flag,
						   rhsSignalPt->dispatch_flag);
		}
		return -1;
	}
	return !!lhsSignalPt;
}
