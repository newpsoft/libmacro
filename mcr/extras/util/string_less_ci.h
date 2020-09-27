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

#ifndef MCR_EXTRAS_UTIL_STRING_LESS_CI_H_
#define MCR_EXTRAS_UTIL_STRING_LESS_CI_H_

#include "mcr/extras/base_cpp.h"

#include <string>
#include <functional>

namespace mcr
{
/*! Case-insensitive less-comparison for case-insensitive STL. */
struct string_less_ci : std::binary_function<std::string, std::string, bool>
{
	// case-independent (ci) compare_less binary function
	struct nocase_compare : public
		std::binary_function<unsigned char,unsigned char,bool> {
		bool operator()(const unsigned char& c1, const unsigned char& c2) const
		{
			return tolower(c1) < tolower(c2);
		}
	};
	bool operator()(const std::string & s1, const std::string & s2) const
	{
		return std::lexicographical_compare
			   (s1.begin(), s1.end(),   // source range
				s2.begin(), s2.end(),   // dest range
				nocase_compare());  // comparison
	}
};
}

#endif
