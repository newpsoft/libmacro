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

#include <stdlib.h>
#include <string.h>

extern const int mcr_Key_gen_key;
const int mcr_Key_gen_key = MCR_KEY_ANY;
extern const unsigned int mcr_Key_gen_up;
const unsigned int mcr_Key_gen_up = MCR_BOTH;

void mcr_AbsKeyDispatcher_ctor(struct mcr_AbsKeyDispatcher *keyDispatcherPt,
							struct mcr_context *ctx, void (*deinit)(struct mcr_IDispatcher *), mcr_IDispatcher_add_fnc add,
							void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_remove_fnc remove,
							void (*trim)(struct mcr_IDispatcher *))
{
	if (keyDispatcherPt) {
		ZERO_PTR(keyDispatcherPt);
		mcr_IDispatcher_ctor(&keyDispatcherPt->idispatcher, ctx, deinit, add, clear,
							 mcr_AbsKeyDispatcher_dispatch, mcr_AbsKeyDispatcher_modifier, remove, trim);
	}
}

static MCR_INLINE bool dispatch_to(struct mcr_ReceiverMapElement *receiversPt,
								   struct mcr_Signal *signalPt, unsigned int mods)
{
	dassert(receiversPt);
	struct mcr_DispatchReceiver *itPt, *endPt;
	if (receiversPt->receiver_count) {
		dassert(receiversPt->receivers);
		for (itPt = receiversPt->receivers,
			 endPt = receiversPt->receivers + receiversPt->receiver_count;
			 itPt < endPt; itPt++) {
			dassert(itPt->receive);
			if (itPt->receive(itPt, signalPt, mods))
				return true;
		}
	}
	return false;
}

static MCR_INLINE bool dispatch_safely(struct mcr_ReceiverMapElement *receiversPt,
									   struct mcr_Signal *signalPt, unsigned int mods)
{
	return receiversPt ? dispatch_to(receiversPt, signalPt, mods) : false;
}

static MCR_INLINE bool find_and_dispatch(struct mcr_ReceiverMapElement *receiverMap,
		size_t mapCount, int key,
		struct mcr_Signal *signalPt, unsigned int mods)
{
	if (!mapCount)
		return false;
	struct mcr_ReceiverMapElement *arrPt;
	/* If not already generic key, dispatch to specific key */
	if (key != MCR_KEY_ANY) {
		arrPt = bsearch(&key, receiverMap, mapCount,
						sizeof(struct mcr_ReceiverMapElement), mcr_int_compare);
		if (dispatch_safely(arrPt, signalPt, mods))
			return true;
	}
	/* Always dispatch to generic. */
	arrPt = bsearch(&mcr_Key_gen_key, receiverMap, mapCount,
					sizeof(struct mcr_ReceiverMapElement), mcr_int_compare);
	return dispatch_safely(arrPt, signalPt, mods);
}

bool mcr_AbsKeyDispatcher_dispatch(struct mcr_IDispatcher *idispPt,
								struct mcr_Signal *signalPt, unsigned int mods)
{
	struct mcr_AbsKeyDispatcher *keyDispatcher = (struct mcr_AbsKeyDispatcher *)idispPt;
	dassert(keyDispatcher);
	struct mcr_Key *keyPt = signalPt ? signalPt->instance.data_member.data : NULL;
	int key = MCR_KEY_ANY, applyType = MCR_BOTH;
	if (keyPt) {
		key = keyPt->key;
		applyType = keyPt->apply;
		/* Only dispatch to press or release listeners. */
		if (applyType < MCR_BOTH) {
			return find_and_dispatch(keyDispatcher->key_receivers[applyType],
									 keyDispatcher->key_receiver_count[applyType], key, signalPt, mods);
		}
	}
	/* Generic press type */
	if (find_and_dispatch(keyDispatcher->key_receivers[0],
						  keyDispatcher->key_receiver_count[0], key, signalPt, mods))
		return true;
	return find_and_dispatch(keyDispatcher->key_receivers[1],
							 keyDispatcher->key_receiver_count[1], key, signalPt, mods);
}

