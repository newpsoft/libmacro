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
 *  \brief Library-wide and utility definitions
 */

#ifndef MCR_DEFINES_H_
#define MCR_DEFINES_H_

#ifdef __cplusplus
#include <cerrno>
#include <ctime>
#include <cstdbool>
#include <cstdint>
#ifdef MCR_DEBUG
	#include <cassert>
#endif
extern "C" {
#else
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef MCR_DEBUG
	#include <assert.h>
#endif
#endif

/* Doxygen in-doxy-comment references */
#ifndef reterr
	/*! Return 0 for success, otherwise standard error code
	*   from \c errno.h.
	*/
	#define reterr int
	#undef reterr
#endif

#ifndef retind
	/*! Return size_t index, or -1 on error. */
	#define retind size_t
	#undef retind
#endif

#ifndef retid
	/*! Return size_t id, or -1 on error. */
	#define retid size_t
	#undef retid
#endif

#ifndef retcmp
	/*! Return 0 if comparison is equal, less than 0 if left < right,
	*   or greater than 0 if left > right.
	*/
	#define retcmp int
	#undef retcmp
#endif

#ifndef opt
	/*! Optional parameter */
	#define opt
	#undef opt
#endif

#ifndef ctor
	/*! This function is a constructor. Initialize members and resources.
	*   \pre Only call once. The referenced object to initialize must not
	*   already be initialized.
	*/
	#define ctor
	#undef ctor
#endif
#ifndef dtor
	/*! This function is a deconstructor. Deinitialize members and resources.
	*   \post Resources will be freed and object may be reinitialized to be
	*   used again.
	*/
	#define dtor
	#undef dtor
#endif

#ifndef mcr_cpp_only
	/*! Only use with C++. */
	#define mcr_cpp_only
	#undef mcr_cpp_only
#endif

#ifndef mcr_platform
	/*! Required to be re-defined for each platform */
	#define mcr_platform
	#undef mcr_platform
#endif

#ifndef mcr_future
	/*! Not currently implemented, or not yet useful, but it will be. */
	#define mcr_future
	#undef mcr_future
#endif

#ifndef MCR_PLATFORM_DEFINES_H
	/*! If defined, this file of platform definitions will always be included
	 *  here.  Must be a quote-enclosed string. */
	#define MCR_PLATFORM_DEFINES_H
	#undef MCR_PLATFORM_DEFINES_H
#endif

/* Library export */
#ifdef MCR_PLATFORM_WINDOWS
	#define MCR_EXPORT __declspec(dllexport)
	#define MCR_IMPORT __declspec(dllimport)
#else
	#define MCR_EXPORT __attribute__((visibility("default")))
	#define MCR_IMPORT __attribute__((visibility("default")))
#endif

#ifndef MCR_API
	#ifdef MCR_STATIC
		#define MCR_API
	#else
		#ifdef LIBMACRO_LIBRARY
			#define MCR_API MCR_EXPORT
		#else
			#define MCR_API MCR_IMPORT
		#endif
	#endif
#endif

#ifndef MCR_INLINE
	#ifdef _MSC_VER
		/*! Inline this C function.  Suggested only to use this with static functions
		*   in .c files. */
		#define MCR_INLINE __inline
	#else
		#define MCR_INLINE inline
	#endif
#endif


/* Rest of file is helpers and finally the platform definitions. */


#define MCR_STR_HELPER(x) #x
/*! Stringify, or create a string.
 *  e.g. MCR_STR(myString) => "myString"
 */
#define MCR_STR(x) MCR_STR_HELPER(x)

#define MCR_EXP_HELPER(x) x
/*! Expand macros: e.g. MCR_EXP(myMacro) => myMacroExpanded */
#define MCR_EXP(x) MCR_EXP_HELPER(x)

#define MCR_TOKEN_HELPER(x, y) x ##y
#define MCR_SNAKE_HELPER(x, y) x ##_ ##y
/*! MCR_TOKEN(MyString, MyString2) => MyStringMyString2 */
#define MCR_TOKEN(x, y) MCR_TOKEN_HELPER(x, y)
/*! MCR_SNAKE(myString, myString2) => myString_myString2 */
#define MCR_SNAKE(x, y) MCR_SNAKE_HELPER(x, y)

#ifndef MCR_LINE
	/*! Constant string "file(line)" */
	#define MCR_LINE __FILE__ "(" MCR_STR(__LINE__) ")"
#endif

#ifndef MCR_PLATFORM
	#define MCR_PLATFORM none
	#pragma message "Warning: MCR_PLATFORM is not defined.  Please provide MCR_PLATFORM as a preprocessor definition for your platform."
	#include "mcr/warn.h"
#endif
#ifndef UNUSED
	/*! Remove compiler warning for unused variable. */
	#define UNUSED(x) ((void)(x))
#endif
#ifndef MCR_CMP
	/*! Compare integral types.  Casting must be done manually.
	*
	*   \param lhs Left side
	*   \param rhs Right side
	*   \return \ref retcmp
	*/
	#define MCR_CMP(lhs, rhs) (((lhs) > (rhs)) - ((lhs) < (rhs)))
#endif
#ifndef MCR_CMP_CAST
	/*! Integral comparison with built-in casting
	*
	*  \param casting Prefix used to cast variables before comparison
	*  \param lhs Left side
	*  \param rhs Right side
	*  \return \ref retcmp
	*/
	#define MCR_CMP_CAST(casting, lhs, rhs) MCR_CMP(casting(lhs), casting(rhs))
#endif
#ifndef MCR_CMP_PTR
/*! Integral comparison of pointers with built-in casting
*
*  If either pointer is null the pointer addresses are compared.
*  \param T Native type each pointer is pointing to.  e.g. if comparing
*  pointers to integers T should be \c int.  If either pointer is const
*  then T must also be const.  e.g. \c const \c int
*  \param lhsPtr Pointer to left side comparison
*  \param rhsPtr Pointer to right side comparison
*  \return \ref retcmp
*/
#define MCR_CMP_PTR(T, lhsPtr, rhsPtr) (lhsPtr && rhsPtr ? \
		MCR_CMP(*mcr_castpt(T, lhsPtr), *mcr_castpt(T, rhsPtr)) : \
		MCR_CMP(lhsPtr, rhsPtr))
