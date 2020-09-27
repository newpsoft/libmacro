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
 *  \brief \ref mcr_Interface
 */

#ifndef MCR_BASE_INTERFACE_H_
#define MCR_BASE_INTERFACE_H_

#include "mcr/base/data_member.h"
#include "mcr/base/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! An interface describes an object of a specific type */
struct mcr_Interface {
	/*! The unique identifier for this object type,
	 *  -1 for unregistered types */
	size_t id;
	/*! Byte size of the data member for this type. If 0, allocate, deallocate,
	 *  copy, and compare must be defined. */
	size_t data_size;
	/*! \ref opt Libmacro context reference */
	struct mcr_context *context;
	/*! \ref opt The registry this interface is registered with. */
	struct mcr_IRegistry *registry;

	/*! Allocate data member to heap, default malloc */
	void *(*allocate)();
	/*! Deallocate from heap, default free */
	void (*deallocate)(void *);
	/*! Initialize object resources, default zero all */
	mcr_data_fnc init;
	/*! Free object resources, default do nothing */
	mcr_data_fnc deinit;
	/*! Compare two data members of this type. default memcmp */
	mcr_compare_fnc compare;
	/*! Copy from source object to destination object,
	 *  default memcpy */
	mcr_copy_fnc copy;
};

/* Interface */
/*! \ref ctor
 *
 *  \param interfacePt \ref opt
 */
MCR_API void mcr_Interface_init(struct mcr_Interface *interfacePt);
/*! \ref ctor
 *
 *  \param interfacePt \ref opt
 *  \param context \ref opt \ref mcr_Interface.context
 *  \param dataSize \ref opt \ref mcr_Interface.data_size
 *  \param allocate \ref opt \ref mcr_Interface.allocate
 *  \param deallocate \ref opt \ref mcr_Interface.deallocate
 *  \param init \ref opt \ref mcr_Interface.init
 *  \param deinit \ref opt \ref mcr_Interface.deinit
 *  \param compare \ref opt \ref mcr_Interface.compare
 *  \param copy \ref opt \ref mcr_Interface.copy
 */
MCR_API void mcr_Interface_ctor(struct mcr_Interface *interfacePt,
								struct mcr_context *context,
								size_t dataSize, void *(*allocate)(), void (*deallocate)(void *),
								mcr_data_fnc init, mcr_data_fnc deinit, mcr_compare_fnc compare,
								mcr_copy_fnc copy);
/*! \ref dtor
 *
 *  Removes self from registry, if the registry is set.
 *  \param interfacePt \ref opt
 */
MCR_API void mcr_Interface_deinit(struct mcr_Interface *interfacePt);

/*! Set all functions
 *
 *  \param context \ref opt \ref mcr_Interface.context
 *  \param interfacePt
 *  \param dataSize \ref opt \ref mcr_Interface.data_size
 *  \param init \ref opt \ref mcr_Interface.init
 *  \param deinit \ref opt \ref mcr_Interface.deinit
 *  \param compare \ref opt \ref mcr_Interface.compare
 *  \param copy \ref opt \ref mcr_Interface.copy
 */
MCR_API void mcr_Interface_set_all(struct mcr_Interface *interfacePt,
								   struct mcr_context *context, size_t dataSize,
								   void *(*allocate)(), void (*deallocate)(void *),
								   mcr_data_fnc init, mcr_data_fnc deinit,
								   mcr_compare_fnc compare, mcr_copy_fnc copy);

/*! Get the id of an interface.
 *
 *  \param interfacePt \ref opt
 *  \return \ref retid
 */
MCR_API size_t mcr_Interface_id(struct mcr_Interface *interfacePt);

/* Interface functions on data */
/*! Heap-allocate and initialize an object
 *
 *  1) Use \ref malloc to allocate object\n
 *  2) Initialize object with \ref mcr_Interface.init\n
 *  3) Set \ref mcr_DataMember.deallocate to \ref free\n
 *  \param interfacePt \ref opt The initialized object
 *  will be empty without this
 *  \param dataMemberPt \ref opt Object to initialize
 *  \return \ref reterr
 */
MCR_API int mcr_Interface_allocate(const struct mcr_Interface *interfacePt,
								   struct mcr_DataMember *dataMemberPt);

/*! Release object resources and deallocate if needed.
 *
 *  1) Release resources with \ref mcr_Interface.deinit\n
 *  2) Use \ref mcr_DataMember.deallocate if it is set\n
 *  3) Nullify \ref mcr_DataMember object\n
 *  \param interfacePt \ref opt
 *  \param dataMemberPt \ref opt Object to deallocate or free
 *  \return \ref reterr
 */
MCR_API int mcr_Interface_deallocate(const struct mcr_Interface *interfacePt,
									 struct mcr_DataMember *dataMemberPt);

/*! Use an interface to compare two objects
 *
 *  \ref mcr_compare_fnc
 *  \param interfacePt
 *  \param lhs \ref opt Left side of comparison
 *  \param rhs \ref opt Right side of comparison
 *  \return \ref retcmp
 */
MCR_API int mcr_Interface_compare(const struct mcr_Interface *interfacePt,
								  const struct mcr_DataMember *lhs, const struct mcr_DataMember *rhs);

/*! Use an interface to copy an object
 *
 *  \ref mcr_copy_fnc
 *  If no source is given, destination will be freed.
 *  \param interfacePt
 *  \param dstPt Object data is copied into
 *  \param srcPt \ref opt Object data is copied from
 *  \return \ref reterr
 */
MCR_API int mcr_Interface_copy(const struct mcr_Interface *interfacePt,
							   struct mcr_DataMember *dstPt,
							   const struct mcr_DataMember *srcPt);

/*! Replace object reference using an interface
 *
 *  Existing data will be freed. The assigned data will be copied directly
 *  and without modification.
 *  \param interfacePt
 *  \param dataMemberPt \ref opt Object to assign to
 *  \param data \ref opt \ref mcr_DataMember.data
 *  \param heapFlag If true, the data member will remember to deallocate from
 *  heap. \ref mcr_Interface.deallocate
 *  \return \ref reterr
 */
MCR_API int mcr_Interface_set_data(const struct mcr_Interface *interfacePt,
								   struct mcr_DataMember *dataMemberPt, void *data, bool heapFlag);

/*! Deinit and reinitialize data.
 *
 *  \pre If \ref mcr_Interface.init and \ref mcr_Interface.deinit are not
 *  defined then nothing will be done.  Zeroing object would be unsafe here.
 *  \param interfacePt
 *  \param dataMemberPt \ref opt Data to reset.  Do nothing if this is null
 *  \return \ref reterr
 */
MCR_API int mcr_Interface_reset(const struct mcr_Interface *interfacePt,
								struct mcr_DataMember *dataMemberPt);

#ifdef __cplusplus
}
#endif

#endif
