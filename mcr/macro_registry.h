/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Registry interface for mapping string names to IMacro instances.
 */

#ifndef __cplusplus
#pragma message "C++ is required.."
#include "mcr/err.h"
#endif

#pragma once

#include "mcr/signal/interrupt.h"

namespace mcr
{
class IMacro;
/**
 * @brief Registry that maps string names to IMacro instances for lookup and interrupt control.
 */
class MCR_API IMacroRegistry : public IInterrupt {
    public:
	class MCR_API Deleter {
	    public:
		void operator()(IMacroRegistry *ptr) const;
	};
	MCR_DECL_INTERFACE(IMacroRegistry)

	/** @brief Apply an interrupt to all registered macros.
	 *  @param type Interrupt value to propagate.
	 */
	virtual void interrupt(IInterrupt::Value type) = 0;
	/** @brief Register a macro by name.
	 *  @param name Unique string key.
	 *  @param macroPtr Pointer to the macro to register.
	 */
	virtual void map(const char *name, IMacro *macroPtr) = 0;
	/** @brief Remove a macro registration by name.
	 *  @param name Key to remove.
	 */
	virtual void unmap(const char *name) = 0;
	/** @brief Look up a macro by name.
	 *  @param name Key to search for.
	 *  @return Pointer to the registered macro, or nullptr if not found.
	 */
	virtual IMacro *find(const char *name) = 0;
	/** @brief Remove all registered macros. */
	virtual void clear() = 0;
};
}


