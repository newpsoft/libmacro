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

// \todo Do not use mutex locks, just disable from separate thread.

#include "mcr/libmacro.h"
#include "mcr/intercept/linux/p_intercept.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct mcr_MapElement key_echos_impl[2][MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT];

MCR_API const struct mcr_MapElement *const mcr_intercept_key_echo_defaults[2] = {
	key_echos_impl[0],
	key_echos_impl[1]
};

/* Local, inline, and optimized read parameters of mcr_Grabber */
typedef struct {
	/* May-dispatch signals */
	struct mcr_Key key;
	struct mcr_HidEcho echo;
	struct mcr_MoveCursor abs, rel;
	struct mcr_Scroll scr;
	struct mcr_Signal keysig, echosig, abssig, relsig, scrsig;

	/* Read variables */
	struct input_event events[MCR_GRAB_SET_LENGTH];
	int rdb;
	struct pollfd fd;

	/* Event handler variables */
	struct mcr_MapElement *keyEchoFound;
	int pos;
	bool bAbs[MCR_DIMENSION_COUNT];
} GrabReader;

static bool has_enabled_locked(struct mcr_context *ctx);
static int set_enabled_locked(struct mcr_context *ctx, bool enable);
static unsigned int get_mods_locked(struct mcr_context *ctx);
/* Disable in separate thread to avoid trying to remove currently running grabber. */
static int thread_disable(void *threadArgs);
/* Disable, clear, re-enable */
static int thread_reset(void *threadArgs);
static int intercept_start(void *threadArgs);
static int read_grabber_blocking(struct mcr_Grabber *grabPt);
static int read_grabber_passing(struct mcr_Grabber *grabPt);
static unsigned int modify_eventbits(struct mcr_context *ctx, char *keybit_values);
static unsigned int max_modifier_val(void);
static int apply_grab_paths_locked(struct mcr_context *ctx);
static int clear_grabbers_locked(struct mcr_context *ctx);

static inline int to_position(int evPos)
{
	switch (evPos) {
	case ABS_X:
	case REL_HWHEEL:
		return MCR_X;
	case ABS_Y:
	case REL_WHEEL:
		return MCR_Y;
	case ABS_Z:
	case REL_DIAL:
		return MCR_Z;
	}
	/* Actually the same as ABS, but this is insurance. */
	switch (evPos) {
	case REL_X:
		return MCR_X;
	case REL_Y:
		return MCR_Y;
	case REL_Z:
		return MCR_Z;
	}
	return MCR_DIMENSION_COUNT;
}

static inline void bpos_zero(bool arr[MCR_DIMENSION_COUNT])
{
	/* Currently only using xyz */
	arr[0] = false;
	arr[1] = false;
	arr[2] = false;
}

static inline void abs_set_current(struct mcr_context *ctx, struct mcr_MoveCursor *abs,
								   bool hasPosArray[])
{
	if (hasPosArray[MCR_X]) {
		ctx->standard.platform_pt->cursor[MCR_X] = abs->position[MCR_X];
	} else {
		abs->position[MCR_X] = ctx->standard.platform_pt->cursor[MCR_X];
	}
	if (hasPosArray[MCR_Y]) {
		ctx->standard.platform_pt->cursor[MCR_Y] = abs->position[MCR_Y];
	} else {
		abs->position[MCR_Y] = ctx->standard.platform_pt->cursor[MCR_Y];
	}
	if (hasPosArray[MCR_Z]) {
		ctx->standard.platform_pt->cursor[MCR_Z] = abs->position[MCR_Z];
	} else {
		abs->position[MCR_Z] = ctx->standard.platform_pt->cursor[MCR_Z];
	}
}

