/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref IDispatcher - Blocking dispatcher, intercept signals before
 *  sending.
 *
 *  Dispatchers are the consumers of modifiers.
 */

#pragma once

#include "mcr/defines.h"

#ifdef __cplusplus

namespace mcr
{
class Signal;
class Libmacro;
/**
 * @brief Interface for objects that can receive dispatched signals.
 */
class MCR_API IReceive {
    public:
	MCR_DECL_INTERFACE(IReceive)
	/** @brief Receive a dispatched signal with modifiers.
	 *  @param signalPtr The signal being dispatched.
	 *  @param mods Active modifier flags.
	 *  @return true to block further dispatch, false to continue.
	 */
	virtual bool receive(Signal *, unsigned int) = 0;
};

/**
 * @brief Blocking dispatcher that intercepts signals before sending.
 *
 *  Dispatchers are the consumers of modifiers. They maintain a list of
 *  signal/receiver pairs and decide whether a signal should be blocked
 *  or allowed to propagate.
 */
class MCR_API IDispatcher {
    public:
	class MCR_API Deleter {
	    public:
		void operator()(IDispatcher *ptr) const;
	};
	MCR_DECL_INTERFACE(IDispatcher)

	/** @brief Register a signal/receiver pair.
	 *  @param signalPtr Signal to intercept.
	 *  @param receiverPtr Receiver to notify on dispatch.
	 */
	virtual void add(Signal *signalPtr, IReceive *receiverPtr) = 0;
	/** @brief Remove all registered signal/receiver pairs. */
	virtual void clear() noexcept = 0;
	/** @brief Update modifier state after a signal is processed.
	 *  @param signalPtr The signal whose modifiers are being updated.
	 *  @param modsPtr Pointer to the modifier flags to modify.
	 */
	virtual void modifier(Signal *, unsigned int *) noexcept = 0;
	/** @brief Dispatch a signal to all registered receivers.
	 *  @param signalPtr The signal to dispatch.
	 *  @param mods Active modifier flags.
	 *  @return true if dispatch was blocked, false if allowed through.
	 */
	virtual bool dispatch(Signal *, unsigned int) = 0;
	/** @brief Dispatch a signal; if not blocked, update modifiers.
	 *  @param signalPtr The signal to dispatch.
	 *  @param modsPtr Pointer to modifier flags to update on success.
	 *  @return true if dispatch was blocked, false if allowed through.
	 */
	inline bool dispatchAndModify(Signal *signalPtr, unsigned int *modsPtr)
	{
		if (dispatch(signalPtr, *modsPtr))
			return true;
		modifier(signalPtr, modsPtr);
		return false;
	}
	/** @brief Remove a specific receiver from all registrations.
	 *  @param removeReceiverPtr Receiver to unregister.
	 */
	virtual void remove(IReceive *removeReceiverPtr) = 0;
	/** @brief Remove invalid entries from the dispatcher list. */
	virtual void trim() noexcept = 0;
	/** @brief Get the number of registered signal/receiver pairs.
	 *  @return Count of registered pairs.
	 */
	virtual mcr_index_t count() const noexcept = 0;
	/** @brief Alias for count(). @return Count of registered pairs. */
	inline mcr_index_t size() const noexcept
	{
		return count();
	}
	/** @brief Check if no receivers are registered.
	 *  @return true if count() is zero.
	 */
	virtual bool empty() const noexcept
	{
		return !count();
	}
};
}

#endif

