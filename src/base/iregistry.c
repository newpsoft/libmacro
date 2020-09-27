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

#include "mcr/base/base.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int mcr_register(struct mcr_IRegistry *iRegPt,
				 struct mcr_Interface *interfacePt,
				 const char *name, const char * const*addNames, size_t addNamesCount)
{
	if (mcr_IRegistry_set_name(iRegPt, interfacePt, name))
		return mcr_err;
	if (addNames && addNamesCount)
		return mcr_IRegistry_add_names(iRegPt, interfacePt, addNames, addNamesCount);
	return 0;
}

void mcr_unregister(struct mcr_IRegistry *iRegPt,
					struct mcr_Interface *interfacePt)
{
	dassert(iRegPt);
	if (iRegPt->remove && interfacePt) {
		iRegPt->remove(iRegPt, interfacePt);
		interfacePt->registry = NULL;
		interfacePt->id = (size_t)~0;
	}
}

void mcr_IRegistry_init(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	memset(iRegPt, 0, sizeof(struct mcr_IRegistry));
}

void mcr_IRegistry_deinit(struct mcr_IRegistry *iRegPt)
{
	if (iRegPt && iRegPt->deinit)
		iRegPt->deinit(iRegPt);
}

size_t mcr_IRegistry_id(const struct mcr_IRegistry *iRegPt,
						struct mcr_Interface *interfacePt)
{
	if (!iRegPt || !interfacePt)
		return (size_t)~0;
	dassert(iRegPt->id);
	return iRegPt->id(iRegPt, interfacePt);
}

struct mcr_Interface *mcr_IRegistry_from_id(const struct mcr_IRegistry *iRegPt,
		size_t typeId)
{
	if (!iRegPt || typeId == (size_t) ~0)
		return NULL;
	dassert(iRegPt->from_id);
	return iRegPt->from_id(iRegPt, typeId);
}

const char *mcr_IRegistry_name(const struct mcr_IRegistry *iRegPt,
							   struct mcr_Interface *interfacePt)
{
	if (!iRegPt || !interfacePt)
		return NULL;
	dassert(iRegPt->name);
	return iRegPt->name(iRegPt, interfacePt);
}

struct mcr_Interface *mcr_IRegistry_from_name(const struct mcr_IRegistry
		*iRegPt,
		const char *name)
{
	if (!iRegPt || !name)
		return NULL;
	dassert(iRegPt->from_name);
	return iRegPt->from_name(iRegPt, name);
}

const char *mcr_IRegistry_id_name(const struct mcr_IRegistry *iRegPt, size_t id)
{
	struct mcr_Interface *interfacePt = mcr_IRegistry_from_id(iRegPt, id);
	if (interfacePt) {
		dassert(iRegPt->name);
		return iRegPt->name(iRegPt, interfacePt);
	}
	return NULL;
}

size_t mcr_IRegistry_name_id(const struct mcr_IRegistry *iRegPt,
							 const char *name)
{
	struct mcr_Interface *interfacePt = mcr_IRegistry_from_name(iRegPt, name);
	if (interfacePt) {
		dassert(iRegPt->id);
		return iRegPt->id(iRegPt, interfacePt);
	}
	return (size_t)~0;
}

int mcr_IRegistry_set_name(struct mcr_IRegistry *iRegPt,
						   struct mcr_Interface *interfacePt, const char *name)
{
	size_t id;
	dassert(iRegPt);
	dassert(interfacePt);
	dassert(name);
	dassert(iRegPt->set_name);

	if ((id = iRegPt->set_name(iRegPt, interfacePt, name)) == (size_t)~0)
		return mcr_err;
	/* Registry did not set interface id is invalid */
	if (interfacePt->id != id) {
		printf("Info: mcr_IRegistry did not set interface id. Setting id to %llu\n", (long long unsigned)id);
		interfacePt->id = id;
	}
	interfacePt->registry = iRegPt;
	return 0;
}

int mcr_IRegistry_add_name(struct mcr_IRegistry *iRegPt,
						   struct mcr_Interface *interfacePt,
						   const char *name)
{
	return mcr_IRegistry_add_names(iRegPt, interfacePt, &name, 1);
}

int mcr_IRegistry_add_names(struct mcr_IRegistry *iRegPt,
							struct mcr_Interface *interfacePt,
							const char * const*addNames, size_t addNamesCount)
{
	dassert(iRegPt);
	dassert(interfacePt);
	if (!addNames || !addNamesCount)
		return 0;
	dassert(iRegPt->add_names);
	return iRegPt->add_names(iRegPt, interfacePt, addNames, addNamesCount);
}

size_t mcr_IRegistry_count(const struct mcr_IRegistry * iRegPt)
{
	return iRegPt ? iRegPt->count(iRegPt) : 0;
}

void mcr_IRegistry_trim(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	if (iRegPt->trim)
		iRegPt->trim(iRegPt);
}

void mcr_IRegistry_clear(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	dassert(iRegPt->clear);
	if (iRegPt->clear)
		iRegPt->clear(iRegPt);
}
