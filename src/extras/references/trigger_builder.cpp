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
TriggerBuilder::TriggerBuilder(Libmacro *context)
	: _context(Libmacro::instance(context)), _trigPt(nullptr)
{
}

void TriggerBuilder::setITrigger(mcr_ITrigger *itrigPt)
{
	if (trigger() && itrigger() != itrigPt) {
		deinit();
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerBuilder::copy(const mcr_Trigger *copytron)
{
	if (_trigPt) {
		if (mcr_Trigger_copy(_trigPt, copytron))
			throw mcr_read_err();
	}
}
}
