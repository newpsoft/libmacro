/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref NoOp - Pause execution in seconds and milliseconds.
 */

#pragma once

#include "mcr/signal.h"

#ifdef __cplusplus

namespace mcr
{
/**
 * @brief Signal that pauses execution for a specified duration.
 *
 *  When sent, sleeps for the configured number of seconds and
 *  milliseconds.
 */
class MCR_API NoOp : public Signal {
    public:
	MCR_DECL_INTERFACE(NoOp)

	/** @brief Seconds to sleep. */
	int seconds = 0;
	/** @brief Additional milliseconds to sleep. */
	int milliseconds = 20;

	/** @brief Get the signal type name. @return "NoOp". */
	virtual const char *name() const
	{
		return "NoOp";
	}
	/** @brief Send this signal, pausing execution for the configured duration. */
	virtual void send();
};
}
#endif

