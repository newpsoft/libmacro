/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief IMacro — thread-like executor with Libmacro-specific context.
 *
 *  @todo Register dipatch should not be coupled with threaded macro run.
 */

#pragma once

#include "mcr/defines.h"
#include "mcr/signal/interrupt.h"
#include "mcr/trigger.h"

#ifdef __cplusplus
namespace mcr
{
class Libmacro;
/*! A thread-like executor, with additional context that is specific to Libmacro.
 *
 *  All set and threaded run functions may throw mcr::Error
 *  Virtual functions: objectChanged, start, clearAll, copy @n
 *  Has a mutex locking interruptor, enabled, activators, and triggers, which
 *  are all mutually exclusive.
 */
class MCR_API IMacro : public IActor, public IInterrupt, public IReceive {
    public:
	class MCR_API Deleter {
	    public:
		void operator()(IMacro *ptr) const;
	};
	MCR_DECL_INTERFACE(IMacro)

	/** @brief Get the Libmacro context this macro belongs to.
	 *  @return Pointer to the owning Libmacro instance.
	 */
	virtual Libmacro *context() const = 0;
	/** @brief Set the Libmacro context.
	 *  @param ctx Libmacro instance to associate with.
	 */
	virtual void setContext(Libmacro *ctx) = 0;

	inline bool operator()()
	{
		start();
		return blocking();
	}

	/** @brief The number of currently active threads, as managed by the concrete class.
	 *  @return The number of currently active threads.
	 */
	virtual bool globalActiveThreadCount() const noexcept = 0;
	/** @brief Check if this macro blocks the dispatcher when running.
	 *  @return true if blocking is enabled.
	 */
	virtual bool blocking() const = 0;
	/** @brief Set whether this macro blocks the dispatcher.
	 *  @param val true to enable blocking.
	 */
	virtual void setBlocking(bool val) = 0;

	inline bool enabled() const
	{
		return interruptor() != IInterrupt::DISABLE;
	}
	inline void setEnabled(bool val)
	{
		setInterruptor(val ? CONTINUE : DISABLE);
	}

	/** @brief Get the current interrupt state.
	 *  @return Current interrupt value.
	 */
	virtual IInterrupt::Value interruptor() const = 0;
	/** @brief Set the interrupt state.
	 *  @param val Interrupt value to apply.
	 */
	virtual void setInterruptor(IInterrupt::Value val) = 0;
	virtual void interrupt(IInterrupt::Value val) override
	{
		setInterruptor(val);
	}

	/** @brief Get the unique name of this macro.
	 *  @return Null-terminated string identifying the macro.
	 */
	virtual const char *name() const = 0;
	/** @brief Set the macro name.
	 *  @param val Null-terminated string name.
	 */
	virtual void setName(const char *val) = 0;

	/** @brief Check if this macro is sticky (retains state across triggers).
	 *  @return true if sticky mode is enabled.
	 */
	virtual bool sticky() const = 0;
	/** @brief Set sticky mode.
	 *  @param val true to enable sticky mode.
	 */
	virtual void setSticky(bool val) = 0;

	/** @brief Get the maximum number of concurrent threads.
	 *  @return Thread count limit.
	 */
	virtual unsigned int threadMax() const = 0;
	/** @brief Set the maximum number of concurrent threads.
	 *  @param val Thread count limit.
	 */
	virtual void setThreadMax(unsigned int val) = 0;

	/** @brief Set the activator signals that can trigger this macro.
	 *  @param array Array of signal instances.
	 *  @param count Number of elements in the array.
	 */
	virtual void setActivators(const Signal *array, mcr_index_t count) = 0;
	template <typename T> inline void setActivators(const T &vals)
	{
		setActivators(&vals[0], vals.size());
	}
	inline void clearActivators()
	{
		setActivators(nullptr, 0);
	}

	/** @brief Set the signals sent when this macro runs.
	 *  @param array Array of signal instances.
	 *  @param count Number of elements in the array.
	 */
	virtual void setSignals(const Signal *array, mcr_index_t count) = 0;
	template <typename T> inline void setSignals(const T &vals)
	{
		setSignals(&vals[0], vals.size());
	}
	inline void clearSignals()
	{
		setSignals(nullptr, 0);
	}

	/** @brief Set the triggers that can activate this macro.
	 *  @param array Array of trigger instances.
	 *  @param count Number of elements in the array.
	 */
	virtual void setTriggers(const Trigger *array, mcr_index_t count) = 0;
	template <typename T> inline void setTriggers(const T &vals)
	{
		setTriggers(&vals[0], vals.size());
	}
	inline void clearTriggers()
	{
		setTriggers(nullptr, 0);
	}

	/** @brief Get the current number of running threads.
	 *  @return Active thread count.
	 */
	virtual int threadCount() const = 0;
	/** @brief Get the number of queued macro runs.
	 *  @return Queue depth.
	 */
	virtual unsigned int queued() const = 0;

	inline void clearAll()
	{
		clearActivators();
		clearSignals();
		clearTriggers();
	}

	/** @brief Start execution of this macro in a new thread. */
	virtual void start() = 0;
	/** @brief Run the macro synchronously in the current thread.
	 *  @return Exit code.
	 */
	virtual int run() = 0;
	inline bool running()
	{
		return threadCount();
	}
	/** @brief Receive a dispatched signal (IReceive override).
	 *  @param signalPtr The signal being dispatched.
	 *  @param mods Active modifier flags.
	 *  @return true to block, false to continue.
	 */
	virtual bool receive(Signal *signalPtr, unsigned int mods) override = 0;

	/** @brief Copy state from another IMacro.
	 *  @param other Source macro to copy from.
	 */
	virtual void copy(const IMacro &other) = 0;

	/** @brief Apply (re-register) all dispatch relationships. */
	virtual void applyDispatch() = 0;
	/** @brief Check if dispatch registration is enabled.
	 *  @return true if dispatch will be registered on applyDispatch().
	 */
	virtual bool applyDispatchEnabled() const = 0;
	/** @brief Enable or disable dispatch registration.
	 *  @param val true to enable dispatch registration (default), false to skip it.
	 *  Safe to toggle at any time, even while the macro is running.
	 */
	virtual void setApplyDispatchEnabled(bool val) = 0;
	/** @brief Add dispatch relationships for all activators and triggers. */
	virtual void addDispatch() = 0;
	/** @brief Remove all dispatch relationships. */
	virtual void removeDispatch() = 0;
	/** @brief Add a dispatch relationship for a specific signal.
	 *  @param signalPtr Signal to register for dispatch.
	 */
	virtual void addDispatch(Signal &signalPtr) = 0;
	/** @brief Add a dispatch relationship for a specific trigger.
	 *  @param trigPtr Trigger to register for dispatch.
	 */
	virtual void addDispatch(Trigger &trigPtr) = 0;
	/** @brief Add a dispatch relationship for a signal and trigger pair.
	 *  @param signalPtr Signal to register for dispatch.
	 *  @param trigPtr Trigger to register for dispatch.
	 */
	virtual void addDispatch(Signal &signalPtr, Trigger &trigPtr) = 0;
};
}
#endif

