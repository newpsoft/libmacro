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
 *  \brief String contract registry for a primitive type.
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_DISPATCHER_H_
#define MCR_EXTRAS_DISPATCHER_H_

#include "mcr/extras/util/dispatch_receiver_set.h"

namespace mcr
{
/*! Non-exportable members */
class DispatcherPrivate;
struct MCR_API Dispatcher
{
	friend class DispatcherPrivate;
	mcr_AbsDispatcher self;

	Dispatcher(Libmacro *ctx = nullptr);
	Dispatcher(const Dispatcher &copytron);
	virtual ~Dispatcher();
	Dispatcher &operator =(const Dispatcher &copytron);

	inline mcr_AbsDispatcher &operator *()
	{
		return self;
	}
	inline const mcr_AbsDispatcher &operator *() const
	{
		return self;
	}
	static inline Dispatcher *offset(mcr_IDispatcher *originPt)
	{
		return mcr::offset<Dispatcher>(originPt);
	}
	static inline const Dispatcher *offset(const mcr_IDispatcher *originPt)
	{
		return mcr::offset<Dispatcher>(originPt);
	}

	virtual void add(struct mcr_Signal *sigPt, void *receiver, mcr_dispatch_receive_fnc receiverFnc);
	virtual void clear();
	// dispatch = mcr_AbsKeyDispatcher_dispatch
	virtual void modifier(struct mcr_Signal *, unsigned int *) {}
	virtual void remove(void *remReceiver);
	virtual void trim();

private:
	Libmacro *_context;
	DispatchReceiverSet _receivers;
	/* non-export */
	DispatcherPrivate *_private;

	static void deinit(struct mcr_IDispatcher *idispPt)
	{
		mcr_throwif(!idispPt, EFAULT);
		offset(idispPt)->clear();
	}
	static int add(struct mcr_IDispatcher *idispPt, struct mcr_Signal *sigPt, void *receiver, mcr_dispatch_receive_fnc receiverFnc);
	static void clear(struct mcr_IDispatcher *idispPt);
	// dispatch = mcr_AbsDispatcher_dispatch
	static void modifier(struct mcr_IDispatcher *idispPt, struct mcr_Signal *sigPt, unsigned int *modifiersPt);
	static void remove(struct mcr_IDispatcher *idispPt, void *remReceiver);
	static void trim(struct mcr_IDispatcher *idispPt);
};
}

#endif
