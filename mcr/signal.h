/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref Signal - Signal to send or dispatch
 *
 */

#pragma once

#include "mcr/api.h"

#ifdef __cplusplus
#include "mcr/template/list.h"
namespace mcr
{
class IDispatcher;
class Libmacro;
/**
 * @brief A signal to send or dispatch.
 *
 *  Signals are the basic unit of communication in Libmacro. They can be
 *  sent directly or dispatched through an IDispatcher to receivers.
 */
class MCR_API Signal {
    public:
	MCR_DECL_INTERFACE(Signal)

	/** @brief Dispatcher this signal is registered with (nullptr if unregistered). */
	IDispatcher *dispatcherPtr = nullptr;
	/** @brief If true, dispatch is enabled for this signal. */
	bool dispatchFlag = false;

	/** @brief Get the unique name of this signal type.
	 *  @return Null-terminated string identifying the signal type.
	 */
	virtual const char *name() const = 0;
	/** @brief Get an alternative name for this signal type.
	 *  @param aliasNumber Alias index.
	 *  @return Alias string, or nullptr if the alias does not exist.
	 */
	virtual const char *alias(unsigned int aliasNumber) const
	{
		(void)(aliasNumber);
		return nullptr;
	}
	/** @brief Send this signal, performing its associated action. */
	virtual void send() = 0;
};

template struct MCR_API List<Signal>;
/**
 * @brief A list of Signal pointers that can be sent as a group.
 */
struct MCR_API SignalList : public List<Signal> {
	SignalList(Signal *array = nullptr, mcr_index_t count = 0)
		: List<Signal>(array, count)
	{
	}
	virtual ~SignalList() = default;
	SignalList(const SignalList &) = default;

	/** @brief Send all signals in this list.
	 *  @param libmacroPtr Libmacro context used for dispatch.
	 */
	void send(Libmacro *libmacroPtr) const;
};

}

#endif // __cplusplus

