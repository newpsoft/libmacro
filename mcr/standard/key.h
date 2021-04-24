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

/*! \file
 *  \brief \ref mcr_Key - Simulate keyboard keys.
 */

#ifndef MCR_STANDARD_KEY_H_
#define MCR_STANDARD_KEY_H_

#include "mcr/base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Simulate keyboard keys
 *
 *  A 0 value of key code should be logically
 *  interpreted as either, "no key available, do not send,"
 *  or "key code of any value."
 */
struct mcr_Key {
	/*! Key code, specified by platform */
	int key;
	/*! Key press or release */
	enum mcr_ApplyType apply;
};

/*! \pre Signal instance data is \ref mcr_Key
 *  \brief Simulate keyboard keys.
 *
 *  \param sigPt Has data member \ref mcr_Key
 *  \return \ref reterr
 */
MCR_API int mcr_Key_send(struct mcr_Signal *signalPt);
/*! \ref mcr_Key_send
 *
 *  \ref mcr_platform
 *  \return \ref reterr
 */
MCR_API int mcr_Key_send_member(struct mcr_Key *dataPt, struct mcr_context *ctx);
/* Default allocate, deallocate, init, deinit, compare, and copy */
/*! Compare mcr_Key elements, to find by key. */
MCR_API int mcr_Key_compare(const void *lhsPt, const void *rhsPt);

/*! Get the Signal interface of \ref mcr_Key */
MCR_API struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx);

///* Modifier mapping */
///*! Get the modifier from a key code. */
//MCR_API unsigned int mcr_Key_modifier(struct mcr_context *ctx, int key);
///*! Get the key code from a modifier. */
//MCR_API int mcr_Key_modifier_key(struct mcr_context *ctx,
//								 unsigned int modifier);
///*! Get the number of keys mapped to modifiers. */
//MCR_API size_t mcr_Key_modifier_count(struct mcr_context *ctx);
///*! Get the number of modifiers mapped to key codes. */
//MCR_API size_t mcr_Key_modifier_key_count(struct mcr_context *ctx);
///*! Get all modifiers mapped to keys. */
//MCR_API void mcr_Key_modifier_all(struct mcr_context *ctx,
//								  unsigned int *modBuffer, size_t bufferLength);
///*! Get all keys mapped to modifiers. */
//MCR_API void mcr_Key_modifier_key_all(struct mcr_context *ctx,
//									  int *keyBuffer, size_t bufferLength);
///*! Empty all modifier mapping. */
//MCR_API void mcr_Key_modifier_clear(struct mcr_context *ctx);
///*! Map modifier to key and key to modifier. */
//MCR_API int mcr_Key_modifier_set_key(struct mcr_context *ctx,
//									 unsigned int modifiers, int key);
///*! Add a key mapped to modifier. */
//MCR_API int mcr_Key_modifier_add(struct mcr_context *ctx,
//								 unsigned int modifiers,
//								 int key);
///*! Map modifier to key and a set of keys to modifier. */
//MCR_API int mcr_Key_modifier_map(struct mcr_context *ctx,
//								 unsigned int modifiers,
//								 int key, int *addKeys, size_t bufferLen);
///*! Move old key->modifier mapping to new key. */
//MCR_API int mcr_Key_modifier_rekey(struct mcr_context *ctx, int oldKey,
//								   int newKey);
///*! Move old key->modifier mapping to new modifier. */
//MCR_API int mcr_Key_modifier_remod(struct mcr_context *ctx,
//								   unsigned int modifiers, unsigned int newMods);
///*! Minimize mapped keys->modifiers used space. */
//MCR_API void mcr_Key_modifier_trim(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