static void GrabReader_ctor(GrabReader *readerPt, struct mcr_context *ctx)
{
	dassert(readerPt);
	dassert(ctx);
	struct mcr_Signal *signals[] = { &readerPt->keysig, &readerPt->echosig, &readerPt->abssig, &readerPt->relsig, &readerPt->scrsig };
	struct mcr_ISignal *isignals[] = { mcr_iKey(ctx), mcr_iHidEcho(ctx), mcr_iMoveCursor(ctx), mcr_iMoveCursor(ctx), mcr_iScroll(ctx) };
	void *dataSet[] = { &readerPt->key, &readerPt->echo, &readerPt->abs, &readerPt->rel, &readerPt->scr };
	int i;
	ZERO_PTR(readerPt);
	for (i = arrlen(dataSet); i--;) {
		mcr_Signal_init(signals[i]);
		signals[i]->isignal = isignals[i];
		signals[i]->instance.data_member.data = dataSet[i];
		signals[i]->dispatch_flag = true;
	}
	readerPt->rel.justify_flag = true;
	readerPt->fd.events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND;
	/* Set fd.fd in read loop */
}

static inline bool GrabReader_read(GrabReader *readerPt, struct mcr_Grabber *grabPt)
{
	/* Disable point */
	while ((readerPt->fd.fd = grabPt->fd) != -1) {
		/* Disable point */
		/* Error or no data within timeout, try again. */
		if (poll(&readerPt->fd, 1, MCR_POLL_TIMEOUT) <= 0)
			continue;
		// Should lock here?
		readerPt->rdb = read(readerPt->fd.fd, readerPt->events, sizeof(readerPt->events));
		if (readerPt->rdb < (int) sizeof(struct input_event)) {
			if (!mcr_Grabber_enabled(grabPt))
				return false;
			mcr_errno(0);
			// Interrupted read is probably disabling grabber.
			if (mcr_err == EINTR)
				mcr_err = 0;
			/* Return false to end execution. */
			return false;
		}
		/* Have read events, return true */
		return true;
	}
	/* Disabled, return false */
	return false;
}

static inline struct input_event *GrabReader_end(GrabReader *readerPt)
{
	return (struct input_event *)(((char *)readerPt->events) + readerPt->rdb);
}

static inline bool GrabReader_handle_key(GrabReader *readerPt, struct mcr_context *ctx, struct mcr_intercept_platform *nInterceptPt, struct input_event *curVent)
{
	/* Key is one event, so save trouble by dispatching immediately. */
	readerPt->key.key = curVent->code;
	readerPt->key.apply = curVent->value ? MCR_SET : MCR_UNSET;
	/* Always dispatch every key. */
	if (mcr_dispatch(ctx, &readerPt->keysig))
		return true;
	if (readerPt->key.key) {
		readerPt->keyEchoFound = bsearch(&readerPt->key.key, nInterceptPt->key_echos[readerPt->key.apply], nInterceptPt->key_echo_count[readerPt->key.apply], sizeof(struct mcr_MapElement), mcr_int_compare);
		if (readerPt->keyEchoFound) {
			readerPt->echo.echo = readerPt->keyEchoFound->second.index;
			return mcr_dispatch(ctx, &readerPt->echosig);
		}
	}
	return false;
}

static inline bool GrabReader_handle_abs(GrabReader *readerPt, struct mcr_context *ctx, struct input_event *curVent)
{
	bool ret = false;
	readerPt->pos = to_position(curVent->code);
	if (readerPt->bAbs[readerPt->pos]) {
		abs_set_current(ctx, &readerPt->abs, readerPt->bAbs);
		/* Do write if blocked */
		ret = mcr_dispatch(ctx, &readerPt->abssig);
		bpos_zero(readerPt->bAbs);
	}
	readerPt->abs.position[readerPt->pos] = curVent->value;
	readerPt->bAbs[readerPt->pos] = true;
	return ret;
}

static inline bool GrabReader_handle_rel(GrabReader *readerPt, struct mcr_context *ctx, struct input_event *curVent)
{
	bool ret = false;
	readerPt->pos = to_position(curVent->code);
	switch (curVent->code) {
	case REL_X:
	case REL_Y:
	case REL_Z:
		if (readerPt->rel.position[readerPt->pos]) {
			if (mcr_dispatch(ctx, &readerPt->relsig))
				ret = true;
			mcr_SpacePosition_zero(readerPt->rel.position);
		}
		readerPt->rel.position[readerPt->pos] = curVent->value;
		break;

	case REL_HWHEEL:
	case REL_WHEEL:
	case REL_DIAL:
		if (readerPt->scr.dimensions[readerPt->pos]) {
			if (mcr_dispatch(ctx, &readerPt->scrsig))
				ret = true;
			mcr_Dimensions_zero(readerPt->scr.dimensions);
		}
		readerPt->scr.dimensions[readerPt->pos] = curVent->value;
		break;
	default:
		/* Not relative or scroll. */
		printf("An unknown relative event code \"%d\" has been encountered. "
			   "Please let the developers know this event code at the "
			   "following URL...\n"
			   "https://github.com/newpsoft/libmacro/issues/new", curVent->code);
	}
	return ret;
}

