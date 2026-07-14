/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref Action - Conditional trigger from intercepted modifiers
 */

#pragma once

#include "mcr/trigger.h"
#include "mcr/types.h"

#ifdef __cplusplus

namespace mcr
{
/*! Conditional trigger from intercepted modifiers */
class MCR_API Action : public Trigger {
    public:
	MCR_DECL_INTERFACE(Action)

	/*! @ref mcr_ModFlags Default MCR_MF_NONE */
	unsigned int modifiers = 0;
	/*! @ref mcr_TriggerMode Default MCR_TM_EQUAL */
	unsigned int triggerMode = MCR_TM_EQUAL;

	virtual const char *name() const override
	{
		return "Action";
	}
	virtual bool receive(Signal *, unsigned int) override;
};
}
#endif

