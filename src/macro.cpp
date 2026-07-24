/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/macro.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "mcr/api.h"
#include "mcr/error.h"
#include "mcr/libmacro.h"
#include "mcr/factory.h"

namespace mcr
{
/* Global thread management -- shared across all MacroImpl instances.
 * g_globalThreadMutex protects both g_threadMax and g_activeThreadCount. */
unsigned int g_threadMax = 0x10;
unsigned int g_activeThreadCount = 0;
std::mutex g_globalThreadMutex;


#ifdef signals
#undef signals
#endif

class MacroImpl : public IMacro {
    public:
	MacroImpl(Libmacro *context = nullptr);
	MacroImpl(const MacroImpl &other);
	virtual ~MacroImpl() override;
	inline MacroImpl &operator=(const MacroImpl &other)
	{
		copy(other);
		return *this;
	}

	virtual Libmacro *context() const override
	{
		return _context;
	}
	virtual void setContext(Libmacro *ctx) override
	{
		_context = ctx;
	}
	virtual bool globalActiveThreadCount() const noexcept override
	{
		std::lock_guard<std::mutex> lock(g_globalThreadMutex);
		return g_activeThreadCount;
	}

	virtual bool blocking() const override
	{
		return _blocking;
	}
	virtual void setBlocking(bool val) override;

	virtual IInterrupt::Value interruptor() const override
	{
		return _interruptor;
	}
	virtual void setInterruptor(IInterrupt::Value val) override;
	virtual bool act() override
	{
		start();
		return blocking();
	}

	virtual const char *name() const override;
	virtual void setName(const char *val) override;

	virtual bool sticky() const override
	{
		return _sticky;
	}
	virtual void setSticky(bool val) override;

	virtual unsigned int threadMax() const override
	{
		return _threadMax;
	}
	virtual void setThreadMax(unsigned int val) override;

	virtual void setActivators(const Signal *array,
				   mcr_index_t count) override;
	virtual void setSignals(const Signal *array,
				mcr_index_t count) override;
	virtual void setTriggers(const Trigger *array,
				 mcr_index_t count) override;

	virtual int threadCount() const override
	{
		return _threadCount;
	}
	virtual unsigned int queued() const override
	{
		return _queued;
	}

	virtual void start() override;
	virtual int run() override;

	virtual bool receive(Signal *signalPtr, unsigned int mods) override
	{
		(void)signalPtr;
		(void)mods;
		start();
		return blocking();
	}

	virtual void copy(const IMacro &other) override;
	virtual void copy(const MacroImpl &other);

	virtual void applyDispatch() override;
	virtual bool applyDispatchEnabled() const override;
	virtual void setApplyDispatchEnabled(bool val) override;
	virtual void addDispatch() override;
	virtual void removeDispatch() override;
	virtual void addDispatch(Signal &signalPtr) override;
	virtual void addDispatch(Trigger &trigPtr) override;
	virtual void addDispatch(Signal &signalPtr, Trigger &trigPtr) override;

    private:
	bool _blocking = false;
	IInterrupt::Value _interruptor = IInterrupt::DISABLE;
	bool _sticky = false;
	bool _applyDispatchFlag = true;
	unsigned int _threadCount = 0;
	unsigned int _threadMax = 1;
	std::atomic<int> _queued{0};
	Libmacro *_context = nullptr;
	std::vector<Signal *> _activators;
	std::string _name;
	std::vector<Signal *> _signals;
	std::vector<Trigger *> _triggers;
	std::condition_variable _cndThreadCount;
	std::mutex _mutex;
	std::vector<std::thread> _threads;

	inline int decQueued()
	{
		if (_queued <= 0)
			return _queued;
		--_queued;
		limitMaxThreads();
		return _queued;
	}
	inline int incQueued()
	{
		if (_queued < _threadMax) {
			++_queued;
			limitMaxThreads();
		}
		return _queued;
	}
	inline bool isIdle()
	{
		return _queued <= 0;
	}
	inline void dequeue(IInterrupt::Value val)
	{
		_queued = 0;
		_interruptor = val;
		_cndThreadCount.notify_all();
	}
	inline void limitMaxThreads()
	{
		if (!_threadMax) {
			_threadMax = 1;
		} else if (_threadMax > g_threadMax) {
			_threadMax = g_threadMax;
		}
		/* Oh no buddy! */
		if (_queued > _threadMax)
			_queued = 0;
	}

	void _removeDispatch();
	void _addDispatch();
	void _addDispatch(Signal &signalPtr);
	void _addDispatch(Trigger &trigPtr);
	void _addDispatch(Signal &signalPtr, Trigger &trigPtr);
	void _applyDispatch();

