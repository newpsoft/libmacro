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

int mcr_Key_send(struct mcr_Signal *signalPt)
{
	dassert(signalPt);
	void *ptr = signalPt->instance.data_member.data;
	if (!ptr)
		return 0;
	return mcr_Key_send_member(ptr, signalPt->interface->context);
}

struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.ikey;
}

int mcr_Key_compare(const void *lhsPt, const void *rhsPt)
{
	int cmp;
	if (rhsPt) {
		if (lhsPt) {
			if ((cmp = ((struct mcr_Key *)lhsPt)->key - ((struct mcr_Key *)rhsPt)->key))
				return cmp;
			return ((struct mcr_Key *)lhsPt)->apply - ((struct mcr_Key *)rhsPt)->apply;
		}
		return -1;
	}
	return !!lhsPt;
}

///* Modifier mapping */
//unsigned int mcr_Key_modifier(struct mcr_context *ctx, int key)
//{
//	unsigned int *found =
//		mcr_Map_value(&ctx->standard.map_key_modifier, &key);
//	return found ? *found : MCR_MF_NONE;
//}

//int mcr_Key_modifier_key(struct mcr_context *ctx, unsigned int modifier)
//{
//	int *found = mcr_Map_value(&ctx->standard.map_modifier_key, &modifier);
//	return found ? *found : MCR_KEY_ANY;
//}

//size_t mcr_Key_modifier_count(struct mcr_context * ctx)
//{
//	return ctx->standard.map_key_modifier.set.used;
//}

//size_t mcr_Key_modifier_key_count(struct mcr_context * ctx)
//{
//	return ctx->standard.map_modifier_key.set.used;
//}

//void mcr_Key_modifier_all(struct mcr_context *ctx, unsigned int *modBuffer,
//						  size_t bufferLength)
//{
//	size_t i = ctx->standard.map_modifier_key.set.used;
//	unsigned int *modPt;
//	if (!modBuffer)
//		return;
//	if (bufferLength < i)
//		i = bufferLength;
//	while (i--) {
//		modPt = mcr_Array_element(ctx->standard.map_modifier_key.set, i);
//		modBuffer[i] = *modPt;
//	}
//}

//void mcr_Key_modifier_key_all(struct mcr_context *ctx, int *keyBuffer,
//							  size_t bufferLength)
//{
//	size_t i = ctx->standard.map_key_modifier.set.used;
//	int *keyPt;
//	if (!keyBuffer)
//		return;
//	if (bufferLength < i)
//		i = bufferLength;
//	while (i--) {
//		keyPt = mcr_Array_element(ctx->standard.map_key_modifier.set, i);
//		keyBuffer[i] = *keyPt;
//	}
//}

//void mcr_Key_modifier_clear(struct mcr_context *ctx)
//{
//	mcr_Map_clear(&ctx->standard.map_key_modifier);
//	mcr_Map_clear(&ctx->standard.map_modifier_key);
//}

//int mcr_Key_modifier_set_key(struct mcr_context *ctx, unsigned int modifiers,
//							 int key)
//{
//	if (mcr_Map_map(&ctx->standard.map_key_modifier, &key, &modifiers) ||
//		mcr_Map_map(&ctx->standard.map_modifier_key, &modifiers,
//					&key)) {
//		return mcr_err;
//	}
//	return 0;
//}

//int mcr_Key_modifier_add(struct mcr_context *ctx, unsigned int modifiers,
//						 int key)
//{
//	return mcr_Map_map(&ctx->standard.map_key_modifier, &key, &modifiers);
//}

//int mcr_Key_modifier_map(struct mcr_context *ctx, unsigned int modifiers,
//						 int key, int *addKeys, size_t bufferLen)
//{
//	size_t i;
//	if (bufferLen)
//		dassert(addKeys);
//	if (mcr_Key_modifier_set_key(ctx, modifiers, key))
//		return mcr_err;
//	for (i = 0; i < bufferLen; i++) {
//		if (mcr_Key_modifier_add(ctx, modifiers, addKeys[i]))
//			return mcr_err;
//	}
//	return 0;
//}

//int mcr_Key_modifier_rekey(struct mcr_context *ctx, int oldKey, int newKey)
//{
//	unsigned int *modPt =
//		mcr_Map_value(&ctx->standard.map_key_modifier, &oldKey);
//	if (modPt) {
//		if (mcr_Map_map(&ctx->standard.map_modifier_key, modPt,
//						&newKey)) {
//			return mcr_err;
//		}
//		return mcr_Map_remap(&ctx->standard.map_key_modifier, &oldKey,
//							 &newKey);
//	}
//	return 0;
//}

//int mcr_Key_modifier_remod(struct mcr_context *ctx, unsigned int modifiers,
//						   unsigned int newMods)
//{
//	int *found = mcr_Map_value(&ctx->standard.map_modifier_key, &modifiers);
//	if (found) {
//		if (mcr_Map_map(&ctx->standard.map_key_modifier, found,
//						&newMods)) {
//			return mcr_err;
//		}
//		return mcr_Map_remap(&ctx->standard.map_modifier_key,
//							 &modifiers, &newMods);
//	}
//	return 0;
//}

//void mcr_Key_modifier_trim(struct mcr_context *ctx)
//{
//	mcr_Map_trim(&ctx->standard.map_key_modifier);
//	mcr_Map_trim(&ctx->standard.map_modifier_key);
//}
