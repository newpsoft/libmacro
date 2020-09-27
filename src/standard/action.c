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

#include <stdio.h>

void mcr_Action_init(struct mcr_Action *actPt)
{
	if (actPt) {
		actPt->modifiers = MCR_MF_NONE;
		actPt->trigger_flags = MCR_TF_ALL;
	}
}

int mcr_Action_init_(void *ptr)
{
	mcr_Action_init(ptr);
	return 0;
}

bool mcr_Action_receive(struct mcr_Trigger *triggerPt,
						struct mcr_Signal * sigPt,
						unsigned int mods)
{
	dassert(triggerPt);
	struct mcr_Action *actPt = triggerPt->instance.data_member.data;
	if (actPt && triggerPt->trigger) {
		if (mcr_TriggerFlags_match(actPt->trigger_flags, actPt->modifiers, mods))
			return triggerPt->trigger(triggerPt, sigPt, mods);
	}
	return false;
}

struct mcr_ITrigger *mcr_iAction(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.iaction;
}
