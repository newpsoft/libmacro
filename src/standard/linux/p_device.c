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

#include "mcr/standard/linux/p_standard.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "mcr/libmacro.h"

const MCR_API struct input_event mcr_syncer = {
	.type = EV_SYN,
	.code = SYN_REPORT
};

/* Default device event bits */
static const int genUiEvents[] = {
	EV_KEY, EV_REL, EV_MSC, EV_SND, EV_REP
};
/* Do not write key 0 value */
static const mcr_EvbitRange_t keyRange = { UI_SET_KEYBIT, 1, KEY_MAX };
static const mcr_EvbitRange_t relRange = { UI_SET_RELBIT, 0, REL_MAX };
static const mcr_EvbitRange_t mscRange = { UI_SET_MSCBIT, 0, MSC_MAX };
static const mcr_EvbitRange_t sndRange = { UI_SET_SNDBIT, 0, SND_MAX };
/* Pointer and direct properties are actually just 0 and 1 */
static const mcr_EvbitRange_t pointerRange = { UI_SET_PROPBIT, INPUT_PROP_POINTER, INPUT_PROP_POINTER };
/* All of the above = 5 */
static mcr_EvbitRange_t genDevRanges[5];

/* Need at least one button to make a pointer device. */
static const int absUiEvents[] = {
	EV_KEY, EV_ABS
};
/* Not used, but required to be set up correctly */
static const mcr_EvbitRange_t absKeyRange = { UI_SET_KEYBIT, BTN_LEFT, BTN_LEFT };
/* ABS_X = 0 */
static const mcr_EvbitRange_t absRange = { UI_SET_ABSBIT, ABS_X, ABS_MISC };
/* + pointer */
static const mcr_EvbitRange_t directRange = { UI_SET_PROPBIT, INPUT_PROP_DIRECT, INPUT_PROP_DIRECT };
/* All of the above = 4 */
static mcr_EvbitRange_t absDevRanges[4];

/* Open uinput ( devPt->fd ) and write device to it. */
static int device_open(struct mcr_Device *devPt);
/* Open event device ( devPt->eventFd ). */
static int device_open_event(struct mcr_Device *devPt);
/* Destroy uinput device ( devPt->fd ) and close devPt->fd. */
static int device_close(struct mcr_Device *devPt);
/* Close devPt->eventFd. */
static int device_close_event(struct mcr_Device *devPt);
/* Write all UI_SET_EVBIT values for given device. */
static int device_write_events(struct mcr_Device *devPt);
/* Write all non-UI_SET_EVBIT values for given device. */
static int device_write_evbits(struct mcr_Device *devPt);
/* Write UI_DEV_CREATE after writing all evbits to device. */
static int device_create(struct mcr_Device *devPt);

static void genDevice_init(struct mcr_context *ctx);
static void absDevice_init(struct mcr_context *ctx);

int mcr_Device_set_uinput_path(struct mcr_context *ctx, const char *path)
{
	if (!path || !path[0])
		error_set_return(ENODEV);
	ctx->standard.platform_pt->uinput_path = path;
	return (mcr_err = 0);
}

int mcr_Device_set_event_path(struct mcr_context *ctx, const char *directoryPath)
{
	if (!directoryPath || !directoryPath[0])
		error_set_return(ENODEV);
	ctx->standard.platform_pt->event_path = directoryPath;
	return (mcr_err = 0);
}

int mcr_Device_set_absolute_resolution(struct mcr_context *ctx, __s32 resolution)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	standardPt->abs_resolution = resolution;
	if (standardPt->abs_dev.enabled) {
		if (mcr_Device_enable(&standardPt->abs_dev, false))
			return mcr_err;
		return mcr_Device_enable(&standardPt->abs_dev, true);
	}
	return 0;
}

