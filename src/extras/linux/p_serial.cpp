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

#include <linux/input.h>	/* input_event */

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
		"None", "Esc", "1", "2", "3",
		"4", "5", "6", "7", "8",
		"9", "0", "Minus", "Equal", "Backspace",
		"Tab", "Q", "W", "E", "R",
		"T", "Y", "U", "I", "O",
		"P", "LeftBrace", "RightBrace", "Enter", "LeftCtrl",
		"A", "S", "D", "F", "G",
		"H", "J", "K", "L", "Semicolon",
		"Apostrophe", "Grave", "LeftShift", "Backslash", "Z",
		"X", "C", "V", "B", "N",
		"M", "Comma", "Dot", "Slash", "RightShift",
		"KPAsterisk", "LeftAlt", "Space", "CapsLock", "F1",
		"F2", "F3", "F4", "F5", "F6",
		"F7", "F8", "F9", "F10", "NumLock",
		"ScrollLock", "KP7", "KP8", "KP9", "KPMinus",
		"KP4", "KP5", "KP6", "KPPlus", "KP1",
		"KP2", "KP3", "KP0", "KPdot", nullptr,
		"ZenkakuHankaku", "102ND", "F11", "F12", "RO",
		"Katakana", "Hiragana", "Henkan", "KatakanaHiragana",
		"Muhenkan",
		"KPJPComma", "KPEnter", "RightCtrl", "KPSlash", "SysRQ",
		"RightAlt", "LineFeed", "Home", "Up", "PageUp",
		"Left", "Right", "End", "Down", "PageDown",
		"Insert", "Delete", "Macro", "Mute", "VolumeDown",
		"VolumeUp", "Power", "KPEqual", "KPPlusMinus", "Pause",
		"Scale", "KPComma", "Hangeul", "Hanja", "Yen",
		"LeftMeta", "RightMeta", "Compose", "Stop", "Again",
		"Props", "Undo", "Front", "Copy", "Open",
		"Paste", "Find", "Cut", "Help", "Menu",
		"Calc", "Setup", "Sleep", "Wakeup", "File",
		"SendFile", "DeleteFile", "Xfer", "Prog1", "Prog2",
		"www", "MsDOS", "ScreenLock", "Direction", "CycleWindows",
		"Mail", "Bookmarks", "Computer", "Back", "Forward",
		"CloseCD", "EjectCD", "EjectCloseCD", "NextSong", "PlayPause",
		"PreviousSong", "StopCD", "Record", "Rewind", "Phone",
		"ISO", "Config", "HomePage", "Refresh", "Exit",
		"Move", "Edit", "ScrollUp", "ScrollDown", "KPLeftParen",
		"KPRightParen", "New", "Redo", "F13", "F14",
		"F15", "F16", "F17", "F18", "F19",
		"F20", "F21", "F22", "F23", "F24",	/* 194 */
		/* 195 - 199 */
		nullptr, nullptr, nullptr, nullptr, nullptr,
		"PlayCD", "PauseCD", "Prog3", "Prog4", "Dashboard",
		"Suspend", "Close", "Play", "FastForward", "BassBoost",
		"Print", "HP", "Camera", "Sound", "Question",
		"Email", "Chat", "Search", "Connect", "Finance",
		"Sport", "Shop", "AltErase", "Cancel", "BrightnessDown",
		"BrightnessUp", "Media", "SwitchVideoMode", "KBDIllumToggle",
		"KBDIllumDown",
		"KBDIllumUp", "Send", "Reply", "ForwardMail", "Save",
		"Documents", "Battery", "Bluetooth", "WLAN", "UWB",
		nullptr,
		"VideoNext", "VideoPrev", "BrightnessCycle",
		"BrightnessZero",
		"DisplayOff", "WWAN", "RFKill", "MicMute",
		/* 249 - 254 */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, "BTN_0", "BTN_1", "BTN_2", "BTN_3",
		"BTN_4", "BTN_5", "BTN_6", "BTN_7", "BTN_8", "BTN_9",
		/* 0x10a - 0x10f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		"BTN_Left", "BTN_Right", "BTN_Middle", "BTN_Side", "BTN_Extra",
		"BTN_Forward", "BTN_Back", "BTN_Task",
		/* 0x118 - 0x11f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		"BTN_Trigger", "BTN_Thumb",
		"BTN_Thumb2", "BTN_Top", "BTN_Top2", "BTN_Pinkie", "BTN_Base",
		"BTN_Base2", "BTN_Base3", "BTN_Base4", "BTN_Base5", "BTN_Base6",
		/* 0x12c - 0x12e */
		nullptr, nullptr, nullptr,
		"BTN_Dead", "BTN_A", "BTN_B", "BTN_C", "BTN_X",
		"BTN_Y", "BTN_Z", "BTN_TL", "BTN_TR", "BTN_TL2",
		"BTN_TR2", "BTN_Select", "BTN_Start", "BTN_Mode", "BTN_ThumbL",
		"BTN_ThumbR",
		nullptr,
		"BTN_Tool_Pen", "BTN_Tool_Rubber", "BTN_Tool_Brush",
		"BTN_Tool_Pencil", "BTN_Tool_Airbrush",
		"BTN_Tool_Finger", "BTN_Tool_Mouse", "BTN_Tool_Lens",
		"BTN_Tool_QuintTap",
		nullptr,
		"BTN_Touch", "BTN_Stylus", "BTN_Stylus2", "BTN_Tool_DoubleTap",
		"BTN_Tool_TripleTap",
		"BTN_Tool_QuadTap", "BTN_Gear_Down", "BTN_Gear_Up",
		/* 0x152 - 0x15f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		"OK", "Select", "Goto", "Clear", "Power2",
		"Option", "Info", "Time", "Vendor", "Archive",
		"Program", "Channel", "Favorites", "EPG", "PVR",
		"MHP", "Language", "Title", "Subtitle", "Angle",
		"Zoom", "Mode", "Keyboard", "Screen", "PC",
		"TV", "TV2", "VCR", "VCR2", "SAT",
		"SAT2", "CD", "Tape", "Radio", "Tuner",
		"Player", "Text", "DVD", "AUX", "MP3",
		"Audio", "Video", "Directory", "List", "Memo",
		"Calendar", "Red", "Green", "Yellow", "Blue",
		"ChannelUp", "ChannelDown", "First", "Last", "AB",
		"Next", "Restart", "Slow", "Shuffle", "Break",
		"Previous", "Digits", "TEEN", "TWEN", "VideoPhone",
		"Games", "ZoomIn", "ZoomOut", "ZoomReset", "WordProcessor",
		"Editor", "Spreadsheet", "GraphicsEditor", "Presentation",
		"Database",
		"News", "VoiceMail", "AddressBook", "Messenger",
		"DisplayToggle",
		"SpellCheck", "LogOff", "Dollar", "Euro", "FrameBack",
		"FrameForward", "ContextMenu", "MediaRepeat", "10ChannelsUp",
		"10ChannelsDown",
		"Images",
		/* 0x1bb - 0x1bf */
		nullptr, nullptr, nullptr, nullptr, nullptr,
		"DEL_EOL", "DEL_EOS", "INS_Line", "DEL_Line",
		/* 0x1c4 - 0x1cf */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr,
		"FN", "FN_Esc", "FN_F1", "FN_F2", "FN_F3",
		"FN_F4", "FN_F5", "FN_F6", "FN_F7", "FN_F8",
		"FN_F9", "FN_F10", "FN_F11", "FN_F12", "FN_1",
		"FN_2", "FN_D", "FN_E", "FN_F", "FN_S",
		"FN_B",
		/* 0x1e5 - 0x1f0 */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr,
		"BRL_Dot1", "BRL_Dot2", "BRL_Dot3", "BRL_Dot4", "BRL_Dot5",
		"BRL_Dot6", "BRL_Dot7", "BRL_Dot8", "BRL_Dot9", "BRL_Dot10",
		/* 0x1fb - 0x1ff */
		nullptr, nullptr, nullptr, nullptr, nullptr,
		"Numeric_0", "Numeric_1", "Numeric_2", "Numeric_3", "Numeric_4",
		"Numeric_5", "Numeric_6", "Numeric_7", "Numeric_8", "Numeric_9",
		"Numeric_Star", "Numeric_Pound",
		/* 0x20c - 0x20f */
		nullptr, nullptr, nullptr, nullptr,
		"CameraFocus", "WPS_Button",

		"TouchpadToggle", "TouchpadOn", "TouchpadOff",

		"CameraZoomIn", "CameraZoomOut", "CameraUp",
		"CameraDown", "CameraLeft",
		"CameraRight", "AttendantOn", "AttendantOff",
		"AttendantToggle", "LightsToggle",
		nullptr,
		"BTN_DPAD_Up", "BTN_DPAD_Down", "BTN_DPAD_Left",
		"BTN_DPAD_Right",
		/* 0x224 - 0x22f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr,
		/* 0x230 */
		"ALS_Toggle",
		/* 0x231 - 0x23f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x240 - 0x24f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x250 - 0x25f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x260 - 0x26f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x270 - 0x27f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x280 - 0x28f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x290 - 0x29f */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x2a0 - 0x2af */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		/* 0x2b0 - 0x2bf */
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		"TriggerHappy", "TriggerHappy2", "TriggerHappy3",
		"TriggerHappy4", "TriggerHappy5", "TriggerHappy6",
		"TriggerHappy7", "TriggerHappy8", "TriggerHappy9",
		"TriggerHappy10", "TriggerHappy11", "TriggerHappy12",
		"TriggerHappy13", "TriggerHappy14", "TriggerHappy15",
		"TriggerHappy16", "TriggerHappy17", "TriggerHappy18",
		"TriggerHappy19", "TriggerHappy20", "TriggerHappy21",
		"TriggerHappy22", "TriggerHappy23", "TriggerHappy24",
		"TriggerHappy25", "TriggerHappy26", "TriggerHappy27",
		"TriggerHappy28", "TriggerHappy29", "TriggerHappy30",
		"TriggerHappy31", "TriggerHappy32", "TriggerHappy33",
		"TriggerHappy34", "TriggerHappy35", "TriggerHappy36",
		"TriggerHappy37", "TriggerHappy38", "TriggerHappy39",
		"TriggerHappy40"
	};
	const char *addNames[][6] = {
		{"reserved"},
		{"escape"}, {"one"}, {"two"}, {"three"}, {"four"},
		{"five"}, {"six"}, {"seven"}, {"eight"}, {"nine"},
		{"zero"}, {"left brace", "left_brace", "[", "{"},
		{"right brace", "right_brace", "]", "}"}, {
			"left ctrl", "left control",
			"left_ctrl", "left_control", "ctrl", "control"
		},
		{"left shift", "left_shift", "shift"},
		{"right shift", "right_shift"}, {"left alt", "left_alt", "alt"},
		{"caps lock", "caps_lock"},
		{"num lock", "num_lock"}, {"scroll lock", "scroll_lock"},
		{"zenkaku hankaku", "zenkaku_hankaku"},
		{"katagana hiragana", "katagana_hiragana"}, {
			"right ctrl",
			"right_ctrl", "right control", "right_control"
		},
		{"right alt", "right_alt"},
		{"line feed", "line_feed"}, {"page up", "page_up"},
		{"page down", "page_down"}, {"volume down", "volume_down"},
		{"volume up", "volume_up"}, {"left meta", "left_meta", "meta"},
		{"right meta", "right_meta"}, {"scroll up", "scroll_up"},
		{"scroll down", "scroll_down"},

		{"hanguel"}, {"coffee"}, {"brightnessauto"}, {"wimax"},
		{"btn_misc"}, {"btn_mouse"}, {"btn_joystick"},
		{"btn_gamepad"}, {"btn_south"}, {"btn_east"},
		{"btn_north"}, {"btn_west"}, {"btn_digi"},
		{"btn_wheel"}, {"triggerhappy1"}, {"`"}, {"-"}, {"="}
	};
	const int addKeys[] = {
		KEY_RESERVED,
		KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4,
		KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
		KEY_0, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_LEFTCTRL,
		KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTALT, KEY_CAPSLOCK,
		KEY_NUMLOCK, KEY_SCROLLLOCK, KEY_ZENKAKUHANKAKU,
		KEY_KATAKANAHIRAGANA, KEY_RIGHTCTRL, KEY_RIGHTALT,
		KEY_LINEFEED, KEY_PAGEUP, KEY_PAGEDOWN, KEY_VOLUMEDOWN,
		KEY_VOLUMEUP, KEY_LEFTMETA, KEY_RIGHTMETA, KEY_SCROLLUP,
		KEY_SCROLLDOWN,

		KEY_HANGUEL, KEY_COFFEE, KEY_BRIGHTNESS_ZERO, KEY_WIMAX,
		BTN_MISC, BTN_MOUSE, BTN_JOYSTICK,
		BTN_GAMEPAD, BTN_SOUTH, BTN_EAST,
		BTN_NORTH, BTN_WEST, BTN_DIGI,
		BTN_WHEEL, BTN_TRIGGER_HAPPY1, KEY_GRAVE, KEY_MINUS, KEY_EQUAL
	};
	const size_t addLens[] = {
		1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 4,
		4, 6,
		3, 2, 3, 2,
		2, 2, 2,
		2, 4, 2,
		2, 2, 2, 2,
		2, 3, 2, 2,
		2,

		1, 1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1, 1, 1
	};
	int i;
	dassert(arrlen(names) == BTN_TRIGGER_HAPPY40 + 1);
	dassert(arrlen(addNames) == arrlen(addKeys));
	for (i = BTN_TRIGGER_HAPPY40 + 1; i--;) {
		if (names[i])
			serial.setKey(i, names[i]);
	}
	for (i = arrlen(addNames); i--;) {
		serial.addKey(addKeys[i], addNames[i], addLens[i]);
	}
}

