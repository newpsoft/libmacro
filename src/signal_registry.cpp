/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/signal_registry.h"
#include "mcr/error.h"
#include "mcr/factory.h"

#include <cassert>
#include <unordered_map>
#include <string>

#include "mcr/signal.h"

namespace mcr
{

class SignalRegistry : public ISignalRegistry {
    public:
	SignalRegistry() = default;
	SignalRegistry(const SignalRegistry &other) = default;
	virtual ~SignalRegistry() override = default;
	SignalRegistry &operator=(const SignalRegistry &other) = default;

	virtual void map(const char *name,
			 const factory::SignalAllocator &allocator) override;
	virtual void map(const Signal &instance,
			 const factory::SignalAllocator &allocator) override;
	virtual Signal *allocate(const char *name) override;
	virtual void deallocate(Signal *signalPtr) override;

    private:
	std::unordered_map<std::string, factory::SignalAllocator> _mapAllocators;
};

void ISignalRegistry::Deleter::operator()(ISignalRegistry *ptr) const
{
	delete ptr;
}

namespace internal
{
namespace factory
{

std::unique_ptr<ISignalRegistry, ISignalRegistry::Deleter>
createSignalRegistry()
{
	return std::unique_ptr<ISignalRegistry, ISignalRegistry::Deleter>(
		new SignalRegistry());
}

std::shared_ptr<ISignalRegistry> createSignalRegistryShared()
{
	return createSignalRegistry();
}

}
}

void SignalRegistry::map(const char *name,
			 const factory::SignalAllocator &allocator)
{
	assert(allocator.allocate);
	assert(allocator.deallocate);
	_mapAllocators[name] = allocator;
}

void SignalRegistry::map(const Signal &instance,
			 const factory::SignalAllocator &allocator)
{
	assert(allocator.allocate);
	assert(allocator.deallocate);
	_mapAllocators[instance.name()] = allocator;
}

Signal *SignalRegistry::allocate(const char *name)
{
	// The hidden signal!
	if (!name)
		name = "";
	auto found = _mapAllocators.find(name);
	if (found == _mapAllocators.end())
		return nullptr;
	auto &allo = found->second;
	Signal *ptr = allo.allocate();
	if (!ptr)
		throw Error(ENOMEM, "Failed to allocate signal");
	ptr->dispatcherPtr = allo.dispatcherPtr;
	return ptr;
}

void SignalRegistry::deallocate(Signal *signalPtr)
{
	if (!signalPtr)
		return;
	auto found = _mapAllocators.find(signalPtr->name());
	if (found == _mapAllocators.end())
		throw Error(EINVAL, "Signal not found in registry");
	auto &allo = found->second;
	allo.deallocate(signalPtr);
}

}
