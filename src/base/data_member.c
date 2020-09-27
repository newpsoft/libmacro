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
#include <string.h>

void mcr_DataMember_ctor(struct mcr_DataMember *dataMemberPt,
						 void *dataPt,
						 void (*deallocateFnc)(void *),
						 mcr_data_fnc deinit)
{
	if (dataMemberPt) {
		dataMemberPt->data = dataPt;
		dataMemberPt->deallocate = deallocateFnc;
		dataMemberPt->deinit = deinit;
	}
}

void mcr_DataMember_set(struct mcr_DataMember *dataMemberPt, void *dataPt,
						void (*deallocateFnc)(void *), mcr_data_fnc deinit)
{
	if (dataMemberPt && dataMemberPt->data != dataPt) {
		mcr_DataMember_destroy(dataMemberPt);
		mcr_DataMember_ctor(dataMemberPt, dataPt,
							deallocateFnc, deinit);
	}
}

bool mcr_DataMember_heap(struct mcr_DataMember *dataMemberPt)
{
	return dataMemberPt ? dataMemberPt->deallocate : false;
}

void mcr_DataMember_destroy(struct mcr_DataMember *dataMemberPt)
{
	if (dataMemberPt) {
		if (dataMemberPt->deinit) {
			if ((mcr_err = dataMemberPt->deinit(dataMemberPt->data))) {
				dmsg;
			}
		}
		mcr_ifdo(dataMemberPt->deallocate, dataMemberPt->data);
		ZERO_PTR(dataMemberPt);
	}
}
