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

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_UTIL_STRING_H_
#define MCR_EXTRAS_UTIL_STRING_H_

#include "mcr/extras/base_cpp.h"

#include <string>

namespace mcr
{
// \todo std::string
/*! Self-destructing, temporary copy of C-String. */
class MCR_API String
{
public:
	String(const char *val = nullptr, size_t length = (size_t)~0)
		: _buffer(nullptr), _length(0)
	{
		setString(val, length);
	}
	String(const std::string &val)
		: _buffer(nullptr), _length(0)
	{
		setString(val.c_str(), val.size());
	}
	String(const String&copytron)
		: _buffer(nullptr), _length(0)
	{
		setString(copytron._buffer, copytron._length);
	}
	virtual ~String()
	{
		clear();
	}
	inline String &operator =(const String &copytron)
	{
		if (&copytron != this)
			setString(copytron._buffer, copytron._length);
		return *this;
	}
	inline String &operator =(const char *val)
	{
		setString(val);
		return *this;
	}
	/*! True if not empty */
	inline operator bool() const
	{
		return _buffer && _length && _buffer[0] != '\0';
	}
	/*! True if empty */
	inline bool operator !() const
	{
		return empty();
	}

	/*! This object as C-String.  Will always be valid, or at least an empty
	 *  C-String. */
	inline const char *operator *() const
	{
		return empty() ? "" : string();
	}

	inline char &operator[](size_t index)
	{
		return _buffer[index];
	}
	inline const char &operator[](size_t index) const
	{
		return _buffer[index];
	}

	inline char *string()
	{
		return _buffer;
	}
	inline const char *string() const
	{
		return _buffer;
	}
	void setString(const char *val = nullptr, size_t length = (size_t)~0);
	inline void terminate()
	{
		if (_buffer)
			_buffer[_length] = '\0';
	}

	inline bool empty() const
	{
		return !_buffer || !_length || _buffer[0] == '\0';
	}

	inline size_t length() const
	{
		return _length;
	}
	inline size_t size() const
	{
		return _length + 1;
	}

	inline void clear()
	{
		if (_buffer) {
			delete[] _buffer;
			_buffer = nullptr;
		}
		_length = 0;
	}
	/*! Reset char buffer to new string size.
	 *
	 *  Warning: This will reinitialize to an empty string[0] = '\0'\n
	 *  This is useful to use String as a buffer, but must be manually
	 *  null-terminated, e.g. \ref setLength. */
	void resize(size_t val);
	/*! Null-terminate and set the string length, without modifying
	 *  the char buffer.
	 *
	 *  \pre Must have buffer length large enough for new string length. */
	void setLength(size_t val);
private:
	char *_buffer;
	size_t _length;
};
}

#endif
