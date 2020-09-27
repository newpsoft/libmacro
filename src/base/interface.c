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
#include <stdlib.h>
#include <string.h>

static MCR_INLINE void apply_deallocator(const struct mcr_Interface
		*interfacePt,
		struct mcr_DataMember *dataMemberPt)
{
	if (dataMemberPt->data) {
		dataMemberPt->deallocate = interfacePt->deallocate ? interfacePt->deallocate :
								   free;
		dataMemberPt->deinit = interfacePt->deinit ? interfacePt->deinit : NULL;
	}
}

void mcr_Interface_init(struct mcr_Interface *interfacePt)
{
	if (interfacePt) {
		ZERO_PTR(interfacePt);
		interfacePt->id = (size_t)~0;
	}
}

void mcr_Interface_ctor(struct mcr_Interface *interfacePt,
						struct mcr_context *context,
						size_t dataSize, void *(*allocate)(), void (*deallocate)(void *),
						mcr_data_fnc init, mcr_data_fnc deinit, mcr_compare_fnc compare,
						mcr_copy_fnc copy)
{
	if (interfacePt) {
		*interfacePt = (struct mcr_Interface) {
			.id = (size_t)~0,
			.context = context,
			.data_size = dataSize,
			.allocate = allocate,
			.deallocate = deallocate,
			.init = init,
			.deinit = deinit,
			.compare = compare,
			.copy = copy
		};
	}
}

void mcr_Interface_deinit(struct mcr_Interface *interfacePt)
{
	if (interfacePt && interfacePt->registry)
		mcr_unregister(interfacePt->registry, interfacePt);
}

void mcr_Interface_set_all(struct mcr_Interface *interfacePt,
						   struct mcr_context *context,
						   size_t dataSize, void *(*allocate)(), void (*deallocate)(void *),
						   mcr_data_fnc init, mcr_data_fnc deinit, mcr_compare_fnc compare,
						   mcr_copy_fnc copy)
{
	dassert(interfacePt);
	interfacePt->context = context;
	interfacePt->data_size = dataSize;
	interfacePt->allocate = allocate;
	interfacePt->deallocate = deallocate;
	interfacePt->init = init;
	interfacePt->deinit = deinit;
	interfacePt->compare = compare;
	interfacePt->copy = copy;
}

size_t mcr_Interface_id(struct mcr_Interface *interfacePt)
{
	return interfacePt ? interfacePt->id : (size_t)~0;
}

int mcr_Interface_allocate(const struct mcr_Interface *interfacePt,
						   struct mcr_DataMember *dataMemberPt)
{
	int err;
	if (!dataMemberPt)
		return 0;
	mcr_err = 0;
	mcr_DataMember_destroy(dataMemberPt);
	if (!interfacePt)
		return mcr_err;
	if (interfacePt->allocate) {
		dataMemberPt->data = interfacePt->allocate();
		apply_deallocator(interfacePt, dataMemberPt);
	} else {
		dassert(interfacePt->data_size);
		if ((dataMemberPt->data = malloc(interfacePt->data_size))) {
			apply_deallocator(interfacePt, dataMemberPt);
			if (interfacePt->init) {
				if ((err = interfacePt->init(dataMemberPt->data)))
					error_set_return(err);
			} else {
				memset(dataMemberPt->data, 0, interfacePt->data_size);
			}
		}
	}
	if (!dataMemberPt->data)
		error_set_return(ENOMEM);
	return mcr_err;
}

int mcr_Interface_deallocate(const struct mcr_Interface *interfacePt,
							 struct mcr_DataMember *dataMemberPt)
{
	UNUSED(interfacePt);
	if (!dataMemberPt)
		return 0;
	mcr_err = 0;
	mcr_DataMember_destroy(dataMemberPt);
	return mcr_err;
}

int mcr_Interface_compare(const struct mcr_Interface *interfacePt,
						  const struct mcr_DataMember *lhs,
						  const struct mcr_DataMember *rhs)
{
	bool lFlag = lhs && lhs->data, rFlag = rhs && rhs->data;
	dassert(interfacePt);
	if (lFlag && rFlag) {
		mcr_ifreturndo(interfacePt->compare, lhs->data, rhs->data);
		dassert(interfacePt->data_size);
		return memcmp(lhs->data, rhs->data, interfacePt->data_size);
	}
	return MCR_CMP(lFlag, rFlag);
}

int mcr_Interface_copy(const struct mcr_Interface *interfacePt,
					   struct mcr_DataMember *dstPt,
					   const struct mcr_DataMember *srcPt)
{
	dassert(interfacePt);
	dassert(dstPt);
	if (srcPt) {
		/* Same object */
		if (srcPt->data == dstPt->data)
			return 0;
		if (srcPt->data) {
			mcr_err = 0;
			/* Create an object to copy into, if it does not exist */
			if (!dstPt->data && mcr_Interface_allocate(interfacePt, dstPt))
				return mcr_err;
			mcr_ifreturndo(interfacePt->copy, dstPt->data, srcPt->data);
			dassert(interfacePt->data_size);
			memcpy(dstPt->data, srcPt->data, interfacePt->data_size);
			return 0;
		}
	}
	/* No source, only free */
	return mcr_Interface_deallocate(interfacePt, dstPt);
}

int mcr_Interface_set_data(const struct mcr_Interface *interfacePt,
						   struct mcr_DataMember *dataMemberPt,
						   void *data, bool heapFlag)
{
	dassert(interfacePt);
	if (!dataMemberPt)
		return 0;
	if (!mcr_Interface_deallocate(interfacePt, dataMemberPt)) {
		mcr_DataMember_ctor(dataMemberPt, data,
							heapFlag ? interfacePt->deallocate : NULL, interfacePt->deinit);
	}
	return mcr_err;
}

int mcr_Interface_reset(const struct mcr_Interface *interfacePt,
						struct mcr_DataMember *dataMemberPt)
{
	int err;
	dassert(interfacePt);
	if (!dataMemberPt)
		return 0;
	mcr_err = 0;
	if (dataMemberPt->data) {
		if (interfacePt->deinit && (err = interfacePt->deinit(dataMemberPt->data)))
			error_set_return(err);
		if (interfacePt->init && (err = interfacePt->init(dataMemberPt->data)))
			error_set_return(err);
	} else {
		mcr_Interface_allocate(interfacePt, dataMemberPt);
	}
	return mcr_err;
}
