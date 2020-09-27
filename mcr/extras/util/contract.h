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

#ifndef MCR_EXTRAS_CONTRACT_H_
#define MCR_EXTRAS_CONTRACT_H_

#include "mcr/extras/base_cpp.h"
#include "mcr/extras/util/string_less_ci.h"

#include <map>

namespace mcr
{
/*! Map any type of value to its name, and vice versa. */
template<typename T>
class Contract final // And that's final!
{
public:
	T notFound;
	std::map<T, std::string> map;
	std::map<std::string, T, string_less_ci> rmap;

	/*! \ref ctor
	 */
	Contract(const T &notFound) : notFound(notFound)
	{}
	Contract(const Contract &) = default;
	/*! \ref dtor
	 */
	~Contract() = default;
	Contract &operator =(const Contract &) = default;

	T value(const std::string &name) const
	{
		auto it = rmap.find(name);
		return it == rmap.end() ? notFound : it->second;
	}
	T value(const char *name) const
	{
		auto it = rmap.find(name);
		return it == rmap.end() ? notFound : it->second;
	}
	bool empty() const
	{
		return map.empty();
	}
	T maximum() const
	{
		if (empty())
			return notFound;
		return map.rbegin()->first;
	}
	const char *name(T value) const
	{
		auto it = map.find(value);
		return it == map.end() ? nullptr : it->second.c_str();
	}
	void set(T value, const std::string &name)
	{
		map[value] = name;
		rmap[name] = value;
	}
	void add(T value, const std::string &addName)
	{
		rmap[addName] = value;
	}
	void add(T value, const char * const*addNames, size_t count)
	{
		for (size_t i = 0; i < count; i++) {
			add(value, addNames[i]);
		}
	}
	void setMap(T value, const std::string &name, const char * const*addNames,
				size_t count)
	{
		set(value, name);
		add(value, addNames, count);
	}
	void clear()
	{
		map.clear();
		rmap.clear();
	}
};
}

#endif