#endif

#ifndef MCR_OFFSET
/*! Get the bytes offset of a member of a struct.
 *
 *  \param structDecl Type
 *  \param memberName Name of member
 *  \param outSizeVariable size_t Byte size offset output
 */
#define MCR_OFFSET(structDecl, memberName, outSizeVariable) \
{ \
	structDecl *__internalStruct = mcr_null; \
	outSizeVariable = mcr_cast(size_t, &(__internalStruct).memberName); \
}
#endif

//! \todo \ref mcr_cast Not required if using functions instead of defines.
#ifndef mcr_cast
	#ifdef __cplusplus
		/*! static_cast for C++ and regular cast for C.  Macros are not affected
		*   by extern "C". Designed for macros C/C++ interoperability. */
		#define mcr_cast(T, obj) static_cast<T>(obj)
		/*! reinterpret_cast as a pointer for C++ and regular cast for C.
		*   Macros are not affected by extern "C". Designed for macros C/C++
		*   interoperability. */
		#define mcr_castpt(T, obj) reinterpret_cast<T *>(obj)
		/*! nullptr for C++ and NULL for C */
		#define mcr_null nullptr
	#else
		#define mcr_cast(T, obj) ((T)(obj))
		#define mcr_castpt(T, obj) ((T *)(obj))
		#define mcr_null NULL
	#endif
#endif
/* Debugging definitions */
#ifndef mcr_ddo
	#ifdef MCR_DEBUG
		/*! Do the argument only for debug builds. */
		#define mcr_ddo(stuff) stuff
	#else
		/* Empty blocks.  Non-empty interferes with one-liners. */
		#define mcr_ddo(stuff) {}
	#endif
#endif
/* Do function pointer if defined */
#ifndef mcr_ifdo
#define mcr_ifdo(fnPtr, ...) \
	if (fnPtr) { \
		(fnPtr)(__VA_ARGS__); \
	}
#endif
/* Do function pointer if defined and return result */
#ifndef mcr_ifreturndo
#define mcr_ifreturndo(fnPtr, ...) \
	if (fnPtr) { \
		return (fnPtr)(__VA_ARGS__); \
	}
#endif
#ifndef mcr_dprint
	/*! If debug build, print to stdout. */
	#define mcr_dprint(...) mcr_ddo(printf(__VA_ARGS__))
#endif
#ifndef mcr_dexit
	/*! Only when debugging exit with \ref mcr_err error code. */
	#define mcr_dexit mcr_ddo({ mcr_dmsg; exit(mcr_err); })
#endif
#ifndef mcr_dmsg
/*! Automated error message of \ref mcr_errno.
 *
 *  Print to \c stderr for only debug builds.
 */
#define mcr_dmsg \
mcr_ddo(fprintf(stderr, "Error %d: " MCR_LINE ", %s: %s.\n", \
		mcr_err, mcr_timestamp(), mcr_error_str(mcr_err)))
#endif
#ifndef mcr_dassert
	/*! Assertion only for debug builds */
	#define mcr_dassert(expression) mcr_ddo(assert(expression))
#endif
#ifndef mcr_arrlen
	/*! For a static array definition get the number of elements.
	*
	*  \param arr Static array
	*  \return size_t Number of elements
	*/
	#define mcr_arrlen(arr) (sizeof(arr) / sizeof(*(arr)))
