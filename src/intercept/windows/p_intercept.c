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

#include "mcr/intercept/windows/p_intercept.h"
#include "mcr/intercept/intercept.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* IsUserAnAdmin */
#if _WIN32_WINNT < 0x0600
	#include <shlobj.h>
#endif

#include "mcr/libmacro.h"

/*! Print windows error, \ref mcr_errno, and set err to mcr_err
 *
 *  Do not change original error of mcr_err */
#define winerr(fallbackError) { \
	mcr_ddo(fprintf(stderr, "Windows Error: %ld\n", GetLastError())); \
	mcr_errno(fallbackError); \
}

static size_t mcr_intercept_wm_echos_impl[MCR_WM_HIDECHO_COUNT];
MCR_API const size_t *const mcr_intercept_wm_echo_defaults =
	mcr_intercept_wm_echos_impl;

static MCR_INLINE enum mcr_ApplyType wparam_apply(WPARAM wParam)
{
	return wParam == WM_KEYUP || wParam == WM_SYSKEYUP ? MCR_UNSET : MCR_SET;
}

// Cannot give context to a hook proc, so must be static.
static struct mcr_context *_hook_context = NULL;
// Optimization, since hook context must be static
static HHOOK _hook_key_id;
static HHOOK _hook_mouse_id;

/* Dispatch signals */
struct mcr_Signal _echoSig = {{0}, true};
struct mcr_Signal _keySig = {{0}, true};
struct mcr_Signal _mcSig = {{0}, true};
struct mcr_Signal _scrollSig = {{0}, true};
struct mcr_HidEcho _echo = {0};
struct mcr_Key _key = {0};
struct mcr_MoveCursor _mc = {{0}, true};
struct mcr_Scroll _scroll = {0};

extern LONG _lastX;
LONG _lastX = 0;
extern LONG _lastY;
LONG _lastY = 0;

static int verify_key_state(PBYTE keyState, size_t keyState_size);
static LRESULT CALLBACK key_proc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK mouse_proc(int nCode, WPARAM wParam, LPARAM lParam);
static bool has_privileges(void);

static int apply_context(struct mcr_context *ctx)
{
	if (_hook_context && _hook_context != ctx)
		error_set_return(EBUSY);
	_echoSig.isignal = mcr_iHidEcho(ctx);
	_echoSig.dispatch_flag = true;
	_keySig.isignal = mcr_iKey(ctx);
	_keySig.dispatch_flag = true;
	_mcSig.isignal = mcr_iMoveCursor(ctx);
	_mcSig.dispatch_flag = true;
	_scrollSig.isignal = mcr_iScroll(ctx);
	_scrollSig.dispatch_flag = true;
	_hook_context = ctx;
	return 0;
}

// \todo Does every application have a message loop?
/* Common intercept thread */
//static DWORD _threadId = 0;
//static DWORD WINAPI intercept_thread(LPVOID lpThreadParameter)
//{
//	UNUSED(lpThreadParameter);
//	MSG msg = { 0 };

//	PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
//	while (GetMessage(&msg, 0, 0, 0)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	return 0;
//}

bool mcr_intercept_enabled(struct mcr_context *ctx)
{
	UNUSED(ctx);
	return _hook_key_id || _hook_mouse_id;
//	if (mcr_Grabber_enabled(&nPt->grab_key))
//		return true;
//	if (mcr_Grabber_enabled(&nPt->grab_mouse))
//		return true;
//	return false;
}

int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable)
{
//	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	if (mcr_intercept_key_set_enabled(ctx, enable))
		return mcr_err;
	return mcr_intercept_mouse_set_enabled(ctx, enable);
//	if (enable && apply_context(ctx))
//		return mcr_err;
//	if (mcr_Grabber_set_enabled(&nPt->grab_key, enable))
//		return mcr_err;
//	if (mcr_Grabber_set_enabled(&nPt->grab_mouse, enable))
//		return mcr_err;
//	return 0;
}

unsigned int mcr_intercept_modifiers(struct mcr_context *ctx)
{
	unsigned int values = 0;
	BYTE keyState[0x100];
	int key;
	unsigned int modifier = 1;
	/* If cannot get the mod state, return current */
	if (verify_key_state(keyState, 0xFF))
		return *mcr_modifiers(ctx);

	/* Cycle all flags */
	while (modifier) {
		key = mcr_modifier_key(ctx, modifier);
		/* high bit is down, lowest byte is toggled. */
		/* high bit of byte is 0x80, high bit of short is 0x8000 */
		if (key && (keyState[key] & 0x8080) != 0)
			values |= modifier;
		modifier <<= 1;
	}
	return values;
}

bool mcr_intercept_key_enabled(struct mcr_context * ctx)
{
	UNUSED(ctx);
	return _hook_key_id;
//	return mcr_Grabber_enabled(&nPt->grab_key);
}

