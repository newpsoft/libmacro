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
#include "mcr/extras/util/string.h"

#include <cstring>

namespace mcr
{
void String::setString(const char *val, size_t length)
{
	mcr_throwif(length && length != (size_t)~0 && !val, EINVAL);
	if (val && length) {
		/* Need size of buffer, cannot optimize-out strlen. */
		if (length == (size_t)~0)
			length = strlen(val);
		if (!length) {
			clear();
			return;
		}
		if (length != _length) {
			clear();
			_length = length;
			_buffer = new char[size()];
		}
		memcpy(_buffer, val, _length * sizeof(char));
		// Always null-terminate.
		_buffer[_length] = '\0';
	} else {
		clear();
	}
}

void String::resize(size_t val)
{
	if (_length != val) {
		delete[] _buffer;
		_buffer = new char[val + 1];
		/* May have buffer, but string is empty. */
		_length = 0;
		terminate();
	}
}

void String::setLength(size_t val)
{
	mcr_throwif(val && !_buffer, EFAULT);
	if (val != _length) {
		_length = val;
		terminate();
	}
}
}
