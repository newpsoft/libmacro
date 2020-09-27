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

#include "mcr/extras/extras.h"

namespace mcr
{
ISignal::ISignal(Libmacro *context, mcr_signal_fnc send,
				 mcr_IDispatcher *dispatcher)
	: _context(Libmacro::instance(context))
{
	mcr_ISignal_init(&self);
	interface().context = &**_context;
	self.send = send;
	self.dispatcher_pt = dispatcher;
}

IDispatcher::IDispatcher(Libmacro *context, void (*deinit)(mcr_IDispatcher *), mcr_IDispatcher_add_fnc add,
						 void (*clear)(struct mcr_IDispatcher *), mcr_IDispatcher_dispatch_fnc dispatch,
						 mcr_IDispatcher_modify_fnc modifier, mcr_IDispatcher_remove_fnc remove,
						 void (*trim)(struct mcr_IDispatcher *))
	: _context(Libmacro::instance(context))
{
	mcr_IDispatcher_ctor(&self, &**_context, deinit, add, clear, dispatch, modifier, remove,
						 trim);
}

ITrigger::ITrigger(Libmacro *context, mcr_Trigger_receive_fnc receive)
	: _context(Libmacro::instance(context))
{
	mcr_ITrigger_init(&self);
	interface().context = &**_context;
	self.receive = receive;
}

void Signal::setISignal(mcr_ISignal *isignal)
{
	if (isignal != self.isignal) {
		mcr_Signal_deinit(&self);
		mcr_throwif(mcr_err, mcr_read_err());
		self.isignal = isignal;
	}
}

void Trigger::setITrigger(mcr_ITrigger *itrigger)
{
	if (itrigger != self.itrigger) {
		mcr_Trigger_deinit(&self);
		mcr_throwif(mcr_err, mcr_read_err());
		self.itrigger = itrigger;
	}
}
}
