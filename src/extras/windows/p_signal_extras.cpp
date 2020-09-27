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

#include "mcr/extras/windows/p_extras.h"
#include "mcr/extras/extras.h"

#include <cstring>

#define winerr mcr_dprint("Windows Error: %ld\n", GetLastError())

namespace mcr
{
// \todo QT Process
void Command::send(mcr_Signal *)
{
	std::string cmdline, mem;
	STARTUPINFOA sInfo;
	PROCESS_INFORMATION pInfo;
	SafeString *array = argsArray();
	size_t count = argCount();
	mcr_throwif(!_file.length(), ENOENT);
	mcr_err = 0;
	ZERO(sInfo);
	ZERO(pInfo);
	cmdline = "\"" + std::string(*_file.text()) + "\"";
	for (size_t i = 0; i < count; i++) {
		mem = *array[i].text();
		if (mem.length())
			cmdline.append(" \"" + mem + '"');
	}
	GetStartupInfoA(&sInfo);
	/* Replace with CreateThread + CreateProcess if CreateProcess does not work */
	/* Create as user, lower securty rights */
	//! \bug Executable with lower access rights, or create process as user
	if (!CreateProcessA(nullptr, &cmdline.front(), nullptr, nullptr, FALSE,
						PROFILE_USER | DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, nullptr, nullptr,
						&sInfo, &pInfo)) {
		winerr;
		mcr_errno(EINTR);
		throw mcr_read_err();
	} else if (WaitForSingleObject(pInfo.hProcess,
								   MCR_INTERCEPT_WAIT_MILLIS) != WAIT_OBJECT_0) {
		winerr;
		mcr_errno(EINTR);
		throw mcr_read_err();
	}
	if (mcr_err) {
		if (!CloseHandle(pInfo.hThread) || !CloseHandle(pInfo.hProcess)) {
			winerr;
		}
		throw mcr_read_err();
	}
}
}