//static inline bool GrabReader_has_key(GrabReader *readerPt)
//{
//	return false;
//}

static inline bool GrabReader_has_abs(GrabReader *readerPt)
{
	for (int i = MCR_DIMENSION_COUNT; i--;) {
		if (readerPt->bAbs[i])
			return true;
	}
	return false;
}

static inline bool GrabReader_has_rel(GrabReader *readerPt)
{
	for (int i = MCR_DIMENSION_COUNT; i--;) {
		if (readerPt->rel.position[i])
			return true;
	}
	return false;
}

static inline bool GrabReader_has_scr(GrabReader *readerPt)
{
	for (int i = MCR_DIMENSION_COUNT; i--;) {
		if (readerPt->scr.dimensions[i])
			return true;
	}
	return false;
}

static inline size_t modbit_size()
{
	return sizeof(char) * max_modifier_val() / 8 + 1;
}

static inline int unlockReturn(struct mcr_intercept_platform *nPt, bool wasLockedFlag, int err)
{
	int thrdErr;
	if (wasLockedFlag) {
		thrdErr = mtx_unlock(&nPt->lock);
		if (!err && thrdErr != thrd_success)
			err = mcr_thrd_errno(thrdErr);
	}
	if (err)
		error_set_return(err);
	return 0;
}

bool mcr_intercept_enabled(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	int thrdErr = mtx_lock(&nPt->lock);
	bool ret = has_enabled_locked(ctx);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return ret;
}

int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	int thrdErr = mtx_lock(&nPt->lock);
	bool wasEnabled = has_enabled_locked(ctx);
	mcr_err = 0;
	/* Do not disable if already disabled. */
	if (enable) {
		if (wasEnabled) {
			/* Completely clear and re-enable. */
			if (mcr_thrd(thread_reset, ctx))
				mset_error(EINTR);
		} else {
			/* Enable, we are sure this is not a grab thread. */
			set_enabled_locked(ctx, enable);
		}
	} else if (wasEnabled) {
		/* Disable in thread in case this is a grabber thread. */
		if (mcr_thrd(thread_disable, ctx))
			mset_error(EINTR);
	}
	/* Do not disable if already disabled. */
	return unlockReturn(nPt, thrdErr == thrd_success, mcr_err);
}

unsigned int mcr_intercept_modifiers(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	int thrdErr = mtx_lock(&nPt->lock);
	unsigned int ret = get_mods_locked(ctx);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return ret;
}

int mcr_intercept_set_grabs(struct mcr_context *ctx, const char * const*allGrabPaths,
							size_t pathCount)
{
	int thrdErr;
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	mcr_err = 0;
	thrdErr = mtx_lock(&nPt->lock);
	// \todo Disabling in thread may cause errors.
	nPt->grab_paths = allGrabPaths;
	nPt->grab_path_count = pathCount;
	if (has_enabled_locked(ctx)) {
		printf("Warning: Intercept was enabled while setting grabber paths. "
			   "Currently running grabbers may or may not have issues being reset.\n");
		if (mcr_thrd(thread_reset, ctx))
			mset_error(EINTR);
	}
	return unlockReturn(nPt, thrdErr == thrd_success, mcr_err);
}

void mcr_intercept_set_key_echo_map(struct mcr_context *ctx, const struct mcr_MapElement *keyEchos, size_t count, enum mcr_ApplyType applyType)
{
	ctx->intercept.platform_pt->key_echos[applyType] = keyEchos;
	ctx->intercept.platform_pt->key_echo_count[applyType] = count;
}