void mcr_AbsKeyDispatcher_modifier(struct mcr_IDispatcher *idispPt,
								struct mcr_Signal *signalPt, unsigned int *modsPt)
{
	struct mcr_context *ctx;
	struct mcr_AbsKeyDispatcher *keyDispatcher = (struct mcr_AbsKeyDispatcher *)idispPt;
	dassert(keyDispatcher);
	dassert(modsPt);
	//	struct mcr_context *ctx = ((struct mcr_IDispatcher *)dispDataPt)->ctx;
	struct mcr_Key *keyPt = signalPt ? signalPt->instance.data_member.data : NULL;
	struct mcr_MapElement *found;
	unsigned int modifier;
	if (keyPt) {
		dassert(signalPt->isignal);
		dassert(signalPt->interface->context);
		ctx = signalPt->interface->context;
		found = bsearch(&keyPt->key, ctx->standard.key_modifiers,
						ctx->standard.key_modifier_count, sizeof(*found),
						mcr_int_compare);
		if (found) {
			modifier = found->second.u_integer;
			switch (keyPt->apply) {
			case MCR_SET:
				*modsPt |= modifier;
				break;
			case MCR_UNSET:
			case MCR_BOTH:
				*modsPt &= ~modifier;
				break;
			case MCR_TOGGLE:
				if ((*modsPt & modifier) == modifier) {
					*modsPt &= ~modifier;
				} else {
					*modsPt |= modifier;
				}
				break;
			}
		}
	}
}

void mcr_AbsKeyDispatcher_set_receivers(struct mcr_AbsKeyDispatcher *keyDispatcher,
									 enum mcr_ApplyType forApplyType, struct mcr_ReceiverMapElement *receivers,
									 size_t receiverCount)
{
	dassert(keyDispatcher);
	dassert(forApplyType <= MCR_UNSET);
	if (receiverCount)
		dassert(receivers);
	keyDispatcher->key_receivers[forApplyType] = receivers;
	keyDispatcher->key_receiver_count[forApplyType] = receiverCount;
}

//static int mcr_AbsKeyDispatcher_add_keys(struct mcr_Map *keyMap, int key,
//									   void *newTrigger, mcr_dispatch_receive_fnc receiveFnc)
//{
//	/* first map int => map, second unsigned => dispatch array */
//	struct mcr_Array *arrPt;
//	struct mcr_DispatchReceiver dispPair = { newTrigger, receiveFnc };
//	arrPt = mcr_Map_element_ensured(keyMap, &key);
//	if (!arrPt)
//		return mcr_err;	// Expect ENOMEM
//	arrPt = MCR_MAP_VALUEOF(*keyMap, arrPt);
//	/* Multiple object references may exist */
//	return mcr_Array_add(arrPt, &dispPair, 1, false);
//}

///* Key */
//int mcr_AbsKeyDispatcher_add(struct mcr_IDispatcher *idispPt, struct mcr_Signal *signalPt,
//						   void *newTrigger, mcr_dispatch_receive_fnc receiveFnc)
//{
//	struct mcr_IDispatcher *dispPt = dispDataPt;
//	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
//	int applyType;
//	struct mcr_Map *dispMaps = dispPt->ctx->standard.key_dispatcher_maps;
//	if (!receiveFnc)
//		receiveFnc = mcr_Trigger_receive;
//	/* mcr_Trigger_receive requires an object */
//	if (receiveFnc == mcr_Trigger_receive && !newTrigger)
//		return EINVAL;
//	if (keyPt) {
//		applyType = keyPt->apply;
//		if (applyType < MCR_BOTH) {
//			return mcr_AbsKeyDispatcher_add_keys(dispMaps + applyType,
//											   keyPt->key, newTrigger, receiveFnc);
//		}
//		/* Generic up, add to both */
//		if (mcr_AbsKeyDispatcher_add_keys(dispMaps,
//										keyPt->key, newTrigger, receiveFnc)) {
//			return mcr_err;
//		}
//		return mcr_AbsKeyDispatcher_add_keys(dispMaps + 1,
//										   keyPt->key, newTrigger, receiveFnc);
//	}
//	/* Generic up, add to both */
//	if (mcr_AbsKeyDispatcher_add_keys(dispMaps, MCR_KEY_ANY,
//									newTrigger, receiveFnc)) {
//		return mcr_err;
//	}
//	return mcr_AbsKeyDispatcher_add_keys(dispMaps + 1, MCR_KEY_ANY,
//									   newTrigger, receiveFnc);
//}

