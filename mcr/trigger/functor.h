/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref FunctorTrigger - Trigger wrapping a std::function for receive().
 *
 *  Header-only; not exported via MCR_API. @c std::function never crosses
 *  the shared-library boundary.
 */

#pragma once

#include "mcr/trigger.h"

#include <functional>

#ifdef __cplusplus

namespace mcr
{

/**
 * @brief A Trigger whose @c receive() invokes a std::function.
 *
 *  Useful for one-off triggers without creating a named subclass.
 *  Set @c actorPtr and/or @c blockingFlag as usual.
 */
class FunctorTrigger : public Trigger {
public:
	MCR_DECL_INTERFACE(FunctorTrigger)

	/** @brief Construct with a callable for @c receive().
	 *  The callable receives (Signal*, unsigned int mods) and returns
	 *  true to block further dispatch. */
	explicit FunctorTrigger(
		std::function<bool(Signal *, unsigned int)> fn)
		: _fn(std::move(fn))
	{
	}

	/** @brief Get the trigger type name. @return "FunctorTrigger". */
	virtual const char *name() const
	{
		return "FunctorTrigger";
	}

	/** @brief Invoke the wrapped function.
	 *  @param signalPtr The dispatched signal.
	 *  @param mods Active modifier flags.
	 *  @return The wrapped function's return value, or trigger() if empty. */
	virtual bool receive(Signal *signalPtr, unsigned int mods)
	{
		if (_fn)
			return _fn(signalPtr, mods);
		return trigger(signalPtr, mods);
	}

private:
	std::function<bool(Signal *, unsigned int)> _fn;
};

}

#endif // __cplusplus
