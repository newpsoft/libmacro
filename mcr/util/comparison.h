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
 *  \brief Comparison functions to be used with qsort and bsearch
 */

#ifndef MCR_UTIL_COMPARISON_H_
#define MCR_UTIL_COMPARISON_H_

#include "mcr/globals.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Function on data or an object.
 *
 *  Use this for data init and deinit, such as \ref mcr_Interface.init
 *  and \ref mcr_Interface.deinit
 *  \param dataPt \ref mcr_DataMember.data
 *  \return \ref reterr
 */
typedef int (*mcr_data_fnc)(void *dataPt);
/*! Copy from one data or object another
 *
 *  \param destinationPt Destination object
 *  \param sourcePt \ref opt Source object
 *  \return \ref reterr
 */
typedef int (*mcr_copy_fnc)(void *destinationPt, const void *sourcePt);
/*! Comparison function of two data or objects
 *
 *  \param lhsPt \ref opt Left side of comparison
 *  \param rhsPt \ref opt Right side of comparison
 *  \return \ref retcmp
 */
typedef int (*mcr_compare_fnc)(const void *lhsPt, const void *rhsPt);

/*! Compare chars
 *
 *  \param lhs \ref opt const char *
 *  \param rhs \ref opt const char *
 *  \return \ref retcmp
 */
MCR_API int mcr_char_compare(const void *lhs, const void *rhs);
/*! Compare size_t
 *
 *  \param lhs \ref opt const size_t *
 *  \param rhs \ref opt const size_t *
 *  \return \ref retcmp
 */
MCR_API int mcr_id_compare(const void *lhs, const void *rhs);
/*! Compare const c-strings referenced by each pointer,
 *  case insensitive.
 *
 *  \param lhs \ref opt const char * const* or mcr_Array *
 *  \param rhs \ref opt const char * const* or mcr_Array *
 *  \return \ref retcmp
 */
MCR_API int mcr_name_compare(const void *lhs, const void *rhs);
/*! Compare const c-strings referenced by each pointer,
 *  case sensitive.
 *
 *  \param lhs \ref opt const char * const* or mcr_Array *
 *  \param rhs \ref opt const char * const* or mcr_Array *
 *  \return \ref retcmp
 */
MCR_API int mcr_str_compare(const void *lhs, const void *rhs);
/*! Compare integers referenced by each pointer.
 *
 *  \param lhs \ref opt int *
 *  \param rhs \ref opt int *
 *  \return \ref retcmp
 */
MCR_API int mcr_int_compare(const void *lhs, const void *rhs);
/*! Compare unsigned integers referenced by each pointer.
 *
 *  \param lhs \ref opt unsigned int *
 *  \param rhs \ref opt unsigned int *
 *  \return \ref retcmp
 */
MCR_API int mcr_unsigned_compare(const void *lhs, const void *rhs);
/*! Compare void * referenced by each pointer.
 *
 *  \param lhs \ref opt void **
 *  \param rhs \ref opt void **
 *  \return \ref retcmp
 */
MCR_API int mcr_ref_compare(const void *lhs, const void *rhs);

#ifdef __cplusplus
}
#endif

#endif