//void mcr_AbsKeyDispatcher_clear(struct mcr_IDispatcher *idispPt)
//{
//	struct mcr_IDispatcher *dispPt = dispDataPt;
//	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
//	/* Down, up, and generic */
//	mcr_Map_clear(maps++);
//	mcr_Map_clear(maps);
//	return 0;
//}

//void mcr_AbsKeyDispatcher_modifier(struct mcr_IDispatcher *idispPt,
//								 struct mcr_Signal *sigPt, unsigned int *modsPt)
//{
//	dassert(dispDataPt);
//	dassert(modsPt);
//	struct mcr_context *ctx = ((struct mcr_IDispatcher *)dispDataPt)->ctx;
//	struct mcr_Key *keyPt = mcr_Key_data(sigPt);
//	int key;
//	unsigned int *found;
//	if (keyPt) {
//		key = keyPt->key;
//		found = mcr_Map_value(&ctx->standard.map_key_modifier, &key);
//		if (found) {
//			switch (keyPt->apply) {
//			case MCR_SET:
//				*modsPt |= *found;
//				break;
//			case MCR_UNSET:
//			case MCR_BOTH:
//				*modsPt &= ~(*found);
//				break;
//			case MCR_TOGGLE:
//				if ((*modsPt & *found) == *found) {
//					*modsPt &= ~(*found);
//				} else {
//					*modsPt |= *found;
//				}
//				break;
//			}
//		}
//	}
//}

//void mcr_AbsKeyDispatcher_remove(struct mcr_IDispatcher *idispPt, void *delTrigger)
//{
//#define localRemove(itPt) \
//	mcr_Array_remove((struct mcr_Array *)itPt, &delTrigger);
//	struct mcr_IDispatcher *dispPt = dispDataPt;
//	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
//	MCR_MAP_FOR_EACH_VALUE(maps[0], localRemove);
//	MCR_MAP_FOR_EACH_VALUE(maps[1], localRemove);
//	return 0;
//#undef localRemove
//}

//static void mcr_AbsKeyDispatcher_remove_empties(struct mcr_Map *mapPt)
//{
//	char *firstPt, *itPt;
//	size_t bytes, valSize = mapPt->value_size;
//	struct mcr_Array *arrPt;
//	struct mcr_Map *mapElement;
//	mcr_Map_iter_range(mapPt, &firstPt, &itPt, &bytes, 0, mapPt->set.used);
//	if (firstPt && itPt) {
//		while (itPt >= firstPt) {
//			if (valSize == sizeof(struct mcr_Map)) {
//				/* Values are maps, remove their mapped empties */
//				mapElement = MCR_MAP_VALUEOF(*mapPt, itPt);
//				mcr_AbsKeyDispatcher_remove_empties(mapElement);
//				if (!mapElement->set.used)
//					mcr_Map_unmap(mapPt, (void *)itPt);
//			} else {
//				arrPt = MCR_MAP_VALUEOF(*mapPt, itPt);
//				if (!arrPt->used)
//					mcr_Map_unmap(mapPt, (void *)itPt);
//			}
//			itPt -= bytes;
//		}
//	}
//}

//void mcr_AbsKeyDispatcher_trim(struct mcr_IDispatcher *idispPt)
//{
//	struct mcr_IDispatcher *dispPt = dispDataPt;
//	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
//	mcr_AbsKeyDispatcher_remove_empties(maps);
//	mcr_AbsKeyDispatcher_remove_empties(maps + 1);
//	return 0;
//}
