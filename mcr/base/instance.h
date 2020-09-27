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
 *  \brief \ref mcr_Instance Example and abstraction of an
 *  interface instance.
 *
 *  All instance implementations should begin with
 *  \ref mcr_Interface *, and then a \ref mcr_DataMember structure.\n
 */

#ifndef MCR_BASE_INSTANCE_H_
#define MCR_BASE_INSTANCE_H_

#include "mcr/base/interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Example of an instance of an interface. */
struct mcr_Instance {
	/*! A reference to the interface */
	struct mcr_Interface *interface;
	/*! Actual data of this instance */
	struct mcr_DataMember data_member;
};

/*! \ref ctor
 *
 *  \param instancePt \ref opt
 */
MCR_API void mcr_Instance_init(struct mcr_Instance *instancePt);

/*! \ref dtor
 *
 *  Free resources of an instance
 *  \param instancePt \ref opt
 *  \return \ref reterr
 */
MCR_API void mcr_Instance_deinit(struct mcr_Instance *instancePt);

/*! Get the id of an instance's interface
 *
 *  \ref mcr_Interface_id
 *  \param instancePt \ref opt
 *  \return \ref retid
 */
MCR_API size_t mcr_Instance_id(struct mcr_Instance *instancePt);

/*! Get \ref mcr_DataMember.data
 *
 *  \param instancePt \ref opt
 *  \return void *
 */
static MCR_INLINE void *mcr_Instance_data(struct mcr_Instance *instancePt)
{
	return instancePt ? instancePt->data_member.data : NULL;
}

/*! \ref mcr_DataMember_heap
 *
 *  \param instancePt \ref opt
 *  \return bool
 */
MCR_API bool mcr_Instance_heap(struct mcr_Instance *instancePt);

/*! Change the interface of an instance
 *
 *  Existing data will be deinitialized before setting the new interface.
 *  \param instancePt
 *  \param interfacePt \ref opt
 *  \return \ref reterr
 */
MCR_API int mcr_Instance_set_interface(struct mcr_Instance *instancePt,
									   struct mcr_Interface *interfacePt);
/*! Free and nullify existing data
 *
 *  \ref mcr_Interface_deallocate
 *  \param instancePt
 *  \return \ref reterr
 */
MCR_API int mcr_Instance_clear(struct mcr_Instance *instancePt);
/*! Free existing data and reinitialize
 *
 *  \ref mcr_Interface_reset
 *  \param instancePt
 *  \return \ref reterr
 */
MCR_API int mcr_Instance_reset(struct mcr_Instance *instancePt);
/*! Compare two instances
 *
 *  \ref mcr_Interface_compare
 *  \param lhsPt \ref opt
 *  \param rhsPt \ref opt
 *  \return \ref retcmp
 */
MCR_API int mcr_Instance_compare(const struct mcr_Instance *lhsPt,
								 const struct mcr_Instance *rhsPt);
/*! \ref Copy from source instance to destination instance
 *
 *  \ref mcr_Interface_copy
 *  \param dstPt Destination
 *  \param srcPt \ref opt Source
 *  \return \ref reterr
 */
MCR_API int mcr_Instance_copy(struct mcr_Instance *dstPt,
							  const struct mcr_Instance *srcPt);

#ifdef __cplusplus
}
#endif

#endif
