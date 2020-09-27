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

#ifndef MCR_EXTRAS_LINUX_P_EXTRAS_H_
#define MCR_EXTRAS_LINUX_P_EXTRAS_H_

#include "mcr/extras/base_cpp.h"
#include "mcr/intercept/linux/p_intercept.h"

#include "mcr/extras/util/string.h"

namespace mcr
{
class LibmacroPrivate;
/*! Non-exportable members */
class LibmacroPlatformPrivate;
class MCR_API LibmacroPlatform final
{
	friend struct Libmacro;
	friend class LibmacroPrivate;
	friend class LibmacroPlatformPrivate;
public:
	LibmacroPlatform(Libmacro &context);
	LibmacroPlatform(const LibmacroPlatform &copytron);
	~LibmacroPlatform();
	LibmacroPlatform &operator =(const LibmacroPlatform &copytron);

	size_t echo(const mcr_Key &val) const;
	mcr_Key echoKey(size_t echo) const;
	size_t setEcho(const mcr_Key &val, bool updateFlag = true);
	size_t echoCount() const;
	void removeEcho(size_t code);
	void updateEchos();

	// key=>echo
	// grab path add, remove, set all
	size_t grabCount() const;
	void setGrabCount(size_t count, bool updateFlag = true);
	String grab(size_t index) const;
	void setGrab(size_t index, const String &value, bool updateFlag = true);
	inline void setGrab(size_t index, const std::string &value, bool updateFlag = true)
	{
		setGrab(index, String(value.c_str(), value.size()), updateFlag);
	}
	void updateGrabs();

	void clear();

	static inline bool valid(mcr_ApplyType keyApply)
	{
		return keyApply == MCR_SET || keyApply == MCR_UNSET;
	}

private:
	Libmacro *_context;
	LibmacroPlatformPrivate *_private;

	void initialize();
	void reset();
};
}

#endif
