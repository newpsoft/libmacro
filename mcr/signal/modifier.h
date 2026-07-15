/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref Modifier - Signal to modify internal modifiers
 */

#pragma once

#include "mcr/signal.h"
#include "mcr/types.h"

#ifdef __cplusplus

namespace mcr
{
/**
 * @brief Signal that modifies the active modifier state.
 *
 *  Pairs a set of modifier bit flags with an apply action to set,
 *  release, or toggle them.
 */
class MCR_API Modifier : public Signal {
    public:
	MCR_DECL_INTERFACE(Modifier)

	/** @brief Libmacro context this modifier belongs to. */
	Libmacro *context = nullptr;
	/*! Bit values representing a set of modifiers. */
	unsigned int modifiers = 0;
	/*! @ref MCR_UNSET will set the modifier, @ref MCR_TOGGLE
	*  will change the state from what it currently has, otherwise it is
	*  released */
	enum mcr_ApplyValue apply = MCR_SET;

	/** @brief Construct a modifier signal.
	 *  @param libmacroPtr Libmacro context (default nullptr).
	 */
	Modifier(Libmacro *libmacroPtr = nullptr)
		: Signal()
		, context(libmacroPtr)
	{
	}

	/** @brief Get the signal type name. @return "Modifier". */
	virtual const char *name() const
	{
		return "Modifier";
	}
	/** @brief Send this signal, applying the modifier changes to the context. */
	virtual void send();
};
}
#endif

