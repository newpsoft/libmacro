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
 *  \brief \ref IData - Data type for instances
 *  C++.
 */

#ifndef MCR_EXTRAS_IDATA_MEMBER_H_
#define MCR_EXTRAS_IDATA_MEMBER_H_

#include "mcr/extras/base_cpp.h"

namespace mcr
{
/*! Instance data, such as \ref mcr_DataMember member of \ref mcr_Instance.
 *
 *  Functions to overload: \ref IDataMember.compare, \ref IDataMember.copy,
 *  \ref IDataMember.name \n
 *  Optional overload: \ref IDataMember.addNameCount and
 *  \ref IDataMember.addNames
 */
class MCR_API IDataMember
{
public:
	MCR_DECL_INTERFACE(IDataMember)

	inline bool operator ==(const IDataMember &rhs)
	{
		return !compare(rhs);
	}
	inline bool operator <(const IDataMember &rhs)
	{
		return compare(rhs) < 0;
	}
	inline bool operator >(const IDataMember &rhs)
	{
		return compare(rhs) > 0;
	}

	/* Instance */
	/*! \ref mcr_Instance_compare */
	virtual int compare(const IDataMember &rhs) const = 0;
	/*! \ref mcr_Instance_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const IDataMember *copytron) = 0;
	/* Instance */
	/*! \ref mcr_Interface_set_name */
	virtual const char *name() const = 0;
	/*! \ref opt \ref mcr_Interface_add_names */
	virtual size_t addNameCount() const
	{
		return 0;
	}
	/*! \ref opt \ref mcr_Interface_add_names */
	virtual void addNames(const char **, size_t) const
	{
	}
};
}

#endif
