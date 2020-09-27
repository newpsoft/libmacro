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
 */

#ifndef MCR_BASE_SIGNAL_SET_H_
#define MCR_BASE_SIGNAL_SET_H_

#include "mcr/base/signal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Sample C-implementation to receive dispatch. Not thread-safe. */
struct mcr_SignalSet {
	struct mcr_context *ctx;
	bool block_flag;
#ifndef signals
	struct mcr_Signal *signals;
#endif
	size_t signal_count;
};

MCR_API bool mcr_SignalSet_receive(struct mcr_DispatchReceiver
								   *dispatchReceiver, struct mcr_Signal *signalPt, unsigned int mods);

#ifdef __cplusplus
}
#endif

#endif
