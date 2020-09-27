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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mcr_AbsDispatcher_ctor(struct mcr_AbsDispatcher
							*genericDispatcherPt, struct mcr_context *ctx,void (*deinit)(struct mcr_IDispatcher *),
							mcr_IDispatcher_add_fnc add,
							void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_modify_fnc modifier,
							mcr_IDispatcher_remove_fnc remove, void(*trim)(struct mcr_IDispatcher *))
{
	if (genericDispatcherPt) {
		mcr_IDispatcher_ctor(&genericDispatcherPt->idispatcher, ctx, deinit, add, clear,
							 mcr_AbsDispatcher_dispatch, modifier, remove, trim);
		genericDispatcherPt->receivers = NULL;
		genericDispatcherPt->signal_receivers = NULL;
		genericDispatcherPt->receiver_count = genericDispatcherPt->signal_receiver_count
											  = 0;
	}
}

void mcr_AbsDispatcher_set_receivers(struct mcr_AbsDispatcher
									 *genDispPt, struct mcr_DispatchReceiver *receiverList, size_t receiverCount)
{
	dassert(genDispPt);
	if (receiverCount) {
		dassert(receiverList);
	} else if (receiverList) {
		mset_error(EINVAL);
		mcr_dexit(EINVAL);
	}
	genDispPt->receivers = receiverList;
	genDispPt->receiver_count = receiverCount;
}

void mcr_AbsDispatcher_set_signal_receivers(
	struct mcr_AbsDispatcher *genDispPt,
	struct mcr_ReceiverMapElement *sortedReferenceReceiverList,
	size_t referenceReceiverCount)
{
	dassert(genDispPt);
	if (referenceReceiverCount) {
		dassert(sortedReferenceReceiverList);
	} else if (sortedReferenceReceiverList) {
		mset_error(EINVAL);
		mcr_dexit(EINVAL);
	}
	genDispPt->signal_receivers = sortedReferenceReceiverList;
	genDispPt->signal_receiver_count = referenceReceiverCount;
}

static MCR_INLINE bool dispatchList(struct mcr_DispatchReceiver *list,
									size_t count,
									struct mcr_Signal *sigPt, unsigned int mods)
{
	struct mcr_DispatchReceiver *endPt = list + count;
	dassert(list);
	dassert(count);
	while (list < endPt) {
		dassert(list->receive);
		if (list->receive(list, sigPt, mods))
			return true;
		++list;
	}
	return false;
}

bool mcr_AbsDispatcher_dispatch(struct mcr_IDispatcher *dispPt,
								struct mcr_Signal *sigPt, unsigned int mods)
{
	struct mcr_ReceiverMapElement *arrayPt;
	struct mcr_AbsDispatcher *genericPt = (struct mcr_AbsDispatcher *)
										  dispPt;
	dassert(genericPt);
	if (genericPt->signal_receiver_count) {
		dassert(genericPt->signal_receivers);
		arrayPt = bsearch(&sigPt, genericPt->signal_receivers,
						  genericPt->signal_receiver_count, sizeof(struct mcr_DispatchReceiver),
						  mcr_ref_compare);
		if (arrayPt && arrayPt->receiver_count) {
			if(dispatchList(arrayPt->receivers, arrayPt->receiver_count, sigPt, mods))
				return true;
		}
	}
	if (genericPt->receiver_count) {
		dassert(genericPt->receivers);
		if(dispatchList(genericPt->receivers, genericPt->receiver_count, sigPt, mods))
			return true;
	}
	return false;
}