static void initialize_key_echos()
{
	size_t i = 0, set = 0, unset = 0;
	int code;
	for (; i < MCR_STANDARD_ECHO_EVENT_DEFAULT_COUNT; i++) {
		code = mcr_standard_echo_event_defaults[i].code;
		if (mcr_standard_echo_event_defaults[i].value) {
			dassert(set < MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT);
			key_echos_impl[MCR_SET][set].first.integer = code;
			key_echos_impl[MCR_SET][set++].second.index = i;
		} else {
			dassert(unset < MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT);
			key_echos_impl[MCR_UNSET][unset].first.integer = code;
			key_echos_impl[MCR_UNSET][unset++].second.index = i;
		}
	}
	qsort(key_echos_impl[0], MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT, sizeof(struct mcr_MapElement), mcr_int_compare);
	qsort(key_echos_impl[1], MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT, sizeof(struct mcr_MapElement), mcr_int_compare);
}

int mcr_intercept_platform_initialize(struct mcr_context *ctx)
{
	int thrdErr = 0, err = 0;
	/* Free in deinitialize */
	struct mcr_intercept_platform *nPt;

	initialize_key_echos();

	if (!(nPt = malloc(sizeof(struct mcr_intercept_platform))))
		error_set_return(ENOMEM);
	ZERO_PTR(nPt);

	if ((thrdErr = mtx_init(&nPt->lock, mtx_plain)) != thrd_success) {
		err = mcr_thrd_errno(thrdErr);
		free(nPt);
		error_set_return(err);
	}

	nPt->key_echos[0] = mcr_intercept_key_echo_defaults[0];
	nPt->key_echo_count[0] = MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT;
	nPt->key_echos[1] = mcr_intercept_key_echo_defaults[1];
	nPt->key_echo_count[1] = MCR_INTERCEPT_KEY_ECHO_DEFAULT_COUNT;
	ctx->intercept.platform_pt = nPt;
	return 0;
}

int mcr_intercept_platform_deinitialize(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	int thrdErr;
	thrdErr = mtx_lock(&nPt->lock);
	if (clear_grabbers_locked(ctx))
		dmsg;
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	mtx_destroy(&nPt->lock);
	free(nPt);
	ctx->intercept.platform_pt = NULL;
	return mcr_err;
}

static bool has_enabled_locked(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	size_t i = nPt->grabber_count;
	while (i--) {
		if (mcr_Grabber_enabled(nPt->grabbers + i))
			return true;
	}
	return false;
}

static int set_enabled_locked(struct mcr_context *ctx, bool enable)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	size_t i;
	int firstError = 0;
	bool wasEnabled = has_enabled_locked(ctx);
	if (enable) {
		/* Enable, already enabled */
		if (wasEnabled)
			error_set_return(EBUSY);
		/* Enable, reset grabbers allocated. */
		if (apply_grab_paths_locked(ctx))
			return mcr_err;
	} else if(!wasEnabled) {
		/* disable, already disabled */
		return 0;
	}
	/* Disable or enable, woohoo */
	i = nPt->grabber_count;
	while (i--) {
		/* Apply to grabber, and begin intercept on enable. */
		if (mcr_Grabber_set_enabled(nPt->grabbers + i, enable)) {
			if (!firstError)
				firstError = mcr_err;
		} else if (enable && mcr_thrd(intercept_start, nPt->grabbers + i) && !firstError) {
			firstError = mcr_err;
		}
	}
	if (firstError)
		mcr_err = firstError;
	return mcr_err;
}

static unsigned int get_mods_locked(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	unsigned int ret = 0;
	/* One modifier for each key that has a modifier mapped */
	char *bitRetrieval = nPt->bit_retrieval;
	size_t modSize = modbit_size(), i = nPt->grabber_count;
	int fd;
	/// \bug handle MCR_MOD_ANY as an error
	if (ctx->standard.platform_pt->gen_dev.fd != -1) {
		if (ioctl(ctx->standard.platform_pt->gen_dev.fd, EVIOCGKEY(modSize), bitRetrieval) < 0) {
			mcr_errno(EINTR);
			return MCR_MF_NONE;
		}
		ret = modify_eventbits(ctx, bitRetrieval);
	}
	while (i--) {
		if ((fd = nPt->grabbers[i].fd) != -1) {
			if (ioctl(fd, EVIOCGKEY(modSize), bitRetrieval) < 0) {
				mcr_errno(EINTR);
			} else {
				ret |= modify_eventbits(ctx, bitRetrieval);
			}
		}
	}
	return ret;
}

