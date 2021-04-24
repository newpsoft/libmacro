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
 *  \brief \ref mcr_MoveCursor - Change spatial coordinates of cursor.
 */

#ifndef MCR_STANDARD_MOVE_CURSOR_H_
#define MCR_STANDARD_MOVE_CURSOR_H_

#include "mcr/base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Change spatial coordinates of ursor.*/
struct mcr_MoveCursor {
	/*! Spatial coordinates to set or modify with */
	mcr_SpacePosition position;
	/*! If false coordinates will be set, otherwise coordinates
	 *  adjust the cursor for current position. */
	bool justify_flag;
};

/*! \pre Signal data is  \ref mcr_MoveCursor
 *  \brief Move HID cursor position.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_MoveCursor_send(struct mcr_Signal *signalPt);
/*! \ref mcr_MoveCursor_send
 *
 *  \todo Translate absolute coordinates on all screens size. Also allow to
 *  recalculate position values when screens size changes.
 *  \ref mcr_platform
 *  \return \ref reterr
 */
MCR_API int mcr_MoveCursor_send_member(struct mcr_MoveCursor *mcPt, struct mcr_context *ctx);
/* Default allocate, deallocate, init, deinit, compare, and copy */

/*! Current cursor position
 *
 *  \ref mcr_platform
 */
MCR_API void mcr_cursor_position(struct mcr_context *ctx, mcr_SpacePosition buffer);
/*! If justified then \ref mcr_resembles_justified,
 *  else \ref mcr_resembles_absolute
 *
 *  \param lhs \ref opt
 *  \param rhs \ref opt
 *  \param measurementError Only used in absolute
 *  \return If false cursors do not resemble each other, otherwise they do
 */
MCR_API bool mcr_resembles(const struct mcr_MoveCursor *lhs,
						   const struct mcr_MoveCursor *rhs, const unsigned int measurementError);

/*! Signal interface of \ref mcr_MoveCursor */
MCR_API struct mcr_ISignal *mcr_iMoveCursor(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