static void addEchoes(Serial &serial)
{
	const char *names[] = {
		"Left", "Middle", "Right",
		"0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9", "Side",
		"Extra", "Forward", "Back", "Task", "Trigger",
		"Thumb", "Thumb2", "Top", "Top2", "Pinkie",
		"Base", "Base2", "Base3", "Base4", "Base5",
		"Base6", "A" /* SOUTH */, "B" /* EAST */,
		"C", "X" /* NORTH */, "Y" /* WEST */,
		"Z", "TL", "TR", "TL2", "TR2", "Select",
		"Start", "Mode", "ThumbL", "ThumbR"
	};
	const char *addNames[6] = {
		"left_down", "left_up", "middle_down", "middle_up", "right_down", "right_up"
	};
	size_t i, echoCode = 0, count = arrlen(names);
	std::string name;
	const char *mem;
	for (i = 0; i < count; i++) {
		name = names[i];
		name.append("Down");
		serial.setEcho(echoCode, name.c_str());
		name.insert(name.end() - 4, ' ');
		mem = name.c_str();
		serial.addEcho(echoCode++, &mem, 1);
		name = names[i];
		name.append("Up");
		serial.setEcho(echoCode, name.c_str());
		name.insert(name.end() - 2, ' ');
		mem = name.c_str();
		serial.addEcho(echoCode++, &mem, 1);
	}
	for (i = arrlen(addNames); i--;) {
		serial.addEcho(i, addNames + i, 1);
	}
}
}
