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
 *  \brief Forward declaration of types
 */

#ifndef MCR_BASE_TYPES_H_
#define MCR_BASE_TYPES_H_

#include "mcr/util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_context;

struct mcr_IDispatcher;
struct mcr_IRegistry;
struct mcr_ISignal;
struct mcr_Signal;
struct mcr_ITrigger;
struct mcr_Trigger;
struct mcr_DispatchReceiver;

/*! Receive dispatch before signal is sent
 *
 *  \param dispatchReceiver Has a reference to the receiver.
 *  \ref mcr_DispatchReceiver.receiver
 *  \param dispatchSignal Intercepted signal
 *  \param mods Intercepted modifiers
 *  \return True to block from sending
 */
typedef bool (*mcr_dispatch_receive_fnc)(struct mcr_DispatchReceiver
		*dispatchReceiver,
		struct mcr_Signal *dispatchSignal, unsigned int mods);
/*! Generic receiver and function that may block or intercept a signal
 *  being sent.
 *
 *  Receiver is first, to compare with \ref mcr_ref_compare.
 */
struct mcr_DispatchReceiver {
	/*! Object to receive signal
	 *
	 *  Optional only if the receive function does not require a receiver
	 *  object.
	 */
	void *receiver;
	/*! Function to act on receiver */
	mcr_dispatch_receive_fnc receive;
};

/*! Sort by first element, such as a reference address, dispatch to either an
 *  array of receivers \ref mcr_ReceiverMapElement.receivers, or continue searching
 *  with new keys in \ref mcr_ReceiverMapElement.recursive_receivers. */
struct mcr_ReceiverMapElement {
	/*! Sort and find by */
	union {
		void *reference;
		struct mcr_Signal *signal_pt;
		char *string;
		char character;
		int integer;
		unsigned int u_integer;
		size_t index;
	};
	union {
		/*! Array of receivers, all will receive. */
		struct mcr_DispatchReceiver *receivers;
		/*! Array to find with bsearch, sorted by first element. */
		struct mcr_ReceiverMapElement *recursive_receivers;
	};
	/*! Number of either \ref mcr_ReceiverMapElement.receivers or
	 *  \ref mcr_ReceiverMapElement.recursive_receivers */
	size_t receiver_count;
};

/*! A map element sorts by the first, and second is the mapped value.
 *
 *  Note this can also be used to hold an unmanaged array of any type.
 */
struct mcr_MapElement {
	union {
		void *ptr;
		void **references;
		struct mcr_Signal *signal_pt;
		struct mcr_Trigger *trigger_pt;
		char *string;
		char character;
		int integer;
		unsigned int u_integer;
		size_t index;
	} first;
	union {
		void *ptr;
		struct mcr_Signal *signal_pt;
		struct mcr_Trigger *trigger_pt;
		char *string;
		char character;
		int integer;
		unsigned int u_integer;
		size_t index;
		size_t count;
		size_t used;
		size_t length;
	} second;
};

/*! Can be used as a function parameter for an array. */
typedef struct mcr_Array_tag {
	void *ptr;
	size_t count;
} mcr_Array_t;

/*! \todo may replace mcr_Map with this. */
typedef struct mcr_Map_tag {
	struct mcr_MapElement *ptr;
	size_t count;
} mcr_Map_t;

//! \todo Maybe not used
struct mcr_BiMap {
	struct mcr_Map_tag first;
	struct mcr_Map_tag second;
};

/*! Usable spatial dimensions for cursor/spatial events */
enum mcr_Dimension {
	MCR_X = 0,
	MCR_Y,
	MCR_Z,
	//! \ref mcr_future
	MCR_W,
	MCR_Y1 = MCR_X,
	MCR_Y2 = MCR_Y,
	MCR_Y3 = MCR_Z,
	//! \ref mcr_future
	MCR_Y4 = MCR_W,
	MCR_DIMENSION_MAX = MCR_Z,
	MCR_DIMENSION_COUNT = MCR_DIMENSION_MAX + 1
};

/*! A value state is set or unset */
enum mcr_ApplyType {
	/*! Set, value is applied. */
	MCR_SET = 0,
	/*! Unset, value is released or not applied. */
	MCR_UNSET,
	/*! Value is both set and unset, in that order. */
	MCR_BOTH,
	/*! Set or unset is swapped.  Other mcr_ApplyType are ignored. */
	MCR_TOGGLE
};

/*! Spatial position, indices are coordinates.
 *  \ref MCR_DIMENSION_COUNT
 *
 *  This is logically a spatial vector for MCR_DIMENSION_COUNT coordinates.
 */
typedef long long mcr_SpacePosition[MCR_DIMENSION_COUNT];
/*! \ref mcr_SpacePosition */
typedef mcr_SpacePosition mcr_Dimensions;

static MCR_INLINE void mcr_SpacePosition_zero(mcr_SpacePosition pos)
{
	for (int i = MCR_DIMENSION_COUNT; i--;) {
		pos[i] = 0;
	}
}

static MCR_INLINE void mcr_Dimensions_zero(mcr_Dimensions pos)
{
	mcr_SpacePosition_zero(pos);
}

#ifdef __cplusplus
}
#endif

#endif