void mcr_Device_ctor(struct mcr_Device *devPt, struct mcr_context *ctx, const char *name, const int *setEvents, size_t setEventCount, const mcr_EvbitRange_t *eventBits, size_t eventBitCount)
{
	if (devPt) {
		ZERO_PTR(devPt);
		devPt->context = ctx;
		devPt->fd = -1;
		devPt->event_fd = -1;
		if (name)
			snprintf(devPt->device.name, UINPUT_MAX_NAME_SIZE, name);
		devPt->set_events = setEvents;
		devPt->set_event_count = setEventCount;
		devPt->event_bits = eventBits;
		devPt->event_bit_count = eventBitCount;
	}
}

void mcr_Device_deinit(struct mcr_Device *devPt)
{
	if (devPt)
		mcr_Device_enable(devPt, false);
}

static int mtx_unlock_error(struct mcr_context *ctx, int mtxError)
{
	if (mtxError == thrd_success)
		mtx_unlock(&ctx->standard.platform_pt->device_lock);
	return mcr_err;
}

int mcr_Device_enable(struct mcr_Device *devPt, bool enable)
{
	struct mcr_standard_platform *standardPt = devPt->context->standard.platform_pt;
	int mtxErr = thrd_success;
	dassert(devPt);
	mcr_err = 0;
	devPt->enabled = false;
	if (device_close_event(devPt))
		return mcr_err;
	/* If already enabled, need to close and restart. */
	if (device_close(devPt))
		return mcr_err;
	/* disable complete */
	if (!enable)
		return 0;

	/* Cannot have device with no evbits to set.
	 * Must have at least 1 UI_SET_EVBIT. */
	if (!mcr_Device_has_evbit(devPt))
		error_set_return(EINVAL);
	mtxErr = mtx_lock(&standardPt->device_lock);
	/* Start by opening. */
	if (device_open(devPt))
		return mtx_unlock_error(devPt->context, mtxErr);
	/* Force evbit satisfaction. */
	if (device_write_events(devPt))
		return mtx_unlock_error(devPt->context, mtxErr);
	/* Then write all other bits. Ensured at least one mapping pair. */
	if (device_write_evbits(devPt))
		return mtx_unlock_error(devPt->context, mtxErr);
	if (device_create(devPt))
		return mtx_unlock_error(devPt->context, mtxErr);

	devPt->enabled = true;
	device_open_event(devPt);
	return mtx_unlock_error(devPt->context, mtxErr);
}

int mcr_Device_enable_all(struct mcr_context *ctx, bool enable)
{
	if (mcr_Device_enable(&ctx->standard.platform_pt->gen_dev, enable))
		return mcr_err;
	return mcr_Device_enable(&ctx->standard.platform_pt->abs_dev, enable);
}

void mcr_Device_set_uibit(struct mcr_Device *devPt, int *uiEvents, size_t count)
{
	devPt->set_events = uiEvents;
	devPt->set_event_count = count;
}

void mcr_Device_set_evbits(struct mcr_Device *devPt, mcr_EvbitRange_t *evBits, size_t count)
{
	devPt->event_bits = evBits;
	devPt->event_bit_count = count;
}

mcr_EvbitRange_t *mcr_Device_bits(struct mcr_Device *devPt, int setUiBit)
{
	return bsearch(&setUiBit, devPt->event_bits, devPt->event_bit_count, sizeof(mcr_EvbitRange_t), mcr_int_compare);
}

bool mcr_Device_has_evbit(struct mcr_Device * devPt)
{
	return devPt->set_events && devPt->set_event_count;
}

static int device_open(struct mcr_Device *devPt)
{
	dassert(devPt);
	const char *path = devPt->context->standard.platform_pt->uinput_path;
	if (!path || !path[0])
		error_set_return(ENODEV);
	mcr_err = 0;
	/* Close in case previously opened. */
	if (device_close(devPt))
		return mcr_err;
	devPt->fd = open(path, O_WRONLY | O_SYNC);
	if (devPt->fd == -1) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	//! \todo
//	int version, rc;
//	rc = ioctl(devPt->fd, UI_GET_VERSION, &version);
	return 0;
}

