/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Case-insensitive string comparison functor for STL containers.
 */

#ifndef __cplusplus
#pragma message "C++ is required.."
#include "mcr/err.h"
#endif

#pragma once

#include <cctype>
#include <string>

namespace mcr
{
/**
 * @brief Functor providing case-insensitive less-than comparison for std::string.
 *
 *  Use as the comparator for std::map or std::set to create case-insensitive
 *  string-keyed containers.
 */
struct string_less_ci {
	/** @brief Character comparison using tolower for case-insensitive less-than. */
	struct nocase_compare {
		bool operator()(const unsigned char &c1,
				const unsigned char &c2) const
		{
			return tolower(c1) < tolower(c2);
		}
	};
	/** @brief Character comparison using tolower for case-insensitive equality. */
	struct nocase_equal {
		bool operator()(const unsigned char &c1,
				const unsigned char &c2) const
		{
			return tolower(c1) == tolower(c2);
		}
	};
	/** @brief Compare two strings case-insensitively.
	 *  @param s1 Left string.
	 *  @param s2 Right string.
	 *  @return true if s1 is lexicographically less than s2 (case-insensitive).
	 */
	bool operator()(const std::string &s1, const std::string &s2) const
	{
		return std::lexicographical_compare(s1.begin(), s1.end(),
						    s2.begin(), s2.end(),
						    nocase_compare());
	}
};
} // namespace mcr