	int waitThreadsChanged();
	void notifyThreadsChanged();
	void disableEverything();
	void clearThreads(IInterrupt::Value clearType, bool stickyInterrupt);
	void resetThread();
	inline void pauseIfInterrupted()
	{
		if (interruptor() == PAUSE)
			pauseLoop();
	}
	bool consumeInterrupt();
	void pauseLoop();
	void onRunComplete();
	void joinAllThreads();
	void dispatchSignals();
	void fixupTriggerActors();
	void removeDispatcherTriggers(IDispatcher *dispatcher);
	void pairActivatorsAndTriggers();
	bool runPreamble();
	bool trySpawnThread();
	bool handleInterruptAll();
	void clearThreadsSticky(IInterrupt::Value clearType);
};

void IMacro::Deleter::operator()(IMacro *ptr) const
{
	delete ptr;
}

MacroImpl::MacroImpl(Libmacro *context)
	: _context(context)
{
}

MacroImpl::MacroImpl(const MacroImpl &other)
	: _context(other._context)
{
	copy(other);
}

MacroImpl::~MacroImpl()
{
	disableEverything();
}

void MacroImpl::setBlocking(bool val)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (val != _blocking)
		_blocking = val;
}

void MacroImpl::setInterruptor(IInterrupt::Value val)
{
	if (val < 0 || val > DISABLE)
		throw Error(EINVAL, "Invalid interruptor value");
	auto prev = interruptor();
	if (val != interruptor()) {
		bool spawnResetThread = false;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_interruptor = val;
			_cndThreadCount.notify_all();
			if (val == INTERRUPT_ALL)
				spawnResetThread = handleInterruptAll();
		}
		if (prev == DISABLE || val == DISABLE)
			applyDispatch();
		if (spawnResetThread) {
			std::this_thread::yield();
			std::thread(&MacroImpl::resetThread, this)
				.detach();
		}
	}
}

const char *MacroImpl::name() const
{
	return _name.c_str();
}

void MacroImpl::setName(const char *val)
{
	if (!val)
		throw Error(EFAULT, "Null name");
	std::lock_guard<std::mutex> lock(_mutex);
	if (val != _name)
		_name = val;
}

void MacroImpl::setSticky(bool val)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (val != _sticky)
		_sticky = val;
}

void MacroImpl::setThreadMax(unsigned int val)
{
	if (!val || val > g_threadMax)
		throw Error(EINVAL, "Invalid thread max");
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (val == _threadMax)
			return;
		_threadMax = val;
	}
	limitMaxThreads();
}

void MacroImpl::start()
{
	/* Check interruptor state and either spawn a thread, resume from
	 * INTERRUPT, or do nothing for INTERRUPT_ALL/DISABLE. */
	std::unique_lock<std::mutex> lock(_mutex);
	switch (interruptor()) {
	case CONTINUE:
	case PAUSE:
		if (_threadCount < _threadMax) {
			if (!trySpawnThread()) {
				lock.unlock();
				limitMaxThreads();
			}
		}
		break;
	case INTERRUPT:
		_interruptor = CONTINUE;
		_cndThreadCount.notify_all();
		break;
	case INTERRUPT_ALL:
	case DISABLE:
		break;
	}
}

int MacroImpl::run()
{
	/* Main execution loop. Each iteration:
	 * 1. Check for interrupts
	 * 2. If idle and not sticky (or multiple threads), exit
	 * 3. Decrement queue and dispatch signals
	 * 4. Pause if interrupted
	 * Loops until interrupted, disabled, or idle. */
	if (!runPreamble())
		return 0;
	try {
		do {
			if (consumeInterrupt())
				break;
			if (interruptor() >= INTERRUPT_ALL)
				break;
			if (isIdle()) {
				if (!sticky() || threadCount() > 1)
					break;
			} else {
				decQueued();
			}
			dispatchSignals();
			pauseIfInterrupted();
		} while (true);
	} catch (...) {
	}
	onRunComplete();
	return 0;
}

void MacroImpl::copy(const IMacro &other)
{
	auto &ref = dynamic_cast<const MacroImpl &>(other);
	if (this == &ref)
		return;
	copy(ref);
}

void MacroImpl::copy(const MacroImpl &other)
{
	disableEverything();
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_activators = other._activators;
		_name = other._name;
		_signals = other._signals;
		_triggers = other._triggers;
		_blocking = other._blocking;
		_interruptor = other._interruptor;
		_sticky = other._sticky;
		_threadMax = other._threadMax;
		_context = other._context;
		_applyDispatchFlag = other._applyDispatchFlag;
		fixupTriggerActors();
	}
	limitMaxThreads();
	applyDispatch();
}

