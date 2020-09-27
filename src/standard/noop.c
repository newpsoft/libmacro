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

int mcr_NoOp_send(struct mcr_Signal *signalPt)
{
	dassert(signalPt);
	void *ptr = signalPt->instance.data_member.data;
	if (!ptr)
		return 0;
	mcr_NoOp_send_member(ptr);
	return 0;
}

void mcr_NoOp_send_member(struct mcr_NoOp *noopPt)
{
	struct timespec val;
	if (!noopPt)
		return;
	// # seconds maybe in msec
	val.tv_sec = noopPt->seconds + noopPt->milliseconds / 1000;
	// # nanoseconds in msec, not including full seconds
	val.tv_nsec = (noopPt->milliseconds % 1000) * 1000000;
	thrd_sleep(&val, NULL);
}

struct mcr_ISignal *mcr_iNoOp(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.inoop;
}
