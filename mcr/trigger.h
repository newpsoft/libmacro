/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref ITrigger - Receive dispatch and trigger actions.
 *
 *  @todo Where does set actor go?
 */

#pragma once

#include "mcr/defines.h"
#include "mcr/dispatcher.h"

#ifdef __cplusplus
#include "mcr/template/list.h"
namespace mcr
{

/**
 * @brief Interface for objects that perform an action when triggered.
 */
class MCR_API IActor {
    public:
	MCR_DECL_INTERFACE(IActor)
	inline bool operator()()
	{
		return act();
	}
	/** @brief Execute the action.
	 *  @return true to block further processing, false to continue.
	 */
	virtual bool act() = 0;
};

/*!
 */
/**
 * @brief Receives dispatched signals and triggers actions.
 *
 *  A Trigger receives signals from an IDispatcher and, based on its
 *  configuration, invokes an IActor to perform an action.
 */
class MCR_API Trigger : public IReceive {
    public:
	MCR_DECL_INTERFACE(Trigger)

	/** @brief Actor to invoke when this trigger fires. */
	IActor *actorPtr = nullptr;
	/** @brief If true, blocks further dispatch when triggered. */
	bool blockingFlag = false;

	/** @brief Get the unique name of this trigger type.
	 *  @return Null-terminated string identifying the trigger type.
	 */
	virtual const char *name() const = 0;
	/** @brief Get an alternative name for this trigger type.
	 *  @param aliasNumber Alias index.
	 *  @return Alias string, or nullptr if the alias does not exist.
	 */
	virtual const char *alias(unsigned int aliasNumber) const
	{
		(void)(aliasNumber);
		return nullptr;
	}
	/** @brief Receive a dispatched signal and decide whether to trigger.
	 *  @param signalPtr The signal being dispatched.
	 *  @param mods Active modifier flags.
	 *  @return true to block further dispatch, false to continue.
	 */
	virtual bool receive(Signal *, unsigned int) = 0;
	/** @brief Invoke the actor if one is set.
	 *  @param signalPtr The triggering signal.
	 *  @param mods Active modifier flags.
	 *  @return The blockingFlag value.
	 */
	virtual bool trigger(Signal *, unsigned int)
	{
		if (actorPtr)
			actorPtr->act();
		return blockingFlag;
	}
};

template struct MCR_API List<Trigger>;
/**
 * @brief A list of Triggers that also acts as an IReceive.
 *
 *  When a signal is received, all triggers in the list are invoked.
 */
class MCR_API TriggerList : public List<Trigger>, IReceive {
    public:
	TriggerList(Trigger *array = nullptr, mcr_index_t count = 0)
		: List<Trigger>(array, count)
	{
	}
	virtual ~TriggerList() = default;

	/** @brief Forward a received signal to all triggers in the list.
	 *  @param signalPtr The signal being dispatched.
	 *  @param modsPtr Active modifier flags.
	 *  @return true always (all triggers are invoked).
	 */
	bool receive(Signal *signalPtr, unsigned int modsPtr) override
	{
		bool block = false;
		for (mcr_index_t i = 0; i < count; i++) {
			if (array[i].receive(signalPtr, modsPtr))
				block = true;
		}
		return true;
	}
};

}

#endif // __cplusplus

