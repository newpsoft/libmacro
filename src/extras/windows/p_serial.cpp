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

#include "mcr/libmacro.h"

namespace mcr
{
static void addKeys(Serial &serial);
static void addEchoes(Serial &serial);

void Serial::platformInitialize()
{
	addKeys(*this);
	addEchoes(*this);
}

static void addKeys(Serial &serial)
{
	const char *names[] = {
		/*0x00 - 0x0F */
		"None", "LButton", "RButton", "Cancel",
		"MButton", "XButton1", "XButton2", nullptr,
		"Back", "Tab", nullptr, nullptr,
		"Clear", "Return", nullptr, nullptr,
		/*0x10 - 0x1F */
		"Shift", "Control", "Menu", "Pause",
		"Capital", "Kana", nullptr, "Junja",
		"Final", "Hanja", nullptr, "Escape",
		"Convert", "NonConvert", "Accept", "ModeChange",
		/*0x20 - 0x2F */
		"Space", "Prior", "Next", "End",
		"Home", "Left", "Up", "Right",
		"Down", "Select", "Print", "Execute",
		"Snapshot", "Insert", "Delete", "Help",
		/*0x30 - 0x3F */
		"0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9", nullptr, nullptr,	nullptr, nullptr, nullptr, nullptr,
		/*0x40 - 0x4F */
		nullptr, "A", "B", "C",	"D", "E", "F", "G",
		"H", "I", "J", "K", "L", "M", "N", "O",
		/*0x50 - 0x5F */
		"P", "Q", "R", "S", "T", "U", "V", "W",
		"X", "Y", "Z", "LWin", "RWin", "Apps", nullptr, "Sleep",
		/*0x60 - 0x6F */
		"Numpad0", "Numpad1", "Numpad2", "Numpad3",
		"Numpad4", "Numpad5", "Numpad6", "Numpad7",
		"Numpad8", "Numpad9", "Multiply", "Add",
		"Seperator", "Subtract", "Decimal", "Divide",
		/*0x70 - 0x7F */
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
		"F9", "F10", "F11",	"F12", "F13", "F14", "F15", "F16",
		"F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/*0x90 - 0x9F */
		"Numlock", "Scroll", nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr,
		/*0xA0 - 0xAF */
		"LShift", "RShift", "LControl", "RControl",
		"LMenu", "RMenu", "BrowserBack", "BrowserForward",
		"BrowserRefresh", "BrowserStop", "BrowserSearch", "BrowserFavorites",
		"BrowserHome", "VolumeMute", "VolumeDown", "VolumeUp",
		/*0xB0 - 0xBF */
		"MediaNextTrack", "MediaPrevTrack", "MediaStop", "MediaPlayPause",
		"LaunchMail", "LaunchMediaSelect", "LaunchApp1", "LaunchApp2",
		nullptr, nullptr, ";", "+", ",", "-", ".", "/",
		/*0xC0 US '`~' - 0xCF */
		"`", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/*0xD0 - 0xDF */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, "[", "\\", "]", "'", "OEM_8",
		/*0xE0 - 0xEF */
		nullptr, nullptr, "¥", nullptr, nullptr, "Process", nullptr, "Packet",
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/*0xF0 - 0xFE */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Attn", "Crsel",
		"Exsel", "Ereof", "Play", "Zoom", "Noname", "Pa1", "OEM_CLEAR"
		/* 0xFF */
	};
	const char *addNames[] = {
		" ", ":", "?",
		"~", "{", "|",
		"}", "\""
	};
	const int addKeys[] = {
		VK_SPACE, VK_OEM_1, VK_OEM_2,
		VK_OEM_3, VK_OEM_4, VK_OEM_5,
		VK_OEM_6, VK_OEM_7
	};
	int i;
	dassert(arrlen(names) == 0xFF);
	dassert(arrlen(addNames) == arrlen(addKeys));
	for (i = 0; i < 0xFF; i++) {
		if (names[i])
			serial.setKey(i, names[i]);
	}
	for (i = arrlen(addNames); i--;) {
		serial.addKey(addKeys[i], addNames + 1, 1);
	}
}

static void addEchoes(Serial &serial)
{
	const char *names[] = {
		"LeftDown", "LeftUp",
		"MiddleDown", "MiddleUp",
		"RightDown", "RightUp",
		"XDown", "XUp"
	};
	const char *addNames[8][2] = {
		{"Left Down", "left_down"}, {"Left Up", "left_up"},
		{"Middle Down", "middle_down"}, {"Middle Up", "middle_up"},
		{"Right Down", "right_down"}, {"Right Up", "right_up"},
		{"X Down", "x_down"}, {"X Up", "x_up"}
	};
	size_t i = 0, count = arrlen(names);
	for (i = 0; i < count; i++) {
		serial.mapEcho(i, names[i], addNames[i], 2);
	}
}
}
