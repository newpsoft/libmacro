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
SignalBuilder::SignalBuilder(Libmacro *context)
	: _context(Libmacro::instance(context)), _sigPt(nullptr)
{
}

void SignalBuilder::setISignal(mcr_ISignal *isigPt)
{
	if (signal() && isignal() != isigPt) {
		deinit();
		_sigPt->isignal = isigPt;
	}
}

bool SignalBuilder::dispatch()
{
	if (_sigPt)
		return mcr_dispatch(&**_context, _sigPt);
	return false;
}

void SignalBuilder::send()
{
	if (_sigPt) {
		if (mcr_send(&**_context, _sigPt))
			throw mcr_read_err();
	}
}

void SignalBuilder::copy(const mcr_Signal *copytron)
{
	if (_sigPt) {
		if (mcr_Signal_copy(_sigPt, copytron))
			throw mcr_read_err();
	}
}
}
