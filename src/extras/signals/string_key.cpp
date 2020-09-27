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
IKeyProvider *StringKey::_keyProvider = nullptr;

int StringKey::compare(const StringKey &rhs) const
{
	int cmp;
	if (&rhs == this)
		return 0;
	if ((cmp = std::memcmp(&interval, &rhs.interval, sizeof(interval))))
		return cmp;
	return string.compare(rhs.string);
}

void StringKey::copy(const IDataMember *copytron)
{
	const StringKey *cPt = dynamic_cast<const StringKey *>(copytron);
	if (!cPt)
		throw EFAULT;
	if (cPt != this) {
		interval = cPt->interval;
		string = cPt->string;
	}
}

void StringKey::send(mcr_Signal *signalPt)
{
	Libmacro *context = Libmacro::offset(signalPt->interface->context);
	mcr_throwif(!context, EFAULT);
	String str = text();
	char c;
	size_t totalCount = context->characterCount(), characterCount, j;
	for (size_t i = 0; i < str.size(); i++) {
		c = str[i];
		if (static_cast<size_t>(str[c]) < totalCount) {
			auto *chara = context->characterData(c);
			characterCount = context->characterCount(c);
			for (j = 0; j < characterCount; j++) {
				if (mcr_send(&**context, &*chara[j]))
					throw mcr_err;
			}
			mcr_NoOp_send_member(&interval);
		}
	}
}
}
