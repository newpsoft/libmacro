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
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_KEY_DISPATCHER_H_
#define MCR_EXTRAS_KEY_DISPATCHER_H_

#include "mcr/extras/util/dispatch_receiver_set.h"

namespace mcr
{
/*! Non-exportable members */
class KeyDispatcherPrivate;
/*! Each key will be at most one modifier, and each modifier will be at
 *  most one key. However, multiple keys may be mapped to the same modifier. */
struct MCR_API KeyDispatcher
{
	friend class KeyDispatcherPrivate;
	const int ANY = MCR_KEY_ANY;

	mcr_AbsKeyDispatcher self;

	KeyDispatcher(Libmacro *context = nullptr);
	KeyDispatcher(const KeyDispatcher &copytron);
	virtual ~KeyDispatcher();
	KeyDispatcher &operator =(const KeyDispatcher &);

	inline mcr_AbsKeyDispatcher &operator *()
	{
		return self;
	}
	inline const mcr_AbsKeyDispatcher &operator *() const
	{
		return self;
	}
	static inline KeyDispatcher *offset(mcr_IDispatcher *originPt)
	{
		return mcr::offset<KeyDispatcher>(originPt);
	}
	static inline const KeyDispatcher *offset(const mcr_IDispatcher *originPt)
	{
		return mcr::offset<KeyDispatcher>(originPt);
	}

	virtual void add(struct mcr_Signal *sigPt, void *receiver, mcr_dispatch_receive_fnc receiverFnc);
	virtual void clear();
	// dispatch = mcr_AbsKeyDispatcher_dispatch
	// modifier = mcr_AbsKeyDispatcher_modifier
	virtual void remove(void *remReceiver);
	virtual void trim();

	virtual unsigned int modifiers(int key) const;
	virtual int key(unsigned int modifiers) const;
	// map key <=> modifier
	virtual void setModifiers(int key, unsigned int modifiers, bool updateFlag = true);
	// map modifier to key
	virtual void addModifiers(int key, unsigned int modifiers, bool updateFlag = true);
	virtual void clearModifiers();

private:
	Libmacro *_context;
	/* non-export */
	KeyDispatcherPrivate *_private;

	void ctor();
	void update();

	static void deinit(struct mcr_IDispatcher *idispPt)
	{
		mcr_throwif(!idispPt, EFAULT);
		offset(idispPt)->clear();
	}
	static int add(struct mcr_IDispatcher *idispPt, struct mcr_Signal *sigPt, void *receiver, mcr_dispatch_receive_fnc receiverFnc);
	static void clear(struct mcr_IDispatcher *idispPt);
	// dispatch = mcr_AbsKeyDispatcher_dispatch
	// modifier = mcr_AbsKeyDispatcher_modifier
	static void remove(struct mcr_IDispatcher *idispPt, void *remReceiver);
	static void trim(struct mcr_IDispatcher *idispPt);
};
}

#endif