static int thread_disable(void *threadArgs)
{
	struct mcr_context *ctx = threadArgs;
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	int thrdErr;
	thrdErr = mtx_lock(&nPt->lock);
	set_enabled_locked(ctx, false);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return mcr_err ? thrd_error : thrd_success;
}

static int thread_reset(void *threadArgs)
{
	struct mcr_context *ctx = threadArgs;
	dassert(ctx);
	int thrdErr = mtx_lock(&ctx->intercept.platform_pt->lock);
	/* Clear all to reset grabber blocking and paths to context current. */
	if (clear_grabbers_locked(ctx)) {
		if (thrdErr == thrd_success)
			mtx_unlock(&ctx->intercept.platform_pt->lock);
		return thrd_error;
	}
	dassert(!ctx->intercept.platform_pt->grabbers);
	dassert(!ctx->intercept.platform_pt->grabber_count);
	set_enabled_locked(ctx, true);
	if (thrdErr == thrd_success)
		mtx_unlock(&ctx->intercept.platform_pt->lock);
	return mcr_err ? thrd_error : thrd_success;
}

/* Note: Excess time setting up and releasing is ok. Please try to limit
 * time-consumption during read events and callbacks. */
static int intercept_start(void *threadArgs)
{
	struct mcr_Grabber *grabPt = threadArgs;
	dassert(grabPt);
	return grabPt->blocking ? read_grabber_blocking(grabPt) : read_grabber_passing(grabPt);
}

static int read_grabber_blocking(struct mcr_Grabber *grabPt)
{
	dassert(grabPt);
	struct mcr_context *ctx = grabPt->context;
	dassert(ctx);
	struct mcr_standard_platform *nStandardPt = ctx->standard.platform_pt;
	struct mcr_intercept_platform *nInterceptPt = ctx->intercept.platform_pt;
	struct input_event *curVent, *end;
	/* Always assume writing to the generic device.  Start writing
	   to abs only if those events happen at least once. */
	/* Write generic whenever not blocked, write abs only when available */
	/* Flag to write to device type.  Generic default true, and does not
	 * recover when blocked. Abs default false, and will set to write every time
	 * not blocked. */
	bool writegen = true, writeabs = false, writeagain = false;
	// Assume blocking does not change while enabled.
	dassert(grabPt->blocking);

	// Note: Possible bool blocking, hasGen, and hasAbs to not always write to gen device.
	// \todo Write to given fd and not Libmacro device.
	// \bug Uh-oh, are we not able to open fd in other thread?

	GrabReader reader;
	GrabReader_ctor(&reader, ctx);

	// Infinite loop optimization for inline function.
	do {
		if (!GrabReader_read(&reader, grabPt))
			break;

		curVent = reader.events;
		end = GrabReader_end(&reader);
		while (curVent < end) {
			switch (curVent->type) {
			case EV_SYN:
				break;
				/* Handle KEY */
			case EV_KEY:
				if (GrabReader_handle_key(&reader, ctx, nInterceptPt, curVent))
					writegen = false;
				break;
				/* Handle ABS */
			case EV_ABS:
				writeabs = !GrabReader_handle_abs(&reader, ctx, curVent);
				break;
			case EV_REL:
				if (GrabReader_handle_rel(&reader, ctx, curVent))
					writegen = false;
				break;
			default:
				/* Other event types should be written into the same device. */
				writeagain = true;
			}
			++curVent;
		}

		/* Call final event if it was not called yet. */
		/* Key is a single event, so it was dispatched immediately. */
		// Note: Here should we have an inline function to reset bAbs, rel, and scroll?
		if (GrabReader_has_abs(&reader)) {
			writeabs = !mcr_dispatch(ctx, &reader.abssig);
			bpos_zero(reader.bAbs);
		}
		if (GrabReader_has_rel(&reader)) {
			if (mcr_dispatch(ctx, &reader.relsig))
				writegen = false;
			mcr_SpacePosition_zero(reader.rel.position);
		}
		if (GrabReader_has_scr(&reader)) {
			if (mcr_dispatch(ctx, &reader.scrsig))
				writegen = false;
			mcr_Dimensions_zero(reader.scr.dimensions);
		}

		/* Write to event file does NOT fix blocked events, so we have to
		 * write to our own device. */
		if (writegen) {
			if (write(nStandardPt->gen_dev.fd, reader.events, reader.rdb) < 0) {
				/// \todo Is non-error ok with thrd error or success?
				mcr_errno(0);
				/* EINTR is disabled during read. */
				if (!mcr_err || mcr_err == EINTR)
					return thrd_success;
				return thrd_error;
			}
		} else {
			writegen = true;
		}
		if (writeabs) {
			if (write(nStandardPt->abs_dev.fd, reader.events, reader.rdb) < 0) {
				mcr_errno(0);
				/* EINTR is disabled during read. */
				if (!mcr_err || mcr_err == EINTR)
					return thrd_success;
				return thrd_error;
			}
			writeabs = false;
		}
		if (writeagain) {
			/* \bug Check that writing to event file will fix blocking keyboard LED's */
			if (ioctl(grabPt->fd, EVIOCGRAB, 0) < 0) {
				perror("Ungrab event error");
			} else {
				write(grabPt->fd, reader.events, reader.rdb);
				/* Ignore rewrite events, \bug may ignore other events */
				read(grabPt->fd, reader.events, sizeof(reader.events));
				if (ioctl(grabPt->fd, EVIOCGRAB, 1) < 0) {
					perror("Grab event error");
				}
//				write(grabPt->fd, &mcr_syncer, sizeof(mcr_syncer));
			}
			writeagain = false;
		}
	} while(true);

	return mcr_err ? thrd_error : thrd_success;
}