static bool str_ends_with_separator(const char *str)
{
	const char *iter;
	if (!str || !*str)
		return false;
	/* Move iter to null-term */
	for (iter = str; *iter; ++iter) {}
	/* One before null-term is path separator */
	return *(--iter) == '/';
}

static int device_open_event(struct mcr_Device *devPt)
{
	dassert(devPt);
	static const char sysNameBase[] = "/sys/devices/virtual/input/";
	struct mcr_standard_platform *standardPt = devPt->context->standard.platform_pt;
	const char *path = standardPt->event_path;
	DIR *dirp;
	struct dirent *entry;
	int timeout = MCR_FILE_TRY_COUNT;
	// Should be in /dev/input and not something longer
	char buffer[PATH_MAX];

	if (!path || !path[0])
		error_set_return(ENODEV);
	mcr_err = 0;

	/* sizeof string includes null-termination */
	snprintf(buffer, sizeof(buffer) - 1, "%s", sysNameBase);
	// strlen sysNameBase == sizeof - 1
	if (ioctl(devPt->fd, UI_GET_SYSNAME(sizeof(buffer) - sizeof(sysNameBase)), buffer + sizeof(sysNameBase) - 1) < 0) {
		mcr_errno(EINTR);
		return mcr_err;
	}

	dirp = opendir(buffer);
	if (dirp == NULL) {
		mcr_errno(EINTR);
		return mcr_err;
	}

	if (device_close_event(devPt))
		return mcr_err;

	/* if !entry then end of directory */
	while ((entry = readdir(dirp))) {
		/* Ignore files without access, and do not fail with lack
		 * of permissions */
		if (!strncmp(entry->d_name, "event", 5)) {
			/* May be either "/dev/input" "/" "event#" or "/dev/input/" "" "event#" */
			snprintf(buffer, sizeof(buffer) - 1, "%s%s%s", path, str_ends_with_separator(path) ? "" : "/", entry->d_name);
			/* Device was just written, may not be ready */
			do {
				devPt->event_fd = open(buffer, O_RDONLY);
				/* Avoid sleep on success */
				if (devPt->event_fd != -1)
					break;
				usleep(382000);
			} while (timeout--);
			if (devPt->event_fd == -1) {
				/* Most likely permissions error */
				mcr_errno(EPERM);
			}
			/* Currently only looking for one event */
			break;
		}
	}
	closedir(dirp);
	/* If error then it did not open, no need to close. */
	return mcr_err;
}

static int device_close(struct mcr_Device *devPt)
{
	dassert(devPt);
	if (devPt->fd != -1) {
		if (ioctl(devPt->fd, UI_DEV_DESTROY) < 0 || close(devPt->fd) < 0) {
			mcr_errno(EINTR);
			return mcr_err;
		} else {
			devPt->fd = -1;
		}
	}
	return 0;
}

static int device_close_event(struct mcr_Device *devPt)
{
	dassert(devPt);
	if (devPt->event_fd != -1) {
		if (close(devPt->event_fd) < 0) {
			mcr_errno(EINTR);
			return mcr_err;
		} else {
			devPt->event_fd = -1;
		}
	}
	return 0;
}

static int device_write_events(struct mcr_Device *devPt)
{
	dassert(devPt);
	int fd = devPt->fd;
	/* All devices sync */
	if (ioctl(fd, UI_SET_EVBIT, EV_SYN))
		error_set_return(EINTR);
	/* Write UI bits in order */
	for (size_t i = 0; i < devPt->set_event_count; i++) {
		if (ioctl(fd, UI_SET_EVBIT, devPt->set_events[i]) < 0)
			error_set_return(EINTR);
	}
	return 0;
}

