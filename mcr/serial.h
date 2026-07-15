/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief String contracts for serialization.
 */

#ifndef __cplusplus
#pragma message "C++ is required.."
#include "mcr/err.h"
#endif

#pragma once

#include "mcr/types.h"
#include "mcr/signal/interrupt.h"

namespace mcr
{
class MCR_API ISerial {
    public:
	class MCR_API Deleter {
	    public:
		void operator()(ISerial *ptr) const;
	};
	MCR_DECL_INTERFACE(ISerial)

	/** @brief Get the apply value for a given name.
	 *  @param name String name to look up.
	 *  @return Corresponding mcr_ApplyValue.
	 */
	virtual mcr_ApplyValue applyValue(const char *name) const = 0;
	/** @brief Get the maximum apply value.
	 *  @return Maximum mcr_ApplyValue enum.
	 */
	inline mcr_ApplyValue applyTypeMax() const
	{
		return MCR_TOGGLE;
	}
	/** @brief Get the count of apply values.
	 *  @return Total number of apply value types.
	 */
	inline mcr_index_t applyTypeCount() const
	{
		return applyTypeMax() + 1;
	}
	/** @brief Get the name for a given apply value.
	 *  @param value Apply value to look up.
	 *  @return Null-terminated string name.
	 */
	virtual const char *applyValue(mcr_ApplyValue value) const = 0;

	/** @brief Look up a key press value by name.
	 *  @param name String name to look up.
	 *  @return Corresponding mcr_Press_t.
	 */
	inline mcr_Press_t keyPress(const char *name) const
	{
		return (mcr_Press_t)applyValue(name);
	}
	/** @brief Get the maximum key press value. @return Maximum mcr_Press_t. */
	inline mcr_Press_t keyPressMax() const
	{
		return (mcr_Press_t)applyTypeMax();
	}
	/** @brief Get the count of key press types. @return Total key press types. */
	inline mcr_index_t keyPressCount() const
	{
		return applyTypeCount();
	}
	/** @brief Get the name for a given key press value.
	 *  @param value Key press value to look up.
	 *  @return Null-terminated string name.
	 */
	virtual const char *keyPressName(mcr_Press_t value) const = 0;

	/** @brief Look up a dimension by name.
	 *  @param name String name to look up.
	 *  @return Corresponding mcr_Dimension.
	 */
	virtual mcr_Dimension dimension(const char *name) const = 0;
	/** @brief Get the maximum dimension value. @return Maximum mcr_Dimension. */
	inline mcr_Dimension dimensionMax() const
	{
		return MCR_DIMENSION_MAX;
	}
	/** @brief Get the count of dimension types. @return Total dimension types. */
	inline mcr_index_t dimensionCount() const
	{
		return MCR_DIMENSION_COUNT;
	}
	/** @brief Get the name for a given dimension.
	 *  @param value Dimension to look up.
	 *  @return Null-terminated string name.
	 */
	virtual const char *dimension(mcr_Dimension value) const = 0;

	/** @brief Look up modifier flags by name.
	 *  @param name String name to look up.
	 *  @return Corresponding mcr_ModFlags bitmask.
	 */
	virtual mcr_ModFlags modifiers(const char *name) const = 0;
	/** @brief Get the maximum modifier flag value.
	 *  @return Maximum mcr_ModFlags.
	 */
	mcr_ModFlags modifiersMax() const
	{
		return MCR_MF_USER;
	}
	/** @brief Get the count of modifier flag entries.
	 *  @return Total modifier entries.
	 */
	virtual mcr_index_t modifiersCount() const = 0;
	/** @brief Get the name for a given modifier flag.
	 *  @param value Modifier flag to look up.
	 *  @return Null-terminated string name.
	 */
	virtual const char *modifiersName(mcr_ModFlags value) const = 0;

	/** @brief Look up a trigger mode by name.
	 *  @param name String name to look up.
	 *  @return Corresponding mcr_TriggerMode.
	 */
	virtual mcr_TriggerMode triggerMode(const char *name) const = 0;
	/** @brief Get the maximum trigger mode value.
	 *  @return Maximum mcr_TriggerMode.
	 */
	mcr_TriggerMode triggerModeMax() const
	{
		return MCR_TM_USER;
	}
	/** @brief Get the count of trigger mode entries.
	 *  @return Total trigger mode entries.
	 */
	virtual mcr_index_t triggerModeCount() const = 0;
	/** @brief Get the name for a given trigger mode.
	 *  @param value Trigger mode to look up.
	 *  @return Null-terminated string name.
	 */
	virtual const char *triggerModeName(mcr_TriggerMode value) const = 0;

	/** @brief Look up an interrupt value by name.
	 *  @param name String name to look up.
	 *  @return Corresponding IInterrupt::Value.
	 */
	virtual IInterrupt::Value interrupt(const char *name) const = 0;
	/** @brief Get the maximum interrupt value.
	 *  @return Maximum IInterrupt::Value.
	 */
	virtual IInterrupt::Value interruptMax() const = 0;
	/** @brief Get the count of interrupt entries.
	 *  @return Total interrupt entries.
	 */
	virtual mcr_index_t interruptCount() const = 0;
	/** @brief Get the name for a given interrupt value.
	 *  @param value Interrupt value to look up.
	 *  @return Null-terminated string name.
	 */
	virtual const char *interruptName(IInterrupt::Value value) = 0;

	/** @name Extensibility
	 *  Register custom name-to-value and value-to-name mappings for
	 *  modifier and trigger-mode lookups.
	 *
	 *  User-registered entries are checked before built-in tables in
	 *  name lookups, and as fallback after switch statements in
	 *  value lookups.
	 */
	///@{
	/** @brief Register a custom modifier name-to-value mapping.
	 *  @param name Modifier name string.
	 *  @param value Modifier flag bitmask. */
	virtual void setModifierName(const char *name,
				    mcr_ModFlags value) = 0;
	/** @brief Register a custom modifier value-to-name mapping.
	 *  @param value Modifier flag bitmask.
	 *  @param name Display name string. */
	virtual void
	setModifierValueName(mcr_ModFlags value, const char *name) = 0;
	/** @brief Remove a previously registered custom modifier name. */
	virtual void removeModifierName(const char *name) = 0;
	/** @brief Register a custom trigger mode name-to-value mapping. */
	virtual void setTriggerModeName(const char *name,
					mcr_TriggerMode value) = 0;
	/** @brief Register a custom trigger mode value-to-name mapping. */
	virtual void setTriggerModeValueName(mcr_TriggerMode value,
					     const char *name) = 0;
	/** @brief Remove a previously registered custom trigger mode name. */
	virtual void removeTriggerModeName(const char *name) = 0;
	///@}
};
}