static int read_grabber_passing(struct mcr_Grabber *grabPt)
{
	dassert(grabPt);
	struct mcr_context *ctx = grabPt->context;
	dassert(ctx);
	struct mcr_intercept_platform *nInterceptPt = ctx->intercept.platform_pt;
	struct input_event *curVent, *end;
	// Assume blocking does not change while enabled.
	dassert(!grabPt->blocking);

	// Note: Possible bool blocking, hasGen, and hasAbs to not always write to gen device.
	// \todo Write to given fd and not Libmacro device.
	// \bug Uh-oh, are we not able to open fd in other thread?

	GrabReader reader;
	GrabReader_ctor(&reader, ctx);

	// Infinite loop optimization for inline function.
	do {
		if (!GrabReader_read(&reader, grabPt))
			break;

		curVent = reader.events;
		end = GrabReader_end(&reader);
		while (curVent < end) {
			switch (curVent->type) {
			case EV_SYN:
				break;
				/* Handle KEY */
			case EV_KEY:
				GrabReader_handle_key(&reader, ctx, nInterceptPt, curVent);
				break;
				/* Handle ABS */
			case EV_ABS:
				GrabReader_handle_abs(&reader, ctx, curVent);
				break;
			case EV_REL:
				GrabReader_handle_rel(&reader, ctx, curVent);
				break;
			}
			++curVent;
		}

		/* Call final event if it was not called yet. */
		/* Key is a single event, so it was dispatched immediately. */
		// Note: Here should we have an inline function to reset bAbs, rel, and scroll?
		if (GrabReader_has_abs(&reader)) {
			mcr_dispatch(ctx, &reader.abssig);
			bpos_zero(reader.bAbs);
		}
		if (GrabReader_has_rel(&reader)) {
			mcr_dispatch(ctx, &reader.relsig);
			mcr_SpacePosition_zero(reader.rel.position);
		}
		if (GrabReader_has_scr(&reader)) {
			mcr_dispatch(ctx, &reader.scrsig);
			mcr_Dimensions_zero(reader.scr.dimensions);
		}
	} while(true);

	return mcr_err ? thrd_error : thrd_success;
}

