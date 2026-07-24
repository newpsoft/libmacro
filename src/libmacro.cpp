/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/libmacro.h"
#include "mcr/error.h"
#include "mcr/factory.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace mcr
{

static std::vector<Libmacro *> _registryStack;
static std::unordered_set<Libmacro *> _registrySet;
static std::mutex _registryMutex;

/*! Public C++ Api.  The context registry (global vector + set, protected
 *  by _registryMutex) allows multiple Libmacro instances to coexist.
 *  instance() returns the most recently created one. Sub-components are
 *  owned via unique_ptr and created in the constructor. */
class MCR_API LibmacroImpl final : public Libmacro {
    public:
	/*! @ref ctor
	 *  @param enabled Initial enabled state, must be disabled before
	 *  destruction
	 */
	LibmacroImpl(bool enabled = true);
	/*! @ref dtor If this is enabled during destruction threading errors
	 *  are likely to happen
	 */
	~LibmacroImpl();
	// future?
	LibmacroImpl(const LibmacroImpl &) = delete;
	LibmacroImpl &operator=(const LibmacroImpl &) = delete;

	// Libmacro
	virtual bool blockableFlag() const override;
	virtual void setBlockableFlag(bool flag) override;
	virtual bool genericDispatchFlag() const override;
	virtual void setGenericDispatchFlag(bool flag) override;
	virtual unsigned int modifiers() const override;
	virtual void setModifiers(unsigned int mods) override;

	virtual IDispatcher *genericDispatcher() const override;
	virtual void setGenericDispatcher(IDispatcher *value) override;

	virtual ISerial &serial() override;
	virtual const ISerial &serial() const override;

	virtual IMacroRegistry &macroRegistry() override;
	virtual const IMacroRegistry &macroRegistry() const override;
	virtual ISignalRegistry &signalRegistry() override;
	virtual const ISignalRegistry &signalRegistry() const override;
	virtual ITriggerRegistry &triggerRegistry() override;
	virtual const ITriggerRegistry &triggerRegistry() const override;

	virtual bool enabled() const override;
	virtual void setEnabled(bool val) override;

    private:
	bool _enabled = false;
	bool _blockableFlag = false;
	bool _genericDispatchFlag = false;
	unsigned int _modifiers = 0;
	std::unique_ptr<ISerial, ISerial::Deleter> _serial;
	std::unique_ptr<IMacroRegistry, IMacroRegistry::Deleter> _macroRegistry;
	std::unique_ptr<ISignalRegistry, ISignalRegistry::Deleter>
		_signalRegistry;
	std::unique_ptr<ITriggerRegistry, ITriggerRegistry::Deleter>
		_triggerRegistry;
	std::unique_ptr<IDispatcher, IDispatcher::Deleter>
		_genericDispatcherInstancePt;
	IDispatcher *_genericDispatcherPtr;
};

void Libmacro::Deleter::operator()(Libmacro *ptr) const
{
	delete ptr;
}

namespace factory
{

std::unique_ptr<Libmacro, Libmacro::Deleter> createContext(bool enabled)
{
	return std::unique_ptr<Libmacro, Libmacro::Deleter>(
		new LibmacroImpl(enabled));
}

std::shared_ptr<Libmacro> createContextShared(bool enabled)
{
	return createContext(enabled);
}

}

LibmacroImpl::LibmacroImpl(bool enabled)
	: _serial(internal::factory::createSerial())
	, _macroRegistry(internal::factory::createMacroRegistry())
	, _signalRegistry(internal::factory::createSignalRegistry())
	, _triggerRegistry(internal::factory::createTriggerRegistry())
	, _genericDispatcherInstancePt(
		  internal::factory::createGenericDispatcher(this))
	, _genericDispatcherPtr(&*_genericDispatcherInstancePt)
{
	{
		std::lock_guard<std::mutex> lock(_registryMutex);
		_registryStack.push_back(this);
		_registrySet.insert(this);
	}
	setEnabled(enabled);
}

// Do not throw error in destructor, it may crash program while closing.
LibmacroImpl::~LibmacroImpl()
{
	if (enabled()) {
		std::cerr
			<< "Error: Libmacro context was not disabled "
			   "before destruction.  Threading errors may occur.\n";
		std::cerr << "Warning: mcr_deinitialize errors are ignored."
			  << std::endl;
	}

	{
		std::lock_guard<std::mutex> lock(_registryMutex);
		for (auto iter = _registryStack.cbegin();
		     iter != _registryStack.cend(); iter++) {
			if (*iter == this) {
				_registryStack.erase(iter);
				break;
			}
		}
		_registrySet.erase(this);
	}
}

Libmacro *Libmacro::instance()
{
	std::lock_guard<std::mutex> lock(_registryMutex);
	if (_registryStack.empty())
		throw Error(EFAULT, "No Libmacro instance exists");
	return _registryStack.back();
}

bool Libmacro::hasInstance(Libmacro *contextContainerPtr)
{
	std::lock_guard<std::mutex> lock(_registryMutex);
	return _registrySet.find(contextContainerPtr) != _registrySet.end();
}

bool LibmacroImpl::blockableFlag() const
{
	return _blockableFlag;
}

void LibmacroImpl::setBlockableFlag(bool flag)
{
	_blockableFlag = flag;
}

bool LibmacroImpl::genericDispatchFlag() const
{
	return _genericDispatchFlag;
}

void LibmacroImpl::setGenericDispatchFlag(bool flag)
{
	_genericDispatchFlag = flag;
}

unsigned int LibmacroImpl::modifiers() const
{
	return _modifiers;
}

void LibmacroImpl::setModifiers(unsigned int mods)
{
	_modifiers = mods;
}

IDispatcher *LibmacroImpl::genericDispatcher() const
{
	return _genericDispatcherPtr;
}

void LibmacroImpl::setGenericDispatcher(IDispatcher *value)
{
	_genericDispatcherPtr = value;
}

ISerial &LibmacroImpl::serial()
{
	return *_serial;
}

const ISerial &LibmacroImpl::serial() const
{
	return *_serial;
}

IMacroRegistry &LibmacroImpl::macroRegistry()
{
	return *_macroRegistry;
}

const IMacroRegistry &LibmacroImpl::macroRegistry() const
{
	return *_macroRegistry;
}

ISignalRegistry &LibmacroImpl::signalRegistry()
{
	return *_signalRegistry;
}

const ISignalRegistry &LibmacroImpl::signalRegistry() const
{
	return *_signalRegistry;
}

ITriggerRegistry &LibmacroImpl::triggerRegistry()
{
	return *_triggerRegistry;
}

const ITriggerRegistry &LibmacroImpl::triggerRegistry() const
{
	return *_triggerRegistry;
}

bool LibmacroImpl::enabled() const
{
	return _enabled;
}

void LibmacroImpl::setEnabled(bool val)
{
	if (val == _enabled)
		return;
	_enabled = val;
}


}
