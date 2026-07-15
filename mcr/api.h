/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Public C API to interact with Libmacro. All library consumers
 *  reference this file.
 */

#pragma once

#include "mcr/defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Flag Manipulation Helpers */

/**
 * @brief Combines an array of binary flags into a single unsigned integer.
 *
 * This function performs a bitwise OR operation across all specified flag values.
 *
 * @param flagsArray A constant pointer to the array of flag integers.
 * @param length The number of flags present in @ref flagsArray to combine.
 * @return An unsigned integer representing the union of all input flags.
 */
MCR_API unsigned int mcr_flags_combine(const unsigned int *flagsArray,
				       mcr_index_t length);

/**
 * @brief Combines two distinct sets of flags using a bitwise OR operation.
 *
 * @param flags The first set of flags (the base value).
 * @param newMod The second set of flags to add (modifier).
 * @return A combined unsigned integer where all bits set in either input are set.
 */
MCR_API unsigned int mcr_flags_add(unsigned int flags, unsigned int newMod);

/**
 * @brief Checks if a given set of flags contains all the specified mask bits.
 *
 * Uses bitwise AND to determine subset relationship: (flags & modVal) == modVal.
 *
 * @param flags The primary set of flags being checked.
 * @param modVal The required flag bits that must all be present in @ref flags.
 * @return True if every bit set in @ref modVal is also set in @ref flags; otherwise, false.
 */
MCR_API bool mcr_flags_has(unsigned int flags, unsigned int modVal);

/**
 * @brief Removes specified flag bits from a base set using XOR or AND logic appropriate for removal.
 *
 * This operation ensures that any bit set in both @ref flags and @ref delMod results in 0 in the output.
 *
 * @param flags The original set of flags from which to remove bits.
 * @param delMod The mask containing flags that should be removed (zeroed out).
 * @return A new unsigned integer representing the original flags with the specified bits cleared.
 */
MCR_API unsigned int mcr_flags_remove(unsigned int flags, unsigned int delMod);

#ifdef __cplusplus
}
#endif