#endif
#ifndef mcr_errno
/*! Set \ref mcr_err to \ref errno.
 *
 *  Also \ref mcr_dmsg
 *  \param fallbackError If errno is not an error, use this error code.
 */
#define mcr_errno(fallbackError) { \
	mcr_err = errno; \
	if (!mcr_err) \
		mcr_err = (fallbackError); \
	mcr_dmsg; \
}
#endif
#ifndef mcr_error_return
/*! Immediately \ref mcr_dmsg and return \ref mcr_err.
 *
 *  Only use in a function that returns \ref reterr
 */
#define mcr_error_return { \
	mcr_dmsg; \
	return mcr_err; \
}
#endif
#ifndef mcr_error_set
/*! Set \ref mcr_err and \ref mcr_dmsg */
#define mcr_error_set(errorNumber) { \
	mcr_err = (errorNumber); \
	mcr_dmsg; \
}
#endif
#ifndef mcr_error_set_return
/*! Set \ref mcr_err, \ref mcr_dmsg and return \ref mcr_err.
 *
 *  Only use in a function that returns \ref reterr
 */
#define mcr_error_set_return(errorNumber) { \
	mcr_err = (errorNumber); \
	mcr_dmsg; \
	return errorNumber; \
}
#endif

#ifndef ZERO
	#define ZERO(inst) memset(&(inst), 0, sizeof(inst))
#endif
#ifndef ZERO_PTR
	#define ZERO_PTR(ptr) memset(ptr, 0, sizeof(*(ptr)))
#endif

#ifndef dmsg
	/*! \ref mcr_dmsg */
	#define dmsg mcr_dmsg
#endif
#ifndef dassert
	/*! \ref mcr_dassert */
	#define dassert(expr) mcr_dassert(expr)
#endif
#ifndef arrlen
	/*! \ref mcr_arrlen */
	#define arrlen(arr) mcr_arrlen(arr)
#endif
#ifndef mset_error
	/*! \ref mcr_error_set */
	#define mset_error(errorNumber) mcr_error_set(errorNumber)
#endif
#ifndef error_set_return
	/*! \ref mcr_error_return */
	#define error_set_return(errorNumber) mcr_error_set_return(errorNumber)
#endif

/*! Any or invalid modifier */
#define MCR_MOD_ANY ((unsigned int) ~0)

#ifndef MCR_THREAD_MAX
	/*! Maximum thread count for macros
	*
	*  For better security macros will never create more than this number of
	*  threads
	*/
	#define MCR_THREAD_MAX 0x10
#endif

#ifndef MCR_MACRO_JOIN_TIMEOUT
	/*! Seconds to wait for macros to respond when they are being
	*  interrupted or paused
	*/
	#define MCR_MACRO_JOIN_TIMEOUT 2
#endif

#ifndef MCR_MAX_PAUSE_COUNT
	/*! The number of times a macro will wait while paused until it resumes
	*/
	#define MCR_MAX_PAUSE_COUNT 5
#endif

/*! Any or invalid echo code */
#define MCR_HIDECHO_ANY mcr_cast(size_t, ~0)
/*! Any or invalid key code */
#define MCR_KEY_ANY mcr_cast(int, 0)

#ifndef mcr_throwif
/*! If true, print a file+line message and throw the error.
*
*  \ref mcr_cpp_only
*  Will not set mcr_err, as thrown errors should be handled with catch.
*  Only use in C+, because of throw.
*/
#define mcr_throwif(condition, errorNumber) \
	if (condition) { \
		mcr_dassert(condition); \
		::mcr::throwError(MCR_LINE, errorNumber); \
	}
#endif

#ifndef mcr_assert_receive_function
/*! mcr_DispatchReceiver must have a valid receiver function, and known
 *  receiver functions also require an object to receive.
 *  \ref mcr_cpp_only
 */
#define mcr_assert_receive_function(receiver, receiveFnc) \
	mcr_throwif(!receiveFnc, EFAULT); \
	mcr_throwif((receiverFnc == mcr_Trigger_receive \
				 || receiverFnc == mcr_Macro_receive) && !receiver, EINVAL);
#endif

/*! \ref mcr_cpp_only */
#define MCR_DECL_INTERFACE(className) \
	className() = default; \
	className(const className &) = default; \
	virtual ~className() = default; \
	className &operator =(const className &) = default;

#ifdef __cplusplus
}
#endif

#ifdef MCR_PLATFORM_DEFINES_H
#include MCR_PLATFORM_DEFINES_H
#endif
#ifndef MCR_PLATFORM_H
#define MCR_PLATFORM_H MCR_STR(mcr/MCR_PLATFORM/p_libmacro.h)
#endif

#endif
