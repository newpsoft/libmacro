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
 *  \brief \ref mcr_DataMember
 */

#ifndef MCR_BASE_DATA_MEMBER_H_
#define MCR_BASE_DATA_MEMBER_H_

#include "mcr/util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

//! \todo Use interface reference to deallocate and deinit, instead of function pointers?

/*! An object reference that remembers how to destroy itself.  It may or may
 *  not be in heap memory to deallocate.
 *
 *  This reference has no knowledge of how it was constructed, only how to destroy.
 */
struct mcr_DataMember {
	/*! Object member reference */
	void *data;
	/*! Deallocate from heap.
	 *
	 *  If data is allocated with C++ new or new[] operators, please
	 *  create functions to delete correctly. \ref mcr::deleteDataMember */
	void (*deallocate)(void *);
	/*! Free resources but do not free from heap.
	 *
	 *  This will be called before deallocate.
	 */
	mcr_data_fnc deinit;
};

/*! \ref ctor
 *
 *  \param dataPt \ref opt \ref mcr_DataMember.data
 *  \param deallocate \ref opt \ref mcr_DataMember.deallocate
 *  \param deinit \ref opt \ref mcr_DataMember.deinit
 */
MCR_API void mcr_DataMember_ctor(struct mcr_DataMember *dataMemberPt,
								 void *dataPt,
								 void (*deallocateFnc)(void *),
								 mcr_data_fnc deinit);
/*! \ref dtor */
#define mcr_DataMember_deinit mcr_DataMember_destroy

/*! Set data reference and deallocator.
 *
 *  \param dataMemberPt \ref mcr_Data
 *  \param dataPt \ref opt \ref mcr_DataMember.data
 *  \param deallocateFnc \ref opt \ref mcr_DataMember.deallocate
 */
MCR_API void mcr_DataMember_set(struct mcr_DataMember *dataMemberPt,
								void *dataPt, void (*deallocateFnc)(void *),
								mcr_data_fnc deinit);

/*! Is data heap allocated?
 *
 *  \param dataMemberPt \ref opt
 *  \return bool
 */
MCR_API bool mcr_DataMember_heap(struct mcr_DataMember *dataMemberPt);

/*! Deinit and deallocate data if needed, and nullify this object.
 *
 *  \param dataMemberPt \ref opt
 */
MCR_API void mcr_DataMember_destroy(struct mcr_DataMember *dataMemberPt);

#ifdef __cplusplus
}
#endif

#endif
