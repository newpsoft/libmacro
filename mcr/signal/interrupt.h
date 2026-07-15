/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Interrupt signal and interface for pausing or cancelling macros.
 */

#pragma once

#include "mcr/signal.h"

#include <string>

#ifdef __cplusplus

namespace mcr
{
/*!
*/
/**
 * @brief Interface for objects that can be paused, interrupted, or disabled.
 */
class MCR_API IInterrupt {
    public:
	MCR_DECL_INTERFACE(IInterrupt)

	/** @brief Interrupt state values for controlling macro execution. */
	enum Value {
		/*! Able to trigger, or continue currently running */
		CONTINUE = 0,
		/*! Briefly stop sending signals. */
		PAUSE,
		/*! One queued macro from one thread will cancel.
		*  After one is cancelled, CONTINUE is set. */
		INTERRUPT,
		/*! All threads and triggered queue will cancel.
		*  After all are cancelled, CONTINUE is set. */
		INTERRUPT_ALL,
		/*! No longer able to trigger */
		DISABLE
	};

	/** @brief Apply an interrupt value to this object.
	 *  @param type The interrupt action to perform.
	 */
	virtual void interrupt(Value type) = 0;
};

/**
 * @brief Signal that sends an interrupt to a target IInterrupt object.
 *
 *  When sent, calls target->interrupt(value) on the designated target.
 */
class MCR_API Interrupt : public Signal {
    public:
	MCR_DECL_INTERFACE(Interrupt)

	/** @brief The object to interrupt when this signal is sent. */
	IInterrupt *target = nullptr;
	/** @brief Display name of the intended target (reserved for future use). */
	std::string targetName = "All"; // Not used
	/** @brief Interrupt action to apply to the target. */
	IInterrupt::Value value = IInterrupt::Value::INTERRUPT_ALL;

	// virtual int compare(const IS &rhs) const override;
	/** @brief Get the signal type name. @return "Interrupt". */
	virtual const char *name() const override
	{
		return "Interrupt";
	}
	/** @brief Send the interrupt to the target.
	 *  Does nothing if target is nullptr.
	 */
	virtual void send() override
	{
		if (target)
			target->interrupt(value);
	}
};
}
#endif

