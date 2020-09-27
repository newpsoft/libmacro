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

#include <algorithm>
#include <cstdio>
#include <map>
#include <vector>

#include "mcr/extras/util/dispatch_receiver_less.h"
#include "mcr/extras/util/dispatch_receiver_map.h"

#define priv _private

namespace mcr
{
class DispatcherPrivate
{
	friend struct Dispatcher;
public:
	DispatchReceiverMap<mcr_Signal *> signalReceivers;
};

Dispatcher::Dispatcher(Libmacro *context)
	: _context(Libmacro::instance(context)), _receivers(&self.receivers,
			&self.receiver_count),
	  priv(new DispatcherPrivate)
{
	mcr_AbsDispatcher_ctor(&self, &**_context, Dispatcher::deinit, Dispatcher::add, Dispatcher::clear, Dispatcher::modifier, Dispatcher::remove, Dispatcher::trim);
	priv->signalReceivers.setApplyReceivers(&self.signal_receivers,
											&self.signal_receiver_count);
}

Dispatcher::Dispatcher(const Dispatcher &copytron)
	: _context(copytron._context), _receivers(&self.receivers, &self.receiver_count),
	  priv(new DispatcherPrivate)
{
	mcr_AbsDispatcher_ctor(&self, &**_context, Dispatcher::deinit, Dispatcher::add, Dispatcher::clear, Dispatcher::modifier, Dispatcher::remove, Dispatcher::trim);
	*priv = *copytron.priv;
	priv->signalReceivers.setApplyReceivers(&self.signal_receivers,
											&self.signal_receiver_count);
}

Dispatcher::~Dispatcher()
{
	delete priv;
}

Dispatcher &Dispatcher::operator =(const Dispatcher &copytron)
{
	if (&copytron != this) {
		*priv = *copytron.priv;
		priv->signalReceivers.setApplyReceivers(&self.signal_receivers,
												&self.signal_receiver_count);
	}
	return *this;
}

void Dispatcher::add(struct mcr_Signal *sigPt, void *receiver,
					 mcr_dispatch_receive_fnc receiverFnc)
{
	mcr_assert_receive_function(receiver, receiverFnc);
	if (sigPt) {
		priv->signalReceivers.add(sigPt, receiver, receiverFnc);
	} else {
		_receivers.add(receiver, receiverFnc);
	}
}

void Dispatcher::clear()
{
	priv->signalReceivers.clear();
	_receivers.clear();
}

void Dispatcher::remove(void *remReceiver)
{
	mcr_throwif(!remReceiver, EFAULT);
	priv->signalReceivers.remove(remReceiver);
	_receivers.remove(remReceiver);
}

void Dispatcher::trim()
{
	priv->signalReceivers.trim();
	_receivers.trim();
}

int Dispatcher::add(struct mcr_IDispatcher *idispPt,
					struct mcr_Signal *sigPt, void *receiver, mcr_dispatch_receive_fnc receiverFnc)
{
	mcr_throwif(!idispPt, EFAULT);
	Dispatcher *dispPt = offset(idispPt);
	try {
		dispPt->add(sigPt, receiver, receiverFnc);
	} catch (int e) {
		return e;
	}
	return 0;
}

void Dispatcher::clear(struct mcr_IDispatcher *idispPt)
{
	mcr_throwif(!idispPt, EFAULT);
	Dispatcher *dispPt = offset(idispPt);
	dispPt->clear();
}

void Dispatcher::modifier(struct mcr_IDispatcher *idispPt,
						  struct mcr_Signal *sigPt, unsigned int *modifiersPt)
{
	mcr_throwif(!idispPt, EFAULT);
	Dispatcher *dispPt = offset(idispPt);
	dispPt->modifier(sigPt, modifiersPt);
}

void Dispatcher::remove(struct mcr_IDispatcher *idispPt, void *remReceiver)
{
	mcr_throwif(!idispPt, EFAULT);
	Dispatcher *dispPt = offset(idispPt);
	dispPt->remove(remReceiver);
}

void Dispatcher::trim(struct mcr_IDispatcher *idispPt)
{
	mcr_throwif(!idispPt, EFAULT);
	Dispatcher *dispPt = offset(idispPt);
	dispPt->trim();
}
}
