/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref FunctorSignal - Signal wrapping a std::function.
 *
 *  Header-only; not exported via MCR_API. @c std::function never crosses
 *  the shared-library boundary.
 */

#pragma once

#include "mcr/signal.h"

#include <functional>

#ifdef __cplusplus

namespace mcr
{

/**
 * @brief A Signal whose @c send() invokes a std::function.
 *
 *  Useful for one-off signals without creating a named subclass.
 */
class FunctorSignal : public Signal {
public:
	MCR_DECL_INTERFACE(FunctorSignal)

	/** @brief Construct with a callable for @c send(). */
	explicit FunctorSignal(std::function<void()> fn)
		: _fn(std::move(fn))
	{
	}

	/** @brief Get the signal type name. @return "FunctorSignal". */
	virtual const char *name() const
	{
		return "FunctorSignal";
	}

	/** @brief Invoke the wrapped function. */
	virtual void send()
	{
		if (_fn)
			_fn();
	}

private:
	std::function<void()> _fn;
};

}

#endif // __cplusplus