static unsigned int modify_eventbits(struct mcr_context *ctx, char *keybitValues)
{
	int curKey;
	unsigned int modValOut = 0;
	size_t i;
	const struct mcr_MapElement *keyMods = ctx->standard.key_modifiers;
	size_t keyModCount = ctx->standard.key_modifier_count;
	/* For each modifier, find the modifier of that key, and check
	 * if key is set in the key bit value set. */
	for (i = keyModCount; i--;) {
		curKey = keyMods[i].second.integer;
		if (curKey != MCR_KEY_ANY
				&& (keybitValues[MCR_EVENTINDEX(curKey)] &
					MCR_EVENTBIT(curKey))) {
			modValOut |= keyMods[i].first.u_integer;
		}
	}
	return modValOut;
}

static unsigned int max_modifier_val()
{
	unsigned int val = KEY_LEFTALT;
	unsigned int keys[] = {KEY_RIGHTALT, KEY_RIGHTALT, KEY_OPTION, KEY_OPTION, KEY_COMPOSE, KEY_COMPOSE, KEY_LEFTCTRL, KEY_LEFTCTRL, KEY_RIGHTCTRL, KEY_RIGHTCTRL, KEY_FN, KEY_FN, KEY_FRONT, KEY_FRONT, KEY_LEFTSHIFT, KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTMETA, KEY_LEFTMETA, KEY_RIGHTMETA, KEY_RIGHTMETA};
	for (size_t i = arrlen(keys); i--;) {
		if (keys[i] > val)
			val = keys[i];
	}
	return val;
}

static int apply_grab_paths_locked(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	size_t i;
	int firstErr = 0;
	struct mcr_NoOp noop = { 0, MCR_POLL_TIMEOUT };
	if (nPt->grabber_count != nPt->grab_path_count && clear_grabbers_locked(ctx))
		return mcr_err;
	/* Empty, cannot do more */
	if (!nPt->grab_path_count)
		return 0;
	if (nPt->grabbers) {
		/* Grabbers already existed with equal number as path count. Preserve
		 * blocking value. */
		for (i = 0; i < nPt->grabber_count; i++) {
			if (mcr_Grabber_set_enabled(nPt->grabbers + i, false) && !firstErr)
				firstErr = mcr_err;
			nPt->grabbers[i].path = nPt->grab_paths[i];
		}
		/* Allow timeout in grabber read */
		mcr_NoOp_send_member(&noop);
		thrd_yield();
	} else {
		nPt->grabbers = malloc(sizeof(struct mcr_Grabber) * nPt->grab_path_count);
		nPt->grabber_count = nPt->grab_path_count;
		for (i = 0; i < nPt->grabber_count; i++) {
			mcr_Grabber_ctor(nPt->grabbers + i, ctx, ctx->intercept.blockable, nPt->grab_paths[i]);
		}
	}
	if (firstErr)
		mcr_err = firstErr;
	return mcr_err;
}

static int clear_grabbers_locked(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform_pt;
	/* Disable all, wait for all batch operation. */
	int timeout = 50;	// Total 10 sec
	struct mcr_NoOp delay = { 0, MCR_POLL_TIMEOUT };
	size_t i;
	/// \todo thread destroy on timeout.
	while (timeout-- && has_enabled_locked(ctx)) {
		/* Disable all */
		for (i = 0; i < nPt->grabber_count; i++) {
			mcr_Grabber_set_enabled(nPt->grabbers + i, false);
		}
		/* Allow disabled grabbers to time out. */
		mcr_NoOp_send_member(&delay);
	}
	/* Impatient free-all, unconditional end for timed out waiting */
	for (i = 0; i < nPt->grabber_count; i++) {
		mcr_Grabber_set_enabled(nPt->grabbers + i, false);
		if (mcr_Grabber_deinit(nPt->grabbers + i))
			dmsg;
	}
	if (nPt->grabbers)
		free(nPt->grabbers);
	nPt->grabbers = NULL;
	nPt->grabber_count = 0;
	return 0;
}
