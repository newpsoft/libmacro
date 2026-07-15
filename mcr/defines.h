/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Library-wide and utility definitions
 *
 *  This is central point for definitions, macros, and platform-specific
 *  include guards. It mixes several concerns: boilerplate includes,
 *  type definitions, global constants, platform detection/inclusion, macro
 *  wrappers, utility helpers, and C++ class declarations.
 *  @todo No Includes. Compilers don't want STD includes here.
 */

#pragma once

// --- 1. Core Includes ---
#ifdef __cplusplus
#include <cerrno>
#include <cstddef>
#else
#include <stdbool.h>
#include <stddef.h>
#endif

/* Primitive types */
/*! size_t index, or -1 on error. */
typedef size_t mcr_index_t;
/*! size_t id, or -1 on error. */
typedef size_t mcr_id_t;

// --- 2. Global Constants & Limits ---

/*! Amount of time, in milliseconds, to wait for mutex locks.
 *  This only affects trylock variants. Regular mutex locks will still
 *  attempt to lock indefinitely. */
#ifndef MCR_LOCK_TIMEOUT_MILLIS
#define MCR_LOCK_TIMEOUT_MILLIS 2
#endif

/*! Maximum thread count for macros.
 *
 *  For better security macros will never create more than this number of
 *  threads.
 */
#ifndef MCR_THREAD_MAX
#define MCR_THREAD_MAX 0x10
#endif

/*! Seconds to wait for macros to respond when they are being
 *  interrupted or paused.
 */
#ifndef MCR_MACRO_JOIN_TIMEOUT
#define MCR_MACRO_JOIN_TIMEOUT 2
#endif

/*! The number of times a macro will wait while paused until it resumes. */
#ifndef MCR_MAX_PAUSE_COUNT
#define MCR_MAX_PAUSE_COUNT 5
#endif

// --- 3. Platform Definition Block ---

#ifndef MCR_EXPORT
/*! @brief @ref mcr_platform Defined in @ref MCR_PLATFORM_DEFINES_H */
#define MCR_EXPORT
#undef MCR_EXPORT
#endif

#ifndef MCR_IMPORT
/*! @brief @ref mcr_platform Defined in @ref MCR_PLATFORM_DEFINES_H */
#define MCR_IMPORT
#undef MCR_IMPORT
#endif

/*! Required and defined externally. */
#ifndef MCR_PLATFORM
/*! @todo UNUSED, use platform headers instead. */
#define MCR_PLATFORM none
#pragma message \
	"Warning: MCR_PLATFORM is not defined.  Please provide MCR_PLATFORM as a preprocessor definition for your platform."
#include "mcr/warn.h"
#endif

/*! This file of platform definitions will always be included
 *  here.  Must be a quote-enclosed string.
 *  Default: "mcr/none/p_defines.h" */
#ifndef MCR_PLATFORM_DEFINES_H
#define MCR_PLATFORM_DEFINES_H "mcr/none/p_defines.h"
#endif

#include MCR_PLATFORM_DEFINES_H

// Compiler-specific macro definitions for symbol visibility
#if !defined(MCR_EXPORT) && !defined(MCR_IMPORT)
/*! Microsoft Visual Studio uses __declspec, while GCC/Clang use attributes. */
#if defined(_MSC_VER)
// Windows platforms (MSVC typically, but may include MinGW with Clang/GCC toolschain)
#define MCR_EXPORT __declspec(dllexport)
#define MCR_IMPORT __declspec(dllimport)
#else // All other known compilers use this format.
// Fallback visibility attributes - assume GCC/Clang defaults if other compilers aren't identified.
#define MCR_EXPORT __attribute__((visibility("default")))
#define MCR_IMPORT __attribute__((visibility("default")))
#endif
#endif /* End of Visibility Attribute Handling */

/*! API Visibility Selectors. */
#ifndef MCR_API
#ifdef MCR_STATIC
#define MCR_API
#else
#ifdef LIBMACRO_LIBRARY
#define MCR_API MCR_EXPORT
#define MCR_EXTERN
#else
#define MCR_API MCR_IMPORT
#define MCR_EXTERN extern
#endif
#endif
#endif

