/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Exception type for Libmacro runtime errors.
 *
 *  Wraps an integer error code and a C string message. Compatible with
 *  std::exception for use in C++ catch blocks.
 */

#pragma once

#include "mcr/defines.h"

#ifdef __cplusplus

#include <exception>

namespace mcr
{

/**
 * @brief Exception class carrying a numeric error code and message string.
 *
 *  Constructed with an integer code (typically from errno or a library-defined
 *  constant) and an optional human-readable message. The code() and what()
 *  accessors retrieve these values.
 */
class MCR_API Error : public std::exception {
    public:
	/** @brief Construct with an error code and optional message.
	 *  @param code Numeric error code.
	 *  @param message Descriptive message (default empty).
	 */
	explicit Error(int code, const char *message = "")
		: _code(code)
		, _message(message)
	{
	}

	virtual ~Error() = default;

	/** @brief Get the numeric error code.
	 *  @return The error code passed at construction.
	 */
	int code() const noexcept
	{
		return _code;
	}

	/** @brief Get the human-readable error message.
	 *  @return Pointer to the message string.
	 */
	virtual const char *what() const noexcept override
	{
		return _message;
	}

    private:
	int _code;
	const char *_message;
};

}

#endif


