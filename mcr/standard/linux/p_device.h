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

/*! \file
 *  \brief Currently Linux-specific. mcr_Device is a wrapper for
 *  creating uinput devices.
 *
 *  The source file contains predefined devices for keyboard,
 *  absolute, and relative.
 */

#ifndef MCR_STANDARD_LINUX_P_DEVICE_H_
#define MCR_STANDARD_LINUX_P_DEVICE_H_

#include "mcr/base/base.h"
#include "mcr/util/linux/p_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int set_ev_bit;
	int first;
	int last;
} mcr_EvbitRange_t;

/*! Linux - Wrapper for uinput devices. */
struct mcr_Device {
	struct mcr_context *context;
	/*! Create a device and write to it */
	struct uinput_user_dev device;
	struct {
		const int *set_events;
		size_t set_event_count;
	};
	/*! int => int[], input type to input bit array */
	struct {
		const mcr_EvbitRange_t *event_bits;
		size_t event_bit_count;
	};
	/*! File descriptor to write input_events */
	int fd;
	/*! File descriptor for event node, to ioctl and read from */
	int event_fd;
	/*! false disabled, otherwise can write events to fd */
	bool enabled;
};

/*! Linux - Append similar input_event to the end of all sending of events. */
extern MCR_API const struct input_event mcr_syncer;

/*! Linux - Set the uinput file to read and write uinput devices
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_uinput_path(struct mcr_context *ctx, const char *path);
/*! Linux - Set the directory event files are located in
 *
 *  Event files are used to read device state
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_event_path(struct mcr_context *ctx, const char *directoryPath);
/*! Linux - Set absolute device resolution
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_absolute_resolution(struct mcr_context *ctx, __s32 resolution);
/*! \ref ctor
 *
 *  \param devPt \ref opt
 *  \return \ref reterr
 */
MCR_API void mcr_Device_ctor(struct mcr_Device *devPt, struct mcr_context *ctx, const char *name, const int *setEvents, size_t setEventCount, const mcr_EvbitRange_t *eventBits, size_t eventBitCount);
/*! \ref dtor
 *
 *  \param devPt \ref opt
 *  \return \ref reterr
 */
MCR_API void mcr_Device_deinit(struct mcr_Device *devPt);
/*! Start or end user device. This will modify fd, event_fd,
 *  and enabled state.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_enable(struct mcr_Device *devPt, bool enable);
/*! \ref mcr_Device_enable for all devices
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_enable_all(struct mcr_context *ctx, bool enable);

MCR_API void mcr_Device_set_uibit(struct mcr_Device *devPt, int *uiEvents, size_t count);
/*! Set input bit values for a single bit type.
 *
 *  \param bitType Type of event to set values for
 *  \param bits Set of all values to set for bitType
 *  \param bitLen Length of bits
 *  \return \ref reterr
 */
MCR_API void mcr_Device_set_evbits(struct mcr_Device *devPt, mcr_EvbitRange_t *evBits, size_t count);
/*! Get all input bits to be set.
 *
 *  \param bitType Type of event to find values for
 *  \return The set of all input bits to be set, or
 *  null if not found.
 */
MCR_API mcr_EvbitRange_t *mcr_Device_bits(struct mcr_Device *devPt, int setUiBit);
/*! Does the device have UI_SET_EVBIT? */
MCR_API bool mcr_Device_has_evbit(struct mcr_Device *devPt);

/* Sending, inlined for efficiency. */
/*! Send input_events to given device.
 *
 *  \param eventObjects input_event * Pointer to or array of input_events
 *  \param size size_t Byte size of all input_events in eventObjects
 *  \return \ref reterr
 */
static MCR_INLINE int mcr_Device_send(struct mcr_Device *devPt, const struct input_event *eventObjects, size_t bytes)
{
	int err;
	if (devPt->fd == -1) {
		mcr_err = ENXIO;
		return ENXIO;
	}
	if (write(devPt->fd, eventObjects, bytes) != bytes) {
		mcr_err = errno;
		if (!mcr_err)
			mcr_err = EIO;
	}
	return err;
}

/*! \ref mcr_Device_send for single input_event.
 *
 *  \param eventObject input_event * Pointer to single event object
 *  \return \ref reterr
 */
static MCR_INLINE int mcr_Device_single(struct mcr_Device *devPt, const struct input_event *eventObjects)
{
	return mcr_Device_send(devPt, eventObjects, sizeof(struct input_event));
}

/*! Send a syncing event to this device. */
static MCR_INLINE int mcr_Device_sync(struct mcr_Device *devPt)
{
	return mcr_Device_send(devPt, &mcr_syncer, sizeof(struct input_event));
}

/*! Initialize values for known mcr_Device objects,
 *  and allocate resources.
 */
MCR_API int mcr_Device_initialize(struct mcr_context *context);
/*! Dealocate resources for linux devices. */
MCR_API int mcr_Device_deinitialize(struct mcr_context *context);

#ifdef __cplusplus
}
#endif

#endif
