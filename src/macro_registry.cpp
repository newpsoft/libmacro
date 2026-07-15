/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/libmacro.h"
#include "mcr/factory.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "mcr/macro.h"

namespace mcr
{

class MacroRegistry : public IMacroRegistry {
    public:
	MacroRegistry() = default;
	MacroRegistry(const MacroRegistry &) = default;
	virtual ~MacroRegistry() override = default;
	MacroRegistry &operator=(const MacroRegistry &other) = default;

	virtual void interrupt(IInterrupt::Value type) override;

	virtual void map(const char *name, IMacro *macroPtr) override;
	virtual void unmap(const char *name) override;
	virtual IMacro *find(const char *name) override;
	virtual void clear() override;

    private:
	std::unordered_map<std::string, IMacro *> _macroMap;
};

void IMacroRegistry::Deleter::operator()(IMacroRegistry *ptr) const
{
	delete ptr;
}

namespace internal
{
namespace factory
{

std::unique_ptr<IMacroRegistry, IMacroRegistry::Deleter> createMacroRegistry()
{
	return std::unique_ptr<IMacroRegistry, IMacroRegistry::Deleter>(
		new MacroRegistry());
}

std::shared_ptr<IMacroRegistry> createMacroRegistryShared()
{
	return createMacroRegistry();
}

}
}

void MacroRegistry::interrupt(IInterrupt::Value type)
{
	auto &myMap = _macroMap;
	std::unordered_set<void *> set;
	for (auto &iter : myMap) {
		if (set.find(iter.second) == set.end()) {
			set.insert(iter.second);
			iter.second->interrupt(type);
		}
	}
}

void MacroRegistry::map(const char *name, IMacro *macroPtr)
{
	_macroMap[name] = macroPtr;
}

void MacroRegistry::unmap(const char *name)
{
	auto &myMap = _macroMap;
	auto element = myMap.find(name);
	if (element != myMap.end())
		myMap.erase(element);
}

IMacro *MacroRegistry::find(const char *name)
{
	auto iter = _macroMap.find(name);
	if (iter == _macroMap.end())
		return nullptr;
	return iter->second;
}

void MacroRegistry::clear()
{
	_macroMap.clear();
}

}
