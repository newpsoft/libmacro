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
 *  \brief \ref mcr_HidEcho - Simulate human interface with spatial
 *  coordinates.
 */

#ifndef MCR_STANDARD_HID_ECHO_H_
#define MCR_STANDARD_HID_ECHO_H_

#include "mcr/base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Simulate human interface with spatial position activation. */
struct mcr_HidEcho {
	/*! \ref retind Echo code */
	size_t echo;
};

/*! \pre Signal data is \ref mcr_HidEcho
 *  \brief Send human interface signal
 *
 *  \param signalPt Has data member \ref mcr_HidEcho
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_send(struct mcr_Signal *signalPt);
/*! \ref mcr_HidEcho_send
 *
 *  \ref mcr_platform
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_send_member(struct mcr_HidEcho *dataPt, struct mcr_context *ctx);
/* Default allocate, deallocate, init, deinit, compare, and copy */

/*! Get the number of registered echo codes.
 *
 * \ref mcr_platform
 */
MCR_API size_t mcr_HidEcho_count(struct mcr_context *ctx);

/*! Get the \ref mcr_ISignal of \ref mcr_HidEcho */
MCR_API struct mcr_ISignal *mcr_iHidEcho(struct mcr_context *ctx);
/*! Signal data cast \ref mcr_HidEcho * */
#define mcr_HidEcho_data(sigPt) \
mcr_castpt(struct mcr_HidEcho, mcr_Signal_data(sigPt))

#ifdef __cplusplus
}
#endif

#endif