static int device_write_evbits(struct mcr_Device *devPt)
{
	dassert(devPt);
	int fd = devPt->fd, evbit, setEvbit;
	const mcr_EvbitRange_t *iter, *end;
	if (!devPt->event_bit_count)
		error_set_return(EINVAL);
	mcr_err = 0;
	for (iter = devPt->event_bits, end = devPt->event_bits + devPt->event_bit_count; iter < end; iter++) {
		setEvbit = iter->set_ev_bit;
		for (evbit = iter->first; evbit <= iter->last; evbit++) {
			if (ioctl(fd, setEvbit, evbit) < 0) {
				mcr_errno(EINTR);
				break;
			}
		}
	}
	return mcr_err;
}

static int device_create(struct mcr_Device *devPt)
{
	dassert(devPt);
	if (write(devPt->fd, &devPt->device, sizeof(devPt->device)) != sizeof(devPt->device)) {
		device_close(devPt);
		mcr_errno(EINTR);
		return mcr_err;
	}
	//! \todo
//	if (ioctl(devPt->fd, UI_DEV_SETUP, &devPt->device)) {
//		device_close(devPt);
//		mcr_errno(EINTR);
//		return mcr_err;
//	}
	if (ioctl(devPt->fd, UI_DEV_CREATE) < 0) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	return 0;
}

static void genDevice_init(struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;

	mcr_Device_ctor(&standardPt->gen_dev, ctx, "libmacro-gen", genUiEvents,	arrlen(genUiEvents), genDevRanges, arrlen(genDevRanges));

	standardPt->gen_dev.device.id.bustype = BUS_VIRTUAL;
	standardPt->gen_dev.device.id.vendor = 1;
	standardPt->gen_dev.device.id.product = 1;
	standardPt->gen_dev.device.id.version = 1;
}

static void absDevice_init(struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	int i;
	mcr_Device_ctor(&standardPt->abs_dev, ctx, "libmacro-abs", absUiEvents, arrlen(absUiEvents), absDevRanges, arrlen(absDevRanges));

	standardPt->abs_dev.device.id.bustype = BUS_VIRTUAL;
	standardPt->abs_dev.device.id.vendor = 1;
	standardPt->abs_dev.device.id.product = 1;
	standardPt->abs_dev.device.id.version = 1;

	for (i = 0; i <= ABS_MISC; i++) {
		standardPt->abs_dev.device.absmax[i] = standardPt->abs_resolution;
	}
}

int mcr_Device_initialize(struct mcr_context *ctx)
{
	int err;

	/* gen dev initial bits */
	genDevRanges[0] = keyRange;
	genDevRanges[1] = relRange;
	genDevRanges[2] = mscRange;
	genDevRanges[3] = sndRange;
	genDevRanges[4] = pointerRange;
	/* abs dev initial bits */
	absDevRanges[0] = absKeyRange;
	absDevRanges[1] = absRange;
	absDevRanges[2] = pointerRange;
	absDevRanges[3] = directRange;

	/* ioctl is unpredictable for valgrind. The first ioctl will
	 * be uninitialized, and others should not cause errors. */
	genDevice_init(ctx);
	absDevice_init(ctx);
	if (!access(MCR_UINPUT_PATH, W_OK | R_OK)) {
		if (mcr_Device_enable_all(ctx, true)) {
			err = mcr_err;
			mcr_Device_deinitialize(ctx);
			error_set_return(err);
		}
	}
	return 0;
}

int mcr_Device_deinitialize(struct mcr_context *ctx)
{
	struct mcr_standard_platform *standardPt = ctx->standard.platform_pt;
	int mtxErr = mtx_lock(&standardPt->device_lock);
	mcr_Device_deinit(&standardPt->gen_dev);
	mcr_Device_deinit(&standardPt->abs_dev);
	if (mtxErr == thrd_success)
		mtx_unlock(&standardPt->device_lock);
	return 0;
}