int mcr_intercept_key_set_enabled(struct mcr_context *ctx, bool enable)
{
	mcr_err = 0;
	if (enable) {
		if (!has_privileges())
			error_set_return(EPERM);
		if (apply_context(ctx))
			return mcr_err;
		if (!_hook_key_id)
			_hook_key_id = SetWindowsHookEx(WH_KEYBOARD_LL, key_proc, GetModuleHandle(NULL),
											0);
		if (!_hook_key_id)
			winerr(EINTR);
	} else {
		if (_hook_key_id) {
			if (!UnhookWindowsHookEx(_hook_key_id))
				winerr(EINTR);
			_hook_key_id = NULL;
		}
		if (!_hook_mouse_id)
			_hook_context = NULL;
	}
//	if (mcr_Grabber_set_enabled(&nPt->grab_key, enable))
//		return mcr_err;
	return mcr_err;
}

bool mcr_intercept_mouse_enabled(struct mcr_context *ctx)
{
	UNUSED(ctx);
	return _hook_mouse_id;
//	return mcr_Grabber_enabled(&nPt->grab_mouse);
}

int mcr_intercept_mouse_set_enabled(struct mcr_context *ctx, bool enable)
{
	mcr_err = 0;
	if (enable) {
		if (!has_privileges())
			error_set_return(EPERM);
		if (apply_context(ctx))
			return mcr_err;
		if (!_hook_mouse_id)
			_hook_mouse_id = SetWindowsHookEx(WH_MOUSE_LL, mouse_proc,
											  GetModuleHandle(NULL), 0);
		if (!_hook_mouse_id)
			winerr(EINTR);
	} else {
		if (_hook_mouse_id) {
			if (!UnhookWindowsHookEx(_hook_mouse_id))
				winerr(EINTR);
			_hook_mouse_id = NULL;
		}
		if (!_hook_key_id)
			_hook_context = NULL;
	}
//	if (enable && apply_context(ctx))
//		return mcr_err;
//	if (mcr_Grabber_set_enabled(&nPt->grab_mouse, enable))
//		return mcr_err;
	return mcr_err;
}

int mcr_intercept_set_wm_echo(struct mcr_context *ctx, WPARAM wm,
							  size_t echoCode)
{
	if (wm < WM_MOUSEFIRST || wm > WM_MOUSELAST)
		error_set_return(EINVAL);
	ctx->intercept.platform_pt->wm_echos[MCR_WM_HIDECHO_INDEX(wm)] = echoCode;
	return (mcr_err = 0);
}

static LRESULT __stdcall key_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;

	if (nCode == HC_ACTION && p->dwExtraInfo != MCR_WM_IGNORE) {
		_key.key = (int)p->vkCode;
		_key.apply = wparam_apply(wParam);
		if (mcr_dispatch(_hook_context, &_keySig) && _hook_context->intercept.blockable)
			return -1;
	}
	return CallNextHookEx(_hook_key_id, nCode, wParam, lParam);
}

static LRESULT __stdcall mouse_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	struct mcr_intercept_platform *nPt =
			_hook_context->intercept.platform_pt;
	bool blockable = _hook_context->intercept.blockable;
	MSLLHOOKSTRUCT *p = (MSLLHOOKSTRUCT *) lParam;
//	POINT current;
	if (nCode == HC_ACTION && p->dwExtraInfo != MCR_WM_IGNORE) {
		switch (wParam) {
		case WM_MOUSEMOVE:
			// \todo Multiple screens may need GetCursorPos
//			if (GetCursorPos(&current)) {
//				fprintf(stderr, "WM_MOUSEMOVE justify? %s: %ld:%ld full? %ld:%ld\n",
//						_mc.justify_flag ? "true" : "false", p->pt.x, p->pt.y, current.x, current.y);
//			}
			_mc.justify_flag = !(p->flags & MOUSE_MOVE_ABSOLUTE);
			if (_mc.justify_flag) {
				_mc.position[MCR_X] = p->pt.x - _lastX;
				_mc.position[MCR_Y] = p->pt.y - _lastY;
			} else {
				_mc.position[MCR_X] = p->pt.x;
				_mc.position[MCR_Y] = p->pt.y;
			}
			_lastX = p->pt.x;
			_lastY = p->pt.y;
			if (mcr_dispatch(_hook_context, &_mcSig) && blockable)
				return -1;
			break;
		case WM_MOUSEWHEEL:
			_scroll.dimensions[MCR_X] = 0;
			_scroll.dimensions[MCR_Y] = GET_WHEEL_DELTA_WPARAM(p->mouseData);
			if (mcr_dispatch(_hook_context, &_scrollSig) && blockable)
				return -1;
			break;
		case WM_MOUSEHWHEEL:
			_scroll.dimensions[MCR_X] = GET_WHEEL_DELTA_WPARAM(p->mouseData);
			_scroll.dimensions[MCR_Y] = 0;
			if (mcr_dispatch(_hook_context, &_scrollSig) && blockable)
				return -1;
			break;
		default:
			// \todo GET_XBUTTON_WPARAM to specify X1 or X2 buttons
			if (wParam >= WM_MOUSEFIRST && wParam <= WM_MOUSELAST) {
				if ((_echo.echo = nPt->wm_echos[MCR_WM_HIDECHO_INDEX(wParam)])
					!= MCR_HIDECHO_ANY) {
					if (mcr_dispatch(_hook_context, &_echoSig) && blockable)
						return -1;
				}
			}
			break;
		}
	}
	return CallNextHookEx(_hook_mouse_id, nCode, wParam, lParam);
}

