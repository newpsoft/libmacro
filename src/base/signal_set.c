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

#include "mcr/base/signal_set.h"
#include "mcr/base/base.h"

#include <stdio.h>

bool mcr_SignalSet_receive(struct mcr_DispatchReceiver *dispatchReceiver,
						   struct mcr_Signal *signalPt, unsigned int mods)
{
	dassert(dispatchReceiver);
	struct mcr_SignalSet *signalSetPt = dispatchReceiver->receiver;
	dassert(signalSetPt);
	struct mcr_context *ctx = signalSetPt->ctx;
	dassert(ctx);
	size_t i = 0;
	UNUSED(signalPt);
	UNUSED(mods);
	for (; i < signalSetPt->signal_count; i++) {
		if (mcr_send(ctx, signalSetPt->signals + i)) {
			dmsg;
			return signalSetPt->block_flag;
		}
	}
	return signalSetPt->block_flag;
}
