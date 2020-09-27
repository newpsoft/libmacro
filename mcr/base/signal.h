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
 *  \brief \ref mcr_Signal - Signal to send or dispatch
 *
 *  See \ref mcr_send
 *  See \ref mcr_dispatch
 */

#ifndef MCR_BASE_SIGNAL_H_
#define MCR_BASE_SIGNAL_H_

#include "mcr/base/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Use functions in \ref mcr_ISignal to dispatch and
 *  send the whole object, mcr_ISignal + data members.
 */
struct mcr_Signal {
	/*! \ref mcr_Instance of \ref mcr_ISignal */
	union {
		struct mcr_ISignal *isignal;
		struct mcr_Instance instance;
		struct mcr_Interface *interface;
	};
	/* Signal specific */
	/*! False to not dispatch, otherwise do dispatch */
	bool dispatch_flag;
};

/*! \ref ctor
 *
 *  \param signalPt \ref opt
 */
MCR_API void mcr_Signal_init(struct mcr_Signal *signalPt);
/*! \ref dtor
 *
 *  \param signalPt \ref opt
 */
MCR_API void mcr_Signal_deinit(struct mcr_Signal *signalPt);

/*! \ref mcr_Instance_copy, then copy \ref mcr_Signal.dispatch_flag
 *
 *  \param dstPt Destination to copy to
 *  \param srcPt \ref opt Source to copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Signal_copy(struct mcr_Signal *dstPt,
							const struct mcr_Signal *srcPt);
/*! Compare two signals
 *
 *  \param lhsSignalPt
 *  \param rhsSignalPt
 *  \return \ref retcmp
 */
MCR_API int mcr_Signal_compare(const struct mcr_Signal *lhsSignalPt,
							   const struct mcr_Signal *rhsSignalPt);

//! \ref mcr_Instance_data
static MCR_INLINE void *mcr_Signal_data(struct mcr_Signal *signalPt)
{
	return signalPt ? signalPt->instance.data_member.data : NULL;
}

#ifdef __cplusplus
}
#endif

#endif
