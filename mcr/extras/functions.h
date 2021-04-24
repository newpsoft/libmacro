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

/*! \file
 * \brief C++ Utility functions
 */

#ifndef MCR_EXTRAS_FUNCTIONS_H_
#define MCR_EXTRAS_FUNCTIONS_H_

#include "mcr/base/base.h"

namespace mcr
{
template<typename T>
T &reference(void *pointer)
{
	mcr_dassert(pointer);
	return *reinterpret_cast<T *>(pointer);
}

/*! \pre vT *value must be a member of rT named "self" */
template<typename rT, typename vT>
rT *offset(vT *selfPt)
{
	rT *r = nullptr;
	size_t diff = reinterpret_cast<size_t>(&r->self);
	return reinterpret_cast<rT *>(reinterpret_cast<char *>(selfPt) - diff);
}

/*! \pre vT *value must be a member of rT named "self" */
template<typename rT, typename vT>
const rT *offset(const vT *selfPt)
{
	rT *r = nullptr;
	size_t diff = reinterpret_cast<size_t>(&r->self);
	return reinterpret_cast<const rT *>(reinterpret_cast<const char *>(selfPt) - diff);
}

/*! C++ function to allocate data member to heap. */
template<typename T>
void *newDataMember()
{
	return new T();
}

/*! C++ function to deallocate C++ objects from heap with \ref mcr_Interface */
template<typename T>
void deleteDataMember(void *dataPt)
{
	if (dataPt)
		delete reinterpret_cast<T *>(dataPt);
}

/*! C++ function to reinitialize C++ objects */
template<typename T>
void resetDataMember(void *dataPt)
{
	if (dataPt)
		reference<T>(dataPt) = T();
}

//! \todo Maybe not used */
template<typename T>
mcr_DataMember doufu()
{
	return (mcr_DataMember) {
		.data = new T(), .deallocate = &deleteDataMember<T>
	};
}

extern MCR_API void throwError(const char *lineString, int errorNumber);
}

#endif
