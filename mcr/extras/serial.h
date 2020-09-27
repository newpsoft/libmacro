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
 *  \brief String contracts for serialization.
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_SERIAL_H_
#define MCR_EXTRAS_SERIAL_H_

#include "mcr/extras/base_cpp.h"

namespace mcr
{
/*! Non-exportable members */
class SerialPrivate;
class MCR_API Serial final // And that's final!
{
	friend class SerialPrivate;
public:
	/*! \ref ctor
	 */
	Serial();
	Serial(const Serial &) = delete;
	/*! \ref dtor
	 */
	~Serial();
	Serial &operator =(const Serial &) = delete;

	int applyType(const char *name) const;
	inline int applyTypeMax() const
	{
		return MCR_TOGGLE;
	}
	inline unsigned int applyTypeCount() const
	{
		return applyTypeMax() + 1;
	}
	const char *applyTypeName(int value) const;

	inline int keyPress(const char *name) const
	{
		return applyType(name);
	}
	inline int keyPressMax() const
	{
		return applyTypeMax();
	}
	inline unsigned int keyPressCount() const
	{
		return applyTypeCount();
	}
	const char *keyPressName(int value) const;

	int dimension(const char *name) const;
	inline int dimensionMax() const
	{
		return MCR_DIMENSION_MAX;
	}
	inline unsigned int dimensionCount() const
	{
		return dimensionMax() + 1;
	}
	const char *dimensionName(int value) const;

	size_t echo(const char *name) const;
	size_t echoMax() const;
	inline size_t echoCount() const
	{
		return echoMax() + 1;
	}
	const char*echoName(size_t value) const;
	void setEcho(size_t value, const char *name);
	void addEcho(size_t value, const char * const*addNames, size_t count);
	void mapEcho(size_t value, const char *name, const char * const*addNames,
				 size_t count);

	int key(const char *name) const;
	int keyMax() const;
	size_t keyCount() const;
	const char*keyName(int value) const;
	void setKey(int value, const char *name);
	void addKey(int value, const char * const*addNames, size_t count);
	void mapKey(int value, const char *name, const char * const*addNames, size_t count);

	unsigned int modifiers(const char *name) const;
	unsigned int modifiersMax() const
	{
		return MCR_MF_USER;
	}
	unsigned int modifiersCount() const;
	const char *modifiersName(unsigned int value) const;

	unsigned int triggerFlags(const char *name) const;
	unsigned int triggerFlagsMax() const
	{
		return MCR_TF_USER;
	}
	unsigned int triggerFlagsCount() const;
	const char *triggerFlagsName(unsigned int value) const;

	int interrupt(const char *name) const;
	int interruptMax() const;
	int interruptCount() const;
	const char *interruptName(int value);
private:
	/* non-export */
	SerialPrivate *_private;

	template<typename vT, typename mT>
	static inline vT nameValue(mT &map, const char *name, vT valNotFound)
	{
		auto it = map.find(name);
		if (it == map.end())
			return valNotFound;
		return it->second;
	}

	void platformInitialize();
};
}

#endif
