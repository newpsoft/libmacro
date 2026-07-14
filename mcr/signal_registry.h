/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Registry interface for allocating Signal instances by name.
 */

#ifndef __cplusplus
#pragma message "C++ is required.."
#include "mcr/err.h"
#endif

#pragma once

#include "mcr/allocator.h"
#include "mcr/signal.h"

namespace mcr
{
class IDispatcher;
/**
 * @brief Registry that maps string names to Signal allocators for dynamic creation.
 */
class MCR_API ISignalRegistry {
    public:
	class MCR_API Deleter {
	    public:
		void operator()(ISignalRegistry *ptr) const;
	};
	MCR_DECL_INTERFACE(ISignalRegistry)

	/** @brief Register a signal type by name with its allocator.
	 *  @param name Unique string key.
	 *  @param allocator Allocator used to create and destroy instances.
	 */
	virtual void map(const char *name,
			 const factory::SignalAllocator &allocator) = 0;
	/** @brief Register a signal type by instance with its allocator.
	 *  @param instance Prototype signal whose name() is used as the key.
	 *  @param allocator Allocator used to create and destroy instances.
	 */
	virtual void map(const Signal &instance,
			 const factory::SignalAllocator &allocator) = 0;
	template <typename InstanceT>
	inline void map(const factory::SignalAllocator &allocator =
				factory::TSignalAllocator<InstanceT>())
	{
		map(InstanceT(), allocator);
	}
	/** @brief Create a new Signal instance by registered name.
	 *  @param name Key of the signal type to allocate.
	 *  @return Newly allocated Signal, or nullptr if name is not registered.
	 */
	virtual Signal *allocate(const char *name) = 0;
	/** @brief Allocate another instance of the same type as the given signal.
	 *  @param signalPtr Prototype signal whose type name() is used.
	 *  @return Newly allocated Signal of the same type, or nullptr.
	 */
	inline Signal *another(const Signal *signalPtr)
	{
		return signalPtr ? allocate(signalPtr->name()) : nullptr;
	}
	/** @brief Destroy a previously allocated Signal.
	 *  @param signalPtr Signal to deallocate (may be nullptr).
	 */
	virtual void deallocate(Signal *signalPtr) = 0;
};
}


