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

#include "mcr/intercept/linux/p_intercept.h"
#include "mcr/intercept/intercept.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

static int grabber_open(struct mcr_Grabber *grabPt);
static int grabber_close(struct mcr_Grabber *grabPt);
static int grabber_grab(struct mcr_Grabber *grabPt, bool toGrab);

void mcr_Grabber_init(struct mcr_Grabber *grabPt, struct mcr_context *ctx)
{
	dassert(ctx);
	if (grabPt) {
		ZERO_PTR(grabPt);
		grabPt->context = ctx;
		grabPt->fd = -1;
	}
}

void mcr_Grabber_ctor(struct mcr_Grabber *grabPt, struct mcr_context *ctx, bool blocking, const char *path)
{
	dassert(ctx);
	if (grabPt) {
		ZERO_PTR(grabPt);
		grabPt->context = ctx;
		grabPt->fd = -1;
		grabPt->blocking = blocking;
		grabPt->path = path;
	}
}

int mcr_Grabber_deinit(struct mcr_Grabber *grabPt)
{
	mcr_err = 0;
	if (grabPt) {
		mcr_Grabber_set_enabled(grabPt, false);
		grabPt->path = NULL;
	}
	return mcr_err;
}

bool mcr_Grabber_blocking(struct mcr_Grabber *grabPt)
{
	return grabPt && grabPt->blocking;
}

int mcr_Grabber_set_blocking(struct mcr_Grabber *grabPt, bool enable)
{
	if (!grabPt || enable == grabPt->blocking)
		return 0;
	/* Currently running */
	if (grabPt->fd != -1) {
		if (grabber_grab(grabPt, enable))
			return mcr_err;
	}
	grabPt->blocking = enable;
	return 0;
}

const char *mcr_Grabber_path(struct mcr_Grabber *grabPt)
{
	return grabPt ? grabPt->path : NULL;
}

int mcr_Grabber_set_path(struct mcr_Grabber *grabPt, const char *path)
{
	bool wasEnabled = mcr_Grabber_enabled(grabPt);
	dassert(grabPt);
	if (wasEnabled) {
		if (mcr_Grabber_set_enabled(grabPt, false))
			return mcr_err;
	}
	grabPt->path = path;
	if (wasEnabled) {
		if (mcr_Grabber_set_enabled(grabPt, true))
			return mcr_err;
	}
	return 0;
}

bool mcr_Grabber_enabled(struct mcr_Grabber * grabPt)
{
	return grabPt ? grabPt->fd != -1 : false;
}

int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable)
{
	dassert(grabPt);
	if (enable != mcr_Grabber_enabled(grabPt))
		return enable ? grabber_open(grabPt) : grabber_close(grabPt);
	return 0;
}

static int grabber_open(struct mcr_Grabber *grabPt)
{
	mcr_err = 0;
	if (grabPt->fd != -1 || !grabPt->path || !grabPt->path[0])
		error_set_return(ENODEV);
	if (access(grabPt->path, R_OK)) {
		mcr_errno(ENODEV);
		return mcr_err;
	}
	if ((grabPt->fd = open(grabPt->path, O_RDWR | O_NONBLOCK)) == -1) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	if (grabPt->blocking)
		return grabber_grab(grabPt, true);
	return 0;
}

static int grabber_close(struct mcr_Grabber *grabPt)
{
	mcr_err = 0;
	if (grabPt->fd == -1)
		return 0;
	if (grabPt->blocking && grabber_grab(grabPt, false)) {
		close(grabPt->fd);
		grabPt->fd = -1;
		return mcr_err;
	}
	if (close(grabPt->fd) < 0) {
		mcr_errno(EINTR);
	}
	grabPt->fd = -1;
	return mcr_err;
}

static int grabber_grab(struct mcr_Grabber *grabPt, bool toGrab)
{
	if (ioctl(grabPt->fd, EVIOCGRAB, toGrab ? 1 : 0) < 0)
		error_set_return(EINTR);
	return 0;
}
