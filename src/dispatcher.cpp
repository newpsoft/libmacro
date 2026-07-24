/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/dispatcher.h"
#include "mcr/factory.h"

#include <unordered_set>

namespace mcr
{

/*! Generic dispatcher type for any signal type. Will always dispatch to every
 *  receiver. The _receivers set stores all registered IReceive pointers.
 *  Dispatch iterates the set and calls receive() on each; if any receiver
 *  returns true (block), dispatch stops immediately. This is a simple
 *  broadcast dispatcher with no signal-type filtering -- all signals reach
 *  all receivers. */
class MCR_API Dispatcher final : public IDispatcher {
    public:
	Libmacro *context = nullptr;

	Dispatcher(Libmacro *context = nullptr);
	Dispatcher(const Dispatcher &other);
	virtual ~Dispatcher() = default;
	Dispatcher &operator=(const Dispatcher &other);

	virtual void add(Signal *signalPtr, IReceive *receiverPtr) override;
	virtual void clear() noexcept override;
	virtual bool dispatch(Signal *, unsigned int) final override;
	virtual void modifier(Signal *, unsigned int *) noexcept override {}
	virtual void remove(IReceive *removeReceiverPtr) override;
	virtual void trim() noexcept override
	{
	}
	virtual mcr_index_t count() const noexcept override;

    private:
	std::unordered_set<IReceive *> _receivers;
};

void IDispatcher::Deleter::operator()(IDispatcher *ptr) const
{
	delete ptr;
}

namespace internal
{
namespace factory
{

std::unique_ptr<IDispatcher, IDispatcher::Deleter>
createGenericDispatcher(Libmacro *context)
{
	return std::unique_ptr<IDispatcher, IDispatcher::Deleter>(
		new Dispatcher(context));
}

std::shared_ptr<IDispatcher> createGenericDispatcherShared(Libmacro *context)
{
	return createGenericDispatcher(context);
}

}
}

Dispatcher::Dispatcher(Libmacro *libmacroPtr)
	: IDispatcher()
	, context(libmacroPtr)
{
}

Dispatcher::Dispatcher(const Dispatcher &other)
	: context(other.context)
{
}

Dispatcher &Dispatcher::operator=(const Dispatcher &other)
{
	if (&other == this)
		return *this;
	context = other.context;
	return *this;
}

void Dispatcher::add(Signal *, IReceive *receiverPtr)
{
	_receivers.insert(receiverPtr);
}

void Dispatcher::clear() noexcept
{
	_receivers.clear();
}

bool Dispatcher::dispatch(Signal *signalPtr, unsigned int mods)
{
	/* Broadcast signal to all receivers. If any receiver returns true
	 * (block), stop immediately and return true. Otherwise return false
	 * (not blocked). */
	bool blockFlag = false;
	for (auto iter : _receivers) {
		if (!iter)
			continue;
		if (iter->receive(signalPtr, mods)) {
			blockFlag = true;
			break;
		}
	}
	return blockFlag;
}

void Dispatcher::remove(IReceive *removeReceiverPtr)
{
	if (!removeReceiverPtr)
		return;
	_receivers.erase(removeReceiverPtr);
}

mcr_index_t Dispatcher::count() const noexcept
{
	return _receivers.size();
}
}
