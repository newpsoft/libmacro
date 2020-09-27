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

#ifndef MCR_EXTRAS_WINDOWS_P_EXTRAS_H_
#define MCR_EXTRAS_WINDOWS_P_EXTRAS_H_

#include "mcr/extras/base_cpp.h"
#include "mcr/intercept/windows/p_intercept.h"

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

	size_t echo(WPARAM windowsMessage) const;
	size_t echo(DWORD mouseEventFlags) const;
	WPARAM echoWindowsMessage(size_t echo) const;
	DWORD echoMouseEventFlags(size_t echo) const;
	size_t setEcho(WPARAM windowsMessage, DWORD mouseEventFlags, bool updateFlag = true);
	size_t echoCount() const;
	void removeEcho(size_t code);
	void clear();

private:
	Libmacro *_context;
	LibmacroPlatformPrivate *_private;

	void initialize();
	// Set mcr_context array references and overwrite existing values.
	void reset();
	// Set mcr_context array references, and do not write array values.
	void update();
};
}

#endif