static int verify_key_state(PBYTE keyState, size_t keyState_size)
{
	/* Only get the keyboard state if not done already. The only way to */
	/* check for this is if keyState is in the right format. */
	if (keyState == NULL)
		error_set_return(EFAULT);
	if (keyState_size < 0xFF)
		error_set_return(EINVAL);
	/* Required: Sample key state so windows refreshes keyboard state */
	UNUSED(GetKeyState(VK_RETURN));

	/* Get pressed keys, error if returned false */
	if (!GetKeyboardState(keyState)) {
		mcr_dprint("Windows Error: %ld\n", GetLastError());
		error_set_return(EINTR);
	}
	return 0;
}

static int free_return(struct mcr_intercept_platform
					   *nPt)//a, bool grabKeyFlag, bool grabMouseFlag)
{
	int error = mcr_err;
//	if (grabKeyFlag)
//		mcr_Grabber_deinit(&nPt->grab_key);
//	if (grabMouseFlag)
//		mcr_Grabber_deinit(&nPt->grab_mouse);
	ZERO_PTR(nPt);
	free(nPt);
	if (!mcr_err)
		mcr_err = error;
	return mcr_err;
}

static void initialize_wm_echos()
{
	/* Known order: left, middle, down. down then up */
	const WPARAM knownOrder[] = {
		WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MBUTTONDOWN,
		WM_MBUTTONUP, WM_RBUTTONDOWN, WM_RBUTTONUP,
		WM_XBUTTONDOWN, WM_XBUTTONUP
	};
	size_t i = MCR_WM_HIDECHO_COUNT;
	while (i--) {
		mcr_intercept_wm_echos_impl[i] = (size_t)~0;
	}
	for (i = arrlen(knownOrder); i--;) {
		mcr_intercept_wm_echos_impl[MCR_WM_HIDECHO_INDEX(knownOrder[i])] = i;
	}
}

int mcr_intercept_platform_initialize(struct mcr_context *ctx)
{
	// not thread safe
	initialize_wm_echos();
	// not thread safe \todo does every application have a message loop
//	if (!_threadId) {
//		HANDLE h = CreateThread(NULL, 0, intercept_thread, NULL, 0, &_threadId);
//		if (!h) {
//			fprintf(stderr, "Windows Error: %ld\n", GetLastError());
//			error_set_return(EINTR);
//		}
//		PostThreadMessage(_threadId, WM_KEYDOWN, 0, 0);
//		CloseHandle(h);
//	}

	struct mcr_intercept_platform *nPt =
		malloc(sizeof(struct mcr_intercept_platform));
	if (!nPt)
		error_set_return(ENOMEM);
	ZERO_PTR(nPt);
	for (size_t i = 0; i < MCR_WM_HIDECHO_COUNT; i++) {
		nPt->wm_echos[i] = MCR_HIDECHO_ANY;
	}

	_echoSig.dispatch_flag = true;
	_echoSig.instance.data_member.data = &_echo;
	_keySig.dispatch_flag = true;
	_keySig.instance.data_member.data = &_key;
	_mcSig.dispatch_flag = true;
	_mcSig.instance.data_member.data = &_mc;
	_scrollSig.dispatch_flag = true;
	_scrollSig.instance.data_member.data = &_scroll;

	// platform is always heap, so threaded grabber is ok.
//	if (mcr_Grabber_ctor(&nPt->grab_key, ctx, WH_KEYBOARD_LL, key_proc))
//		return free_return(nPt, false, false);
//	if (mcr_Grabber_ctor(&nPt->grab_mouse, ctx, WH_MOUSE_LL, mouse_proc))
//		return free_return(nPt, true, false);
	ctx->intercept.platform_pt = nPt;
	return 0;
}

int mcr_intercept_platform_deinitialize(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	if (nPt) {
		mcr_err = 0;
		mcr_intercept_set_enabled(ctx, false);
		_hook_context = NULL;
		Sleep(500);
		ctx->intercept.platform_pt = NULL;
		return free_return(nPt);//, true, true);
	}
	_hook_context = NULL;
	return 0;
}

static bool has_privileges()
{
#if _WIN32_WINNT >= 0x0600
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD uiAccess = 0;
		DWORD cbSize = sizeof(DWORD);
		/* Intended use, uiAccess */
		if(GetTokenInformation(hToken, TokenUIAccess, &uiAccess, sizeof(DWORD),
							   &cbSize)) {
			fRet = !!uiAccess;
		}
		/* Another possibility is Admin */
		if(!fRet && GetTokenInformation(hToken, TokenElevation, &Elevation,
										sizeof(Elevation),
										&cbSize)) {
			fRet = !!Elevation.TokenIsElevated;
		}
	}
	if(hToken)
		CloseHandle(hToken);
	return fRet;
#else
	return !!IsUserAnAdmin();
#endif
}
