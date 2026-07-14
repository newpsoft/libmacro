/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief Allocator types for runtime polymorphic creation of Signal/Trigger
 *  subtypes.
 *
 *  Moved from the registry interfaces into @ref mcr::factory.
 */

#pragma once

#include "mcr/signal.h"
#include "mcr/trigger.h"

#ifdef __cplusplus

namespace mcr
{
namespace factory
{

/**
 * @brief Allocator for creating and destroying Signal instances.
 *
 *  Holds function pointers for allocate/deallocate and an optional
 *  default dispatcher to assign to newly created signals.
 */
struct MCR_API SignalAllocator {
	MCR_DECL_POD(SignalAllocator)

	typedef Signal *(*allocate_fn)();
	typedef void (*deallocate_fn)(Signal *);

	/** @brief Function pointer to allocate a new Signal. */
	allocate_fn allocate;
	/** @brief Function pointer to deallocate a Signal. */
	deallocate_fn deallocate;
	/** @brief Default dispatcher to assign to newly allocated signals. */
	IDispatcher *dispatcherPtr;

	SignalAllocator(allocate_fn allocate = nullptr,
			deallocate_fn deallocate = nullptr,
			IDispatcher *dispatcherPtr = nullptr)
		: allocate(allocate)
		, deallocate(deallocate)
		, dispatcherPtr(dispatcherPtr)
	{
	}
};

/**
 * @brief Typed signal allocator that creates instances of a specific Signal subtype.
 *
 *  @tparam InstanceT The Signal subclass to allocate.
 */
template <typename InstanceT> struct TSignalAllocator : public SignalAllocator {
	MCR_DECL_POD(TSignalAllocator)

	TSignalAllocator(allocate_fn allocate = tallocate,
			 deallocate_fn deallocate = tdeallocate,
			 IDispatcher *dispatcherPtr = nullptr)
		: SignalAllocator(allocate, deallocate, dispatcherPtr)
	{
	}

	/** @brief Allocate a new InstanceT. @return Pointer to the new Signal. */
	static Signal *tallocate()
	{
		return new InstanceT();
	}
	/** @brief Deallocate a Signal created by tallocate.
	 *  @param ptr Signal to delete (may be nullptr).
	 */
	static void tdeallocate(Signal *ptr)
	{
		if (ptr)
			delete ptr;
	}
};

/**
 * @brief Allocator for creating and destroying Trigger instances.
 *
 *  Holds function pointers for allocate/deallocate.
 */
struct MCR_API TriggerAllocator {
	MCR_DECL_POD(TriggerAllocator)

	typedef Trigger *(*allocate_fn)();
	typedef void (*deallocate_fn)(Trigger *);

	/** @brief Function pointer to allocate a new Signal. */
	allocate_fn allocate;
	/** @brief Function pointer to deallocate a Signal. */
	deallocate_fn deallocate;

	TriggerAllocator(allocate_fn allocate = nullptr,
			 deallocate_fn deallocate = nullptr)
		: allocate(allocate)
		, deallocate(deallocate)
	{
	}
};

/**
 * @brief Typed trigger allocator that creates instances of a specific Trigger subtype.
 *
 *  @tparam InstanceT The Trigger subclass to allocate.
 */
template <typename InstanceT>
struct TTriggerAllocator : public TriggerAllocator {
	MCR_DECL_POD(TTriggerAllocator)

	TTriggerAllocator(allocate_fn allocate = tallocate,
			  deallocate_fn deallocate = tdeallocate)
		: TriggerAllocator(allocate, deallocate)
	{
	}

	/** @brief Allocate a new InstanceT. @return Pointer to the new Trigger. */
	static Trigger *tallocate()
	{
		return new InstanceT();
	}
	static void tdeallocate(Trigger *ptr)
	{
		if (ptr)
			delete ptr;
	}
};

} /* namespace factory */
} /* namespace mcr */

#endif /* __cplusplus */