void MacroImpl::setActivators(const Signal *array, mcr_index_t count)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_removeDispatch();
	_activators.clear();
	for (mcr_index_t i = 0; i < count; i++) {
		_activators.push_back(const_cast<Signal *>(&array[i]));
	}
	_applyDispatch();
}

void MacroImpl::setSignals(const Signal *array, mcr_index_t count)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_threadCount > 0)
		throw Error(EBUSY, "Cannot modify signals while macro is running");
	_signals.clear();
	for (mcr_index_t i = 0; i < count; i++) {
		_signals.push_back(const_cast<Signal *>(&array[i]));
	}
}

void MacroImpl::setTriggers(const Trigger *array, mcr_index_t count)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_removeDispatch();
	_triggers.clear();
	for (mcr_index_t i = 0; i < count; i++) {
		auto trig = const_cast<Trigger *>(&array[i]);
		_triggers.push_back(trig);
		trig->actorPtr = this;
	}
	_applyDispatch();
}

void MacroImpl::_removeDispatch()
{
	std::unordered_set<void *> dispset;
	auto *gdisp = _context->genericDispatcher();
	if (gdisp)
		gdisp->remove(this);
	for (auto *signalPtr : _activators) {
		auto *dispatcher = signalPtr->dispatcherPtr;
		if (!dispatcher)
			continue;
		if (dispset.find(dispatcher) != dispset.end())
			continue;
		dispset.insert(dispatcher);
		dispatcher->remove(this);
		removeDispatcherTriggers(dispatcher);
	}
}

void MacroImpl::_addDispatch(Signal &signalPtr)
{
	auto *dispatcher = signalPtr.dispatcherPtr;
	if (!dispatcher)
		return;
	dispatcher->add(&signalPtr, this);
}

void MacroImpl::_addDispatch(Trigger &trigPtr)
{
	auto *genericDispatcher = _context->genericDispatcher();
	if (!genericDispatcher)
		return;
	genericDispatcher->add(nullptr, &trigPtr);
}

void MacroImpl::_addDispatch(Signal &signalPtr, Trigger &trigPtr)
{
	auto *dispatcher = signalPtr.dispatcherPtr;
	if (!dispatcher)
		return;
	dispatcher->add(&signalPtr, &trigPtr);
}

void MacroImpl::_addDispatch()
{
	if (_activators.empty()) {
		if (_triggers.empty()) {
			auto *gdisp = _context->genericDispatcher();
			if (!gdisp)
				return;
			gdisp->add(nullptr, this);
		} else {
			for (auto *trig : _triggers) {
				_addDispatch(*trig);
			}
		}
	} else {
		pairActivatorsAndTriggers();
	}
}

void MacroImpl::_applyDispatch()
{
	if (!_applyDispatchFlag)
		return;
	_removeDispatch();
	if (enabled())
		_addDispatch();
}

bool MacroImpl::applyDispatchEnabled() const
{
	return _applyDispatchFlag;
}

void MacroImpl::setApplyDispatchEnabled(bool val)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_applyDispatchFlag = val;
}

void MacroImpl::applyDispatch()
{
	std::lock_guard<std::mutex> lock(_mutex);
	_applyDispatch();
}

void MacroImpl::addDispatch()
{
	std::lock_guard<std::mutex> lock(_mutex);
	_addDispatch();
}

void MacroImpl::removeDispatch()
{
	std::lock_guard<std::mutex> lock(_mutex);
	_removeDispatch();
}

void MacroImpl::addDispatch(Signal &signalPtr)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_addDispatch(signalPtr);
}

void MacroImpl::addDispatch(Trigger &trigPtr)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_addDispatch(trigPtr);
}

void MacroImpl::addDispatch(Signal &signalPtr, Trigger &trigPtr)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_addDispatch(signalPtr, trigPtr);
}

int MacroImpl::waitThreadsChanged()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cndThreadCount.wait(lock, [this] { return !_threadCount; });
	return 0;
}

void MacroImpl::notifyThreadsChanged()
{
	_cndThreadCount.notify_all();
}

void MacroImpl::disableEverything()
{
	dequeue(DISABLE);
	_removeDispatch();
	clearThreads(DISABLE, true);
}

void MacroImpl::clearThreads(IInterrupt::Value clearType, bool stickyInterrupt)
{
	if (clearType == CONTINUE || clearType == PAUSE)
		throw Error(EINVAL, "Invalid clear type: CONTINUE or PAUSE");
	if (stickyInterrupt && clearType == INTERRUPT)
		throw Error(EINVAL, "Invalid clear type: sticky with INTERRUPT");
	dequeue(clearType);
	if (!stickyInterrupt) {
		waitThreadsChanged();
	} else {
		clearThreadsSticky(clearType);
	}
	joinAllThreads();
}

