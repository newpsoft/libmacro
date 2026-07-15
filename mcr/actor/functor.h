/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref FunctorActor - IActor wrapping a std::function.
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
 * @brief An IActor whose @c act() invokes a std::function.
 *
 *  Useful for one-off actors without creating a named subclass.
 */
class FunctorActor : public IActor {
public:
	MCR_DECL_INTERFACE(FunctorActor)

	/** @brief Construct with a callable for @c act(). */
	explicit FunctorActor(std::function<bool()> fn)
		: _fn(std::move(fn))
	{
	}

	/** @brief Invoke the wrapped function.
	 *  @return The wrapped function's return value, or false if empty. */
	virtual bool act()
	{
		return _fn ? _fn() : false;
	}

private:
	std::function<bool()> _fn;
};

}

#endif // __cplusplus