/*! Inline Attribute. */
#ifndef MCR_INLINE
#ifdef _MSC_VER
/*! Inline this C function.  Suggested only to use this with static functions
		*   in .c files. */
#define MCR_INLINE __inline
#else
#define MCR_INLINE inline
#endif
#endif

// --- 4. Utility Types and Macros ---

/*! Return Type Definitions (Replacing macros like reterr, retind etc.). */
#ifndef impl
/*! Template or otherwise not implemented function placeholder. */
#define impl
#undef impl
#endif

#ifndef reterr
/*! Return 0 for success, otherwise standard error code
	*   from @c errno.h. */
#define reterr int
#undef reterr
#endif

#ifndef retind
/*! Return size_t id, or -1 on error. */
#define retind mcr_index_t
#undef retind
#endif

#ifndef retid
/*! Return size_t id, or -1 on error. */
#define retid mcr_id_t
#undef retid
#endif

#ifndef retcmp
/*! Return 0 if comparison is equal, less than 0 if left < right,
	*   or greater than 0 if left > right. */
#define retcmp int
#undef retcmp
#endif

/*! General Helpers and Utility Macros. */

/*! Return the number of elements in a C array. */
#ifndef mcr_arrlen
#define mcr_arrlen(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef opt
/*! Optional parameter */
#define opt
#undef opt
#endif

#ifdef __cplusplus
/** @brief Compare two values and return -1, 0, or 1.
 *  @tparam T Comparable type.
 *  @param lhs Left side.
 *  @param rhs Right side.
 *  @return -1 if lhs < rhs, 0 if equal, 1 if lhs > rhs.
 */
template<typename T>
constexpr int mcr_cmp(const T &lhs, const T &rhs) noexcept
{
	return (lhs > rhs) - (lhs < rhs);
}
#else
/*! Comparison Macro. */
#ifndef MCR_CMP
/*! Compare integral types.  Casting must be done manually.
	*
	*   @param lhs Left side
	*   @param rhs Right side
	*   @return @ref retcmp
	*/
#define MCR_CMP(lhs, rhs) (((lhs) > (rhs)) - ((lhs) < (rhs)))
#endif
#endif

// --- 5. Placeholder/Future Macros ---

/*! Required to be re-defined for each platform. */
#ifndef mcr_platform
#define mcr_platform
#undef mcr_platform
#endif

/*! Not currently implemented, or not yet useful, but it will be. */
#ifndef mcr_future
#define mcr_future
#undef mcr_future
#endif

/*! Optimize this section of code. */
#ifndef mcr_optimize
#define mcr_optimize
#undef mcr_optimize
#endif

// --- 6. C++ Specific Declarations ---
#ifdef __cplusplus

/** @brief Declare default constructors, destructor, and assignment operators.
 *  @param className Class to declare members for.
 */
#ifndef MCR_DECL_INTERFACE
#define MCR_DECL_INTERFACE(className)                      \
	className() = default;                             \
	className(const className &) = default;            \
	className(className &&) = default;                 \
	virtual ~className() = default;                    \
	className &operator=(const className &) = default; \
	className &operator=(className &&) = default;
#endif

/** @brief Declare default destructor and assignment operators (no default constructor).
 *  @param className Class to declare members for.
 */
#ifndef MCR_DECL_ABSTRACT
#define MCR_DECL_ABSTRACT(className)                       \
	className(const className &) = default;            \
	className(className &&) = default;                 \
	virtual ~className() = default;                    \
	className &operator=(const className &) = default; \
	className &operator=(className &&) = default;
#endif

/** @brief Alias for MCR_DECL_ABSTRACT. For POD-like classes with custom constructors.
 *  @param className Class to declare members for.
 */
#ifndef MCR_DECL_POD
#define MCR_DECL_POD(className) MCR_DECL_ABSTRACT(className)
#endif

#endif // __cplusplus

