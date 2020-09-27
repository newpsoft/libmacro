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
ITriggerBuilder::ITriggerBuilder(Libmacro *context)
	: _context(Libmacro::instance(context)), _itrigPt(nullptr)
{
}

void ITriggerBuilder::registerType(const char *name, const char * const*addNames,
							   size_t addNamesCount)
{
	if (_itrigPt) {
		dassert(registry());
		mcr_register(registry(), interface(), name, addNames, addNamesCount);
		if (mcr_err)
			throw mcr_read_err();
	}
}

void ITriggerBuilder::unregisterType()
{
	if (itrigger())
		mcr_unregister(registry(), interface());
}

void ITriggerBuilder::setId(size_t val)
{
	_itrigPt = mcr_ITrigger_from_id(&**_context, val);
}

const char *ITriggerBuilder::name()
{
	return _itrigPt ? mcr_ITrigger_name(&**_context, _itrigPt) : nullptr;
}

void ITriggerBuilder::setName(const char *name)
{
	_itrigPt = mcr_ITrigger_from_name(&**_context, name);
}

mcr_IRegistry *ITriggerBuilder::registry()
{
	return mcr_ITrigger_registry(&**_context);
}
}
