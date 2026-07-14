/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref mcr::Libmacro Public C++ Api
 *
 *  @todo The only essential portion of code to optimize is intercept and dispatch.
 */

/*! @file Libmacro
 *  @brief @ref mcr::Libmacro Public C++ Api
 *
 *  Includes @ref libmacro.h
 */

#pragma once

#include "mcr/error.h"
#include "mcr/macro_registry.h"
#include "mcr/signal_registry.h"
#include "mcr/trigger_registry.h"
#include "mcr/serial.h"
#include "mcr/dispatcher.h"

#ifdef __cplusplus

/*! @namespace mcr
*  @brief Libmacro, by Jonathan Pelletier, New Paradigm Software. Alpha version.
*
*  1. @ref ISignal is dispatched to @ref IDispatcher using @ref Libmacro::dispatch. @n
*       1.0.a Disable dispatch for a signal by setting @ref ISignal.dispatchFlag to false. @n
*       1.0.b Disable dispatch for all of an ISignal type by setting @ref TODOISignal.dispatcher to NULL. @n
*       1.0.c Disable Libmacro generic dispatch (receive all types) by setting @ref mcr_context.base.generic_dispatch.flag to false. @n
*   1.1 Dispatching may be received by @ref mcr_DispatchReceiver. @n
*   1.2 @ref mcr_Trigger_receive may be used to dispatch into @ref mcr_Trigger. @n
*   1.3 Triggered action may be a @ref mcr_Macro, which sends a list of @ref ISignal. @n
*  2. If signal is not blocked by dispatching, it is then sent to cause an action. @n
*/
namespace mcr
{

/**
 * @brief Public C++ Library context interface.
 *
 *  Singleton that owns all Libmacro sub-components (registries, dispatcher,
 *  serial). Must be disabled with setEnabled(false) before destruction to
 *  avoid threading errors.
 */
class MCR_API Libmacro {
    public:
	MCR_DECL_INTERFACE(Libmacro)
	class MCR_API Deleter {
	    public:
		void operator()(Libmacro *ptr) const;
	};

	/** @brief Get the last created Libmacro module.
	 *
	 *  Will throw mcr::Error(EFAULT) if no Libmacro exists, and will not
	 *  create a singleton object.
	 */
	static Libmacro *instance();
	static inline Libmacro *instance(Libmacro *contextContainerPtrIfNotNull)
	{
		return contextContainerPtrIfNotNull ?
			       contextContainerPtrIfNotNull :
			       instance();
	}
	/** @brief Check if a specific context pointer is the current instance.
	 *  @param contextContainerPtr Pointer to test.
	 *  @return true if contextContainerPtr matches the current instance.
	 */
	static bool hasInstance(Libmacro *contextContainerPtr);

	/** @brief Check if hardware interception is active and blocking is available.
	 *  @return true if hardware intercept is grabbed.
	 */
	virtual bool blockableFlag() const = 0;
	/** @brief Enable or disable hardware interception.
	 *  @param flag true to enable hardware intercept.
	 */
	virtual void setBlockableFlag(bool flag) = 0;
	/** @brief Check if generic dispatch is enabled for all signals.
	 *  @return true if generic dispatch is active.
	 */
	virtual bool genericDispatchFlag() const = 0;
	/** @brief Enable or disable generic dispatch for all signals.
	 *  @param flag true to enable generic dispatch.
	 */
	virtual void setGenericDispatchFlag(bool flag) = 0;
	/** @brief Get the current set of active modifier flags.
	 *  @return Bitmask of active modifiers.
	 */
	virtual unsigned int modifiers() const = 0;
	/** @brief Set the active modifier flags.
	 *  @param mods Bitmask of modifiers to set.
	 */
	virtual void setModifiers(unsigned int mods) = 0;

	/** @brief Get the generic dispatcher that receives all signal types.
	 *  @return Pointer to the generic dispatcher.
	 */
	virtual IDispatcher *genericDispatcher() const = 0;
	/** @brief Set the generic dispatcher.
	 *  @param value Dispatcher to use for generic signal dispatch.
	 */
	virtual void setGenericDispatcher(IDispatcher *value) = 0;

	/** @brief Get the serialization interface for name/value mapping.
	 *  @return Reference to the serial interface.
	 */
	virtual ISerial &serial() = 0;
	virtual const ISerial &serial() const = 0;

	/** @brief Get the macro registry.
	 *  @return Reference to the macro registry.
	 */
	virtual IMacroRegistry &macroRegistry() = 0;
	virtual const IMacroRegistry &macroRegistry() const = 0;
	/** @brief Get the signal registry.
	 *  @return Reference to the signal registry.
	 */
	virtual ISignalRegistry &signalRegistry() = 0;
	virtual const ISignalRegistry &signalRegistry() const = 0;
	/** @brief Get the trigger registry.
	 *  @return Reference to the trigger registry.
	 */
	virtual ITriggerRegistry &triggerRegistry() = 0;
	virtual const ITriggerRegistry &triggerRegistry() const = 0;

	/** @brief Check if the library is enabled.
	 *  @return true if enabled.
	 */
	virtual bool enabled() const = 0;
	/** @brief Enable or disable the library.
	 *  @param val true to enable, false to disable.
	 */
	virtual void setEnabled(bool val) = 0;
	/** @brief Get the global macro thread limit.
	 *  @return The current global thread limit.
	 */
	unsigned int globalThreadLimit() const;
	/** @brief Set the global macro thread limit.
	 *  @param val New global thread limit.
	 */
	void setGlobalThreadLimit(unsigned int val);
};
}
#endif

