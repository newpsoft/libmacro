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

Linux-only
* MCR_PLATFORM_LINUX
* MCR_UINPUT_PATH - default "/dev/uinput"
* MCR_EVENT_PATH - default "/dev/input"
* MCR_ABS_RESOLUTION - default 0x0FFF
* MCR_FILE_TRY_COUNT
* MCR_POLL_TIMEOUT
* MCR_GRAB_SET_LENGTH
* MCR_GRAB_SET_SIZE

Windows-only
* MCR_PLATFORM_WINDOWS
* MCR_INTERCEPT_WAIT_MILLIS

Apple-only
* MCR_PLATFORM_APPLE

## Context Members ##
Standard
* mcr_standard_platform
* key_modifier.map + key_modifier.count - Map key to modifier
* modifier_key.map + modifier_key.count - Map modifier to key

Intercept
* mcr_intercept_platform

Linux-only
* mcr_standard_platform echo_events + echo_event_count - Map echo event code to input_event.
* mcr_intercept_platform key_echos + key_echo_count - Map key code to echo event code.

Windows-only

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

Intercept
* mcr_intercept_platform_initialize
* mcr_intercept_platform_deinitialize
* mcr_intercept_enabled
* mcr_intercept_set_enabled
* mcr_intercept_modifiers

Linux-only
* mcr_standard_set_echo_events
* mcr_intercept_set_grabs - All strings are file names that should start with /dev/input/
* mcr_intercept_set_key_echo_map

Windows-only

## Global constants ##
Standard
* mcr_key_modifier_defaults + mcr_key_modifier_default_count
* mcr_modifier_key_defaults + mcr_modifier_key_default_count

Linux-only
* mcr_echo_event_defaults
* mcr_key_echo_map_defaults

Windows-only
