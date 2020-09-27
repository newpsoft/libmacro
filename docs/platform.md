# Platforms #
Some library symbols must be redefined for each platform. Some symbols only exist on one platform.
Not all single-platform symbols will be noted in this document. We will
document here what must be redefined, as well as platform symbols that require
special care or attention.

Currently supported platforms:
* Linux
* Windows

Future planned platforms:
* Ubuntu Touch
* Web service
* Android (system or root app)

## Defines ##
* MCR_PLATFORM - Name of the platform.  Expand to a string with MCR_STR(MCR_PLATFORM)
* MCR_EXPORT, MCR_IMPORT, MCR_API - Library import + export identifiers.
* MCR_INLINE - inline keyword, defined in defines.h, but make sure it is appropriate for all platforms.
* mcr_snprintf - snprintf equivalent
* mcr_casecmp - strcasecmp or stricmp equivalent
* mcr_ncasecmp - strncasecmp or strnicmp equivalent

Linux-only
* MCR_PLATFORM_LINUX
* MCR_UINPUT_PATH - default "/dev/uinput"
* MCR_EVENT_PATH - default "/dev/input"
* MCR_ABS_RESOLUTION - default 0x0FFF
* MCR_FILE_TRY_COUNT
* MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT - Number of echo codes.  This is
twice for each HidEcho key code, pressed and released.
* MCR_POLL_TIMEOUT
* MCR_GRAB_SET_LENGTH
* MCR_GRAB_SET_SIZE
* MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT - Map key + apply to echo code. Press
and release are already separated, so this is half of MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT.

Windows-only
* MCR_PLATFORM_WINDOWS
* MCR_INTERCEPT_WAIT_MILLIS
* MCR_WM_HIDECHO_COUNT - Number of mouse-related windows messages available.
* MCR_WM_HIDECHO_INDEX - Get an index in wm_echos from a mouse-related windows message

Apple-only
* MCR_PLATFORM_APPLE

## Context Members ##
Standard
* mcr_standard_platform
* key_modifiers + key_modifier_count - Map key to modifier
* modifier_keys + modifier_key_count - Map modifier to key

Intercept
* mcr_intercept_platform

Linux-only
* mcr_standard_platform echo_events + echo_event_count - Map echo event code to input_event.
* mcr_intercept_platform key_echos + key_echo_count - Map key code to echo event code.

Windows-only
* mcr_standard_platform echo_flags + echo_flag_count - Map echo event code to mouse event flags.
* mcr_intercept_platform wm_echos - Map WPARAM windows message to echo event code.

## Functions ##
Util
* mcr_privileges_deactivate

Standard
* mcr_standard_platform_initialize
* mcr_standard_platform_deinitialize
* mcr_HidEcho_send_member
* mcr_Key_send_member
* mcr_MoveCursor_send_member
* mcr_Scroll_send_member
* mcr_HidEcho_count
* mcr_cursor_position

Intercept
* mcr_intercept_platform_initialize
* mcr_intercept_platform_deinitialize
* mcr_intercept_enabled
* mcr_intercept_set_enabled
* mcr_intercept_modifiers

Extras
* Libmacro::initialize
* Libmacro::deinitialize
* LibmacroPlatform - Platform-defined class.  Should be exported, but probably
not used directly outside of this Library.

Linux-only
* mcr_standard_set_echo_events
* mcr_intercept_set_grabs - All strings are file names that should start with /dev/input/
* mcr_intercept_set_key_echo_map

Windows-only
* mcr_standard_set_echo_flags
* mcr_intercept_set_wm_echo

## Global constants ##
Standard
* mcr_key_modifier_defaults + mcr_key_modifier_default_count
* mcr_modifier_key_defaults + mcr_modifier_key_default_count

Linux-only
* mcr_standard_echo_event_defaults
* mcr_intercept_key_echo_defaults`

Windows-only
* mcr_standard_echo_flag_defaults + mcr_standard_echo_flag_default_count
* mcr_intercept_wm_echo_defaults
