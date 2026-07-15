/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/trigger_registry.h"
#include "mcr/error.h"
#include "mcr/factory.h"

#include <cassert>
#include <unordered_map>
#include <string>

#include "mcr/trigger.h"

namespace mcr
{

class TriggerRegistry : public ITriggerRegistry {
    public:
	TriggerRegistry() = default;
	TriggerRegistry(const TriggerRegistry &other) = default;
	virtual ~TriggerRegistry() override = default;
	TriggerRegistry &operator=(const TriggerRegistry &other) = default;

	virtual void map(const char *name,
			 const factory::TriggerAllocator &allocator) override;
	virtual void map(const Trigger &instance,
			 const factory::TriggerAllocator &allocator) override;
	virtual Trigger *allocate(const char *name) override;
	virtual void deallocate(Trigger *triggerPtr) override;

    private:
	std::unordered_map<std::string, factory::TriggerAllocator>
		_mapAllocators;
};

void ITriggerRegistry::Deleter::operator()(ITriggerRegistry *ptr) const
{
	delete ptr;
}

namespace internal
{
namespace factory
{

std::unique_ptr<ITriggerRegistry, ITriggerRegistry::Deleter>
createTriggerRegistry()
{
	return std::unique_ptr<ITriggerRegistry, ITriggerRegistry::Deleter>(
		new TriggerRegistry());
}

std::shared_ptr<ITriggerRegistry> createTriggerRegistryShared()
{
	return createTriggerRegistry();
}

}
}

void TriggerRegistry::map(const char *name,
			  const factory::TriggerAllocator &allocator)
{
	assert(allocator.allocate);
	assert(allocator.deallocate);
	_mapAllocators[name] = allocator;
}

void TriggerRegistry::map(const Trigger &instance,
			  const factory::TriggerAllocator &allocator)
{
	assert(allocator.allocate);
	assert(allocator.deallocate);
	_mapAllocators[instance.name()] = allocator;
}

Trigger *TriggerRegistry::allocate(const char *name)
{
	// The hidden trigger!
	if (!name)
		name = "";
	auto found = _mapAllocators.find(name);
	if (found == _mapAllocators.end())
		return nullptr;
	auto &allo = found->second;
	Trigger *ptr = allo.allocate();
	if (!ptr)
		throw Error(ENOMEM, "Failed to allocate trigger");
	return ptr;
}

void TriggerRegistry::deallocate(Trigger *triggerPtr)
{
	if (!triggerPtr)
		return;
	auto found = _mapAllocators.find(triggerPtr->name());
	if (found == _mapAllocators.end())
		throw Error(EINVAL, "Trigger not found in registry");
	auto &allo = found->second;
	allo.deallocate(triggerPtr);
}

}
