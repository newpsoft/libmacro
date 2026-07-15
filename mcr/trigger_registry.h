/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Registry interface for allocating Trigger instances by name.
 */

#ifndef __cplusplus
#pragma message "C++ is required.."
#include "mcr/err.h"
#endif

#pragma once

#include "mcr/allocator.h"
#include "mcr/trigger.h"

namespace mcr
{

/**
 * @brief Registry that maps string names to Trigger allocators for dynamic creation.
 */
class MCR_API ITriggerRegistry {
    public:
	class MCR_API Deleter {
	    public:
		void operator()(ITriggerRegistry *ptr) const;
	};
	MCR_DECL_INTERFACE(ITriggerRegistry)

	/** @brief Register a trigger type by name with its allocator.
	 *  @param name Unique string key.
	 *  @param allocator Allocator used to create and destroy instances.
	 */
	virtual void map(const char *name,
			 const factory::TriggerAllocator &allocator) = 0;
	/** @brief Register a trigger type by instance with its allocator.
	 *  @param instance Prototype trigger whose name() is used as the key.
	 *  @param allocator Allocator used to create and destroy instances.
	 */
	virtual void map(const Trigger &instance,
			 const factory::TriggerAllocator &allocator) = 0;
	template <typename InstanceT>
	inline void map(const factory::TriggerAllocator &allocator =
				factory::TTriggerAllocator<InstanceT>())
	{
		map(InstanceT(), allocator);
	}
	/** @brief Create a new Trigger instance by registered name.
	 *  @param name Key of the trigger type to allocate.
	 *  @return Newly allocated Trigger, or nullptr if name is not registered.
	 */
	virtual Trigger *allocate(const char *name) = 0;
	/** @brief Allocate another instance of the same type as the given trigger.
	 *  @param triggerPtr Prototype trigger whose type name() is used.
	 *  @return Newly allocated Trigger of the same type, or nullptr.
	 */
	inline Trigger *another(const Trigger *triggerPtr)
	{
		return triggerPtr ? allocate(triggerPtr->name()) : nullptr;
	}
	/** @brief Destroy a previously allocated Trigger.
	 *  @param triggerPtr Trigger to deallocate (may be nullptr).
	 */
	virtual void deallocate(Trigger *triggerPtr) = 0;
};
}


