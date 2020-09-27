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
 *  \brief \ref mcr_IRegistry
 *  Register \ref mcr_Interface instances as id, names, and a reference.
 */

#ifndef MCR_BASE_IREGISTRY_H_
#define MCR_BASE_IREGISTRY_H_

#include "mcr/base/interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! For a given category of interfaces, register an id and name
 *  for each interface definition.
 */
struct mcr_IRegistry {
	//! Notify this object is to be destroyed.
	void (*deinit)(struct mcr_IRegistry *);

	size_t (*id)(const struct mcr_IRegistry *, struct mcr_Interface *);
	struct mcr_Interface *(*from_id)(const struct mcr_IRegistry *, size_t id);
	const char *(*name)(const struct mcr_IRegistry *, struct mcr_Interface *);
	struct mcr_Interface *(*from_name)(const struct mcr_IRegistry *,
									   const char *name);
	/*! Register an interface as the given name.
	 *
	 *  If the interface is already registered, the previously mapped name will
	 *  not be removed.
	 *  \return \ref retind Id of registered interface.
	 */
	size_t (*set_name)(struct mcr_IRegistry *, struct mcr_Interface *,
					   const char *name);
	//! Add names that will map to an interface.
	int (*add_names)(struct mcr_IRegistry *, struct mcr_Interface *,
					 const char * const*addNames, size_t addNamesCount);
	//! Remove from id list and all mapped name strings.
	void (*remove)(struct mcr_IRegistry *, struct mcr_Interface *);
	//! Remove the name without removing the interface.
	void (*remove_name)(struct mcr_IRegistry *, const char *removeName);
	//! Get the total number of interfaces registered.
	size_t (*count)(const struct mcr_IRegistry *);
	//! Minimize allocation.
	void (*trim)(struct mcr_IRegistry *);
	/*! Remove all registered interfaces. */
	void (*clear)(struct mcr_IRegistry *);
};

/*! Register a new interface.  The interface id will be
 *  set as the next possible id to be registered.  If names are provided, also
 *  \ref mcr_reg_set_name, \ref mcr_reg_add_names
 *
 *  \param interfacePt The new interface to register
 *  \param name \ref opt Name to map to and from newType
 *  \param addNames \ref opt Set of additional names to map to newType
 *  \param bufferLen Number of names to map in addNames,
 *  required if addNames is to be used
 *  \return \ref reterr
 */
MCR_API int mcr_register(struct mcr_IRegistry *iRegPt,
						 struct mcr_Interface *interfacePt, const char *name, const char * const*addNames,
						 size_t addNamesCount);
/*! Unregister an interface from this registry.
 *
 *  Remove all id's, names, and references to the interface.
 */
MCR_API void mcr_unregister(struct mcr_IRegistry *iRegPt,
							struct mcr_Interface *interfacePt);

/*! \ref ctor
 *
 *  \param regPt \ref opt
 */
MCR_API void mcr_IRegistry_init(struct mcr_IRegistry *iRegPt);
/*! \ref dtor
 *
 *  \param regPt \ref opt
 *  \return \ref reterr
 */
MCR_API void mcr_IRegistry_deinit(struct mcr_IRegistry *iRegPt);
MCR_API size_t mcr_IRegistry_id(const struct mcr_IRegistry *iRegPt,
								struct mcr_Interface *interfacePt);
/*! Get a reference to the interface of the same id.
 *
 *  \param iRegPt \ref opt
 *  \param typeId Id of interface
 *  \return Interface of given id, or null if not found
 */
MCR_API struct mcr_Interface *mcr_IRegistry_from_id(const struct mcr_IRegistry
		*iRegPt, size_t typeId);
MCR_API const char *mcr_IRegistry_name(const struct mcr_IRegistry *iRegPt,
									   struct mcr_Interface *interfacePt);
/*! Get a reference to the interface from its registered name.
 *
 *  \param iRegPt \ref opt
 *  \param typeName \ref opt Name of interface
 *  \return Interface of given name, or null if not found
 */
MCR_API struct mcr_Interface *mcr_IRegistry_from_name(const struct mcr_IRegistry
		*iRegPt, const char *name);
/*! Get the name of the interface of given id, or null if
 *  not found.
 *
 *  \param iRegPt \ref opt
 *  \param id Id of the interface
 *  \return Name of the interface, or null if not found
 */
MCR_API const char *mcr_IRegistry_id_name(const struct mcr_IRegistry *iRegPt,
		size_t id);
/*! Get the id of the interface of given name.
 *
 *  \param iRegPt \ref opt
 *  \param name Name of the interface
 *  \return \ref retid
 */
MCR_API size_t mcr_IRegistry_name_id(const struct mcr_IRegistry *iRegPt,
									 const char *name);
/*! Map a name to an interface, and the interface to that name.
 *
 *  \param interfacePt \ref opt Interface to map
 *  \param name \ref opt Name to map
 *  \return \ref reterr
 */
MCR_API int mcr_IRegistry_set_name(struct mcr_IRegistry *iRegPt,
								   struct mcr_Interface *interfacePt, const char *name);
/*! Add a mapping from name to interface.
 *
 *  \param interfacePt Interface to map to
 *  \param name \ref opt Name to map from
 *  \return \ref reterr
 */
MCR_API int mcr_IRegistry_add_name(struct mcr_IRegistry *iRegPt,
								   struct mcr_Interface *interfacePt, const char *name);
/*! \ref mcr_reg_add_name for all given names
 *
 *  \param interfacePt Interface to map to
 *  \param names \ref opt Array of names to map from
 *  \param bufferLen Number of elements to use in names
 *  \return \ref reterr
 */
MCR_API int mcr_IRegistry_add_names(struct mcr_IRegistry *iRegPt,
									struct mcr_Interface *interfacePt, const char * const*addNames, size_t addNamesCount);
/*! Get the number of registered interfaces.
 *
 *  \param iRegPt
 *  \return \ref retind
 */
MCR_API size_t mcr_IRegistry_count(const struct mcr_IRegistry *iRegPt);
/*! Minimize allocation */
MCR_API void mcr_IRegistry_trim(struct mcr_IRegistry *iRegPt);
/*! Remove all registered interfaces. */
MCR_API void mcr_IRegistry_clear(struct mcr_IRegistry *iRegPt);

#ifdef __cplusplus
}
#endif

#endif