void MacroImpl::resetThread()
{
	const auto prev = _interruptor;
	dequeue(INTERRUPT_ALL);
	std::this_thread::yield();
	if (running())
		clearThreads(INTERRUPT_ALL, false);
	if (prev != DISABLE)
		setInterruptor(CONTINUE);
}

void MacroImpl::pauseLoop()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cndThreadCount.wait(lock, [this] { return interruptor() != PAUSE; });
}

void MacroImpl::onRunComplete()
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		--_threadCount;
	}
	{
		std::lock_guard<std::mutex> globalLock(g_globalThreadMutex);
		if (g_activeThreadCount > 0)
			--g_activeThreadCount;
	}
	notifyThreadsChanged();
}

bool MacroImpl::consumeInterrupt()
{
	if (interruptor() != IInterrupt::INTERRUPT)
		return false;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (interruptor() != IInterrupt::INTERRUPT)
			return false;
	}
	setInterruptor(IInterrupt::CONTINUE);
	return true;
}

void MacroImpl::joinAllThreads()
{
	const auto thisId = std::this_thread::get_id();
	for (auto &t : _threads) {
		if (!t.joinable())
			continue;
		if (t.get_id() == thisId)
			t.detach();
		else
			t.join();
	}
	_threads.clear();
}

void MacroImpl::dispatchSignals()
{
	for (auto *signalPtr : _signals) {
		if (interruptor() >= INTERRUPT_ALL)
			break;
		if (interruptor() >= INTERRUPT)
			continue;
		if (!signalPtr)
			continue;
		auto *dispatcher = signalPtr->dispatcherPtr;
		if (dispatcher) {
			dispatcher->dispatch(signalPtr, 0);
		} else {
			signalPtr->send();
		}
	}
}

bool MacroImpl::runPreamble()
{
	{
		std::lock_guard<std::mutex> lock(_mutex);
		++_threadCount;
	}
	notifyThreadsChanged();
	if (consumeInterrupt()) {
		onRunComplete();
		return false;
	}
	if (_signals.empty()) {
		onRunComplete();
		return false;
	}
	pauseIfInterrupted();
	return true;
}

bool MacroImpl::trySpawnThread()
{
	{
		std::lock_guard<std::mutex> globalLock(g_globalThreadMutex);
		if (g_activeThreadCount >= g_threadMax) {
			_queued = 0;
			return false;
		}
		++g_activeThreadCount;
	}

	try {
		if (incQueued() > _threadMax || !_queued) {
			std::lock_guard<std::mutex> globalLock(g_globalThreadMutex);
			--g_activeThreadCount;
			_queued = 0;
			return false;
		}
		_threads.emplace_back(&MacroImpl::run, this);
	} catch (...) {
		std::lock_guard<std::mutex> globalLock(g_globalThreadMutex);
		--g_activeThreadCount;
		return false;
	}
	return true;
}

bool MacroImpl::handleInterruptAll()
{
	dequeue(INTERRUPT_ALL);
	if (running())
		return true;
	_interruptor = CONTINUE;
	_cndThreadCount.notify_all();
	return false;
}

void MacroImpl::clearThreadsSticky(IInterrupt::Value clearType)
{
	std::cv_status threadStatus;
	std::this_thread::yield();
	if (running()) {
		do {
			dequeue(clearType);
			threadStatus = static_cast<std::cv_status>(
				waitThreadsChanged());
		} while (running() &&
			 threadStatus != std::cv_status::timeout);
	}
	if (running() && threadStatus == std::cv_status::timeout) {
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_threadCount = 0;
		}
		notifyThreadsChanged();
	}
}

void MacroImpl::fixupTriggerActors()
{
	for (auto *trig : _triggers) {
		if (!trig)
			continue;
		trig->actorPtr = this;
	}
}

void MacroImpl::removeDispatcherTriggers(IDispatcher *dispatcher)
{
	for (auto *trigPtr : _triggers) {
		dispatcher->remove(trigPtr);
	}
}

void MacroImpl::pairActivatorsAndTriggers()
{
	for (auto *activator : _activators) {
		if (_triggers.empty()) {
			_addDispatch(*activator);
		} else {
			for (auto *trigger : _triggers) {
				_addDispatch(*activator, *trigger);
			}
		}
	}
}

namespace factory
{

std::unique_ptr<IMacro, IMacro::Deleter> createMacro(Libmacro *context)
{
	return std::unique_ptr<IMacro, IMacro::Deleter>(new MacroImpl(context));
}

std::shared_ptr<IMacro> createMacroShared(Libmacro *context)
{
	return createMacro(context);
}
}

void Libmacro::setGlobalThreadLimit(unsigned int val)
{
	g_threadMax = val;
}

unsigned int Libmacro::globalThreadLimit() const
{
	return g_threadMax;
}
}
