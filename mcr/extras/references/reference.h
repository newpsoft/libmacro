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
 *  \brief
 */

#ifndef MCR_EXTRAS_REFERENCES_REFERENCE_H_
#define MCR_EXTRAS_REFERENCES_REFERENCE_H_

#include "mcr/extras/wrappers.h"

namespace mcr
{
/*! Generic pointer reference
 */
template<typename T>
class MCR_API Reference
{
public:
	Reference(Libmacro &context, T *reference = nullptr) :
		_context(&context), _reference(reference)
	{
	}
	Reference(const Reference &) = default;
	virtual ~Reference() {}
	Reference &operator =(const Reference &) = delete;

	inline T &operator *() const
	{
		return *_reference;
	}
	inline T *operator ->() const
	{
		return _reference;
	}
	inline T *operator &() const
	{
		return _reference;
	}
	inline T *operator ()() const
	{
		return _reference;
	}

	Libmacro &context() const
	{
		return _context;
	}
private:
	Libmacro *_context;
	T *_reference;
};

//typedef Reference<mcr_ISignal> ISignalBuilder;
//typedef Reference<mcr_Signal> SignalBuilder;
//typedef Reference<mcr_ITrigger> ITriggerBuilder;
//typedef Reference<mcr_Trigger> TriggerBuilder;
}

#endif
