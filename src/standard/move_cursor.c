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

int mcr_MoveCursor_send(struct mcr_Signal *signalPt)
{
	dassert(signalPt);
	void *ptr = signalPt->instance.data_member.data;
	if (!ptr)
		return 0;
	return mcr_MoveCursor_send_member(ptr, signalPt->interface->context);
}

bool mcr_resembles(const struct mcr_MoveCursor *lhs,
				   const struct mcr_MoveCursor *rhs, const unsigned int measurementError)
{
	if (!lhs || !rhs)
		return lhs == rhs;
	if (lhs->justify_flag != rhs->justify_flag)
		return false;
	if (lhs->justify_flag)
		return mcr_resembles_justified(lhs->position, rhs->position);
	return mcr_resembles_absolute(lhs->position, rhs->position, measurementError);
}

struct mcr_ISignal *mcr_iMoveCursor(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.imove_cursor;
}
