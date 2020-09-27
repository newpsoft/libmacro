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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mcr_Instance_init(struct mcr_Instance *instancePt)
{
	if (instancePt)
		ZERO_PTR(instancePt);
}

void mcr_Instance_deinit(struct mcr_Instance *instancePt)
{
	if (instancePt) {
		if (mcr_Instance_clear(instancePt)) {
			dmsg;
			mcr_dexit(mcr_err);
		}
	}
}

size_t mcr_Instance_id(struct mcr_Instance *instancePt)
{
	return instancePt
		   && instancePt->interface ? instancePt->interface->id : (size_t)~0;
}

bool mcr_Instance_heap(struct mcr_Instance *instancePt)
{
	return instancePt ? instancePt->data_member.deallocate : false;
}

int mcr_Instance_set_interface(struct mcr_Instance *instancePt,
							   struct mcr_Interface *interfacePt)
{
	dassert(instancePt);
	if (mcr_Instance_clear(instancePt))
		return mcr_err;
	instancePt->interface = interfacePt;
	return 0;
}

int mcr_Instance_clear(struct mcr_Instance *instancePt)
{
	dassert(instancePt);
	return mcr_Interface_deallocate(instancePt->interface,
									&instancePt->data_member);
}

int mcr_Instance_reset(struct mcr_Instance *instancePt)
{
	dassert(instancePt);
	if (instancePt->interface)
		return mcr_Interface_reset(instancePt->interface, &instancePt->data_member);
	mcr_DataMember_destroy(&instancePt->data_member);
	return 0;
}

int mcr_Instance_compare(const struct mcr_Instance *lhsPt,
						 const struct mcr_Instance *rhsPt)
{
	if (rhsPt && rhsPt->interface) {
		if (lhsPt && lhsPt->interface) {
			if (lhsPt->interface == rhsPt->interface) {
				return mcr_Interface_compare(lhsPt->interface, &lhsPt->data_member,
											 &rhsPt->data_member);
			}
			return MCR_CMP(mcr_Interface_id(lhsPt->interface),
						   mcr_Interface_id(rhsPt->interface));
		}
		return -1;
	}
	return lhsPt && lhsPt->interface;
}

int mcr_Instance_copy(struct mcr_Instance *dstPt,
					  const struct mcr_Instance *srcPt)
{
	dassert(dstPt);
	if (dstPt == srcPt)
		return 0;
	if (!srcPt || !srcPt->interface)
		return mcr_Instance_clear(dstPt);
	/* Interface exists, make sure it is the same one */
	if (mcr_Instance_set_interface(dstPt, srcPt->interface))
		return mcr_err;
	dassert(dstPt->interface == srcPt->interface);
	return mcr_Interface_copy(srcPt->interface, &dstPt->data_member,
							  &srcPt->data_member);
}
