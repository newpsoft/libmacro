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

#include "mcr/base/base.h"

#include <string.h>

bool mcr_Trigger_receive(struct mcr_DispatchReceiver *dispReceiver,
						 struct mcr_Signal *dispatchSignal, unsigned int mods)
{
	dassert(dispReceiver);
	struct mcr_Trigger *triggerPt = dispReceiver->receiver;
	dassert(triggerPt);
	if (triggerPt->itrigger) {
		dassert(triggerPt->itrigger->receive);
		return triggerPt->itrigger->receive(triggerPt, dispatchSignal,
											mods);
	}
	return false;
}

void mcr_Trigger_init(struct mcr_Trigger *triggerPt)
{
	if (triggerPt)
		ZERO_PTR(triggerPt);
}

void mcr_Trigger_deinit(struct mcr_Trigger *trigPt)
{
	mcr_Instance_deinit(&trigPt->instance);
}

int mcr_Trigger_copy(struct mcr_Trigger *dstPt, const struct mcr_Trigger *srcPt)
{
	dassert(dstPt);
	if (mcr_Instance_copy(&dstPt->instance, &srcPt->instance))
		return mcr_err;
	if (dstPt) {
		if (srcPt) {
			dstPt->trigger = srcPt->trigger;
			dstPt->actor = srcPt->actor;
		} else {
			dstPt->trigger = NULL;
			dstPt->actor = NULL;
		}
	}
	return 0;
}

int mcr_Trigger_compare(const struct mcr_Trigger *lhsTriggerPt,
						const struct mcr_Trigger *rhsTriggerPt)
{
	int cmp;
	if (lhsTriggerPt && rhsTriggerPt) {
		if ((cmp = mcr_Instance_compare(&lhsTriggerPt->instance,
										&rhsTriggerPt->instance)))
			return cmp;
		if ((cmp = MCR_CMP_CAST((void *), lhsTriggerPt->trigger,
								rhsTriggerPt->trigger))) {
			return cmp;
		}
		return MCR_CMP(lhsTriggerPt->actor, rhsTriggerPt->actor);
	}
	return MCR_CMP(lhsTriggerPt, rhsTriggerPt);
}

int mcr_Trigger_add_dispatch(struct mcr_context *ctx,
							 struct mcr_Trigger *trigPt, struct mcr_Signal *interceptPt)
{
	return mcr_dispatch_add(ctx, interceptPt, trigPt,
							mcr_Trigger_receive);
}

void mcr_Trigger_remove_dispatch(struct mcr_context *ctx,
								 struct mcr_Trigger *trigPt, struct mcr_ISignal *isigPt)
{
	mcr_dispatch_remove(ctx, isigPt, trigPt);
}
