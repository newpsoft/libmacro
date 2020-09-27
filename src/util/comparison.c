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

#include "mcr/libmacro.h"

#include <string.h>

/* Comparison */
int mcr_char_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const char, lhs, rhs);
}

int mcr_id_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(size_t, lhs, rhs);
}

int mcr_name_compare(const void *lhs, const void *rhs)
{
	if (lhs && rhs) {
		return mcr_casecmp(*(const char *const *)lhs, *(const char *const *)rhs);
	}
	return MCR_CMP(lhs, rhs);
}

int mcr_str_compare(const void *lhs, const void *rhs)
{
	if (lhs && rhs)
		return strcmp(*(const char *const *)lhs, *(const char *const *)rhs);
	return MCR_CMP(lhs, rhs);
}

int mcr_int_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const int, lhs, rhs);
}

int mcr_unsigned_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const unsigned int, lhs, rhs);
}

int mcr_ref_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const void *const, lhs, rhs);
}
