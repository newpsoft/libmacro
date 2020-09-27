/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mcr/extras/extras.h"

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define priv _private

extern "C" bool mcr_Macro_receive(struct mcr_DispatchReceiver *dispPt,
								  struct mcr_Signal *, unsigned int)
{
	dassert(dispPt);
	dassert(dispPt->receiver);
	return mcr::reference<mcr::Macro>(dispPt->receiver)();
}

namespace mcr
{
/* Declare this scope uses triggers and activators, and will block
 * until locked.
 * Always call property change outside of critical section. */
#define CriticalBlock std::lock_guard<std::mutex> \
	criticalBlockGuard(priv->mutex)
/* Declare this scope uses triggers and activators, but will not block
 * if already locked. */
#define CriticalSoftBlock std::unique_lock<std::mutex> \
	criticalSoftBlockGuard(priv->mutex, std::try_to_lock)

#ifdef signals
#undef signals
#endif

class MacroPrivate
{
	friend class Macro;
public:
	std::vector<Signal> activators;
	std::string name;
	std::vector<Signal> signals;
	std::vector<Trigger> triggers;
	std::condition_variable cndThreadCount;
	std::mutex mutex;

	MacroPrivate() = default;
	virtual ~MacroPrivate() = default;
	MacroPrivate(const MacroPrivate &copytron)
	{
		activators = copytron.activators;
		name = copytron.name;
		signals = copytron.signals;
		triggers = copytron.triggers;
	}
	MacroPrivate &operator =(const MacroPrivate &copytron)
	{
		if (&copytron != this) {
			activators = copytron.activators;
			name = copytron.name;
			signals = copytron.signals;
			triggers = copytron.triggers;
		}
		return *this;
	}
};

Macro::Macro(Libmacro *context, bool blocking, bool sticky, unsigned int threadMax, bool enable)
	: _context(Libmacro::instance(context)), priv(new MacroPrivate)
{
	ctor(blocking, sticky, threadMax);
	limitMaxThreads();
	setEnabled(enable);
	/* Nothing to dispatch yet */
}

Macro::Macro(const Macro &copytron)
	: _context(copytron._context), priv(new MacroPrivate)
{
	ctor(copytron.blocking(), copytron.sticky(), copytron.threadMax());
	copy(copytron);
}

Macro::~Macro()
{
	/* If enabled while destroying we have threading problems */
	disableEverything();
	delete priv;
}

void Macro::setBlocking(bool val)
{
	/* Not critical */
	if (val != blocking()) {
		_blocking = val;
		callProperty(blockingChanged);
	}
}

// \todo setInterruptor, lookup if critical
void Macro::setInterruptor(Interrupt val)
{
	mcr_throwif(val < 0 || val > DISABLE, EINVAL);
	Interrupt prev = interruptor();
	if (val != interruptor()) {
		{
			CriticalBlock;
			_interruptor = val;
			if (prev == DISABLE || val == DISABLE)
				applyDispatch();
			/* CONTINUE - No threads previous
			 * PAUSE - Threads do not stop, only wait to be unpaused.
			 * INTERRUPT - One thread does nothing, and will reenable.
			 * INTERRUPT_ALL - Wait for all to finish, then reenable.
			 * Disable - Let threads run their course. No action is needed. */
			if (val == INTERRUPT_ALL) {
				dequeue(INTERRUPT_ALL);
				std::this_thread::yield();
				if (running())
					std::thread(&Macro::resetThread, this).detach();
			}
		}
		callProperty(interruptorChanged);
		if (prev == DISABLE || val == DISABLE)
			callProperty(enabledChanged);
	}
}

const char *Macro::name() const
{
	return priv->name.c_str();
}

void Macro::setName(const char *val)
{
	/* Not critical */
	mcr_throwif(!val, EFAULT);
	if (val != priv->name) {
		priv->name = val;
		callProperty(nameChanged);
	}
}

void Macro::setSticky(bool val)
{
	/* Not critical */
	if (val != sticky()) {
		_sticky = val;
		callProperty(stickyChanged);
	}
}

void Macro::setThreadMax(unsigned int val)
{
	/* Not critical */
	/* Non-threaded not yet supported */
	mcr_throwif(!val || val > MCR_THREAD_MAX, EINVAL);
	if (val != _threadMax) {
		_threadMax = val;
		callProperty(threadMaxChanged);
	}
}

void Macro::setActivators(const Signal *array, size_t count)
{
	{
		CriticalBlock;
		auto &activators = priv->activators;
		if (enabled())
			removeDispatch();
		activators.resize(count);
		mcr_throwif(count && !array, EINVAL);
		for (size_t i = 0; i < count; i++) {
			activators[i] = array[i];
		}
		if (enabled())
			addDispatch();
	}
	callProperty(activatorsChanged);
}

void Macro::setSignals(const Signal *array, size_t count)
{
	{
		CriticalBlock;
		/*! special condition, thread cannot lock while we are, and we cannot
		 *  change signals while running */
		mcr_throwif(enabled(), EBUSY);
		mcr_throwif(running(), EBUSY);

		auto &signals = priv->signals;
		signals.resize(count);
		mcr_throwif(count && !array, EINVAL);
		for (size_t i = 0; i < count; i++) {
			signals[i] = array[i];
		}
	}
	callProperty(signalsChanged);
}

void Macro::setTriggers(const Trigger *array, size_t count)
{
	{
		CriticalBlock;
		auto &triggers = priv->triggers;
		if (enabled())
			removeDispatch();
		triggers.resize(count);
		mcr_throwif(count && !array, EINVAL);
		for (size_t i = 0; i < count; i++) {
			triggers[i] = array[i];
			triggerMe(&*triggers[i]);
		}
		if (enabled())
			addDispatch();
	}
	callProperty(triggersChanged);
}

void Macro::start()
{
	/* Want to lock, but we have to unlock for property. */
	std::unique_lock<std::mutex> lock(priv->mutex);
	switch (interruptor()) {
	case CONTINUE:
	case PAUSE:
		/* Do not add to queue count if thread max is reached */
		if (_threadCount < MCR_THREAD_MAX && _threadCount < _threadMax) {
			//! \todo Possible unused threads if only one queued item.
			if (incQueued() > _threadMax || !_queued) {
				dmsg;
				_queued = 0;
				lock.unlock();
				callProperty(queuedChanged);
			} else {
				std::thread(&Macro::run, this).detach();
			}
		}
		break;
	case INTERRUPT:
		/* One or more has now been interrupted (canceled). */
		_interruptor = CONTINUE;
		break;
	case INTERRUPT_ALL:
	case DISABLE:
		break;
	}
}

void Macro::run()
{
	/* Thread count first so all threads know we are running. */
	++_threadCount;
	notifyThreadsChanged();
	callProperty(threadCountChanged);

	auto &signals = priv->signals;

	/* Verification */
	if (interruptor() == INTERRUPT) {
		priv->mutex.lock();
		if (interruptor() == INTERRUPT) {
			priv->mutex.unlock();
			setInterruptor(CONTINUE);
			onRunComplete();
			return;
		}
		priv->mutex.unlock();
	}
	if (!signals.size() || interruptor() >= INTERRUPT_ALL) {
		/* No signal set is grounds for immediate dismissal. */
		if (!signals.size()) {
			_queued = 0;
			callProperty(queuedChanged);
		}
		onRunComplete();
		return;
	}
	/* Verification complete */

	onCanPause();
	do {
		/* Only interrupt one, or all concurrent? */
		if (interruptor() == INTERRUPT) {
			priv->mutex.lock();
			if (interruptor() == INTERRUPT) {
				priv->mutex.unlock();
				setInterruptor(CONTINUE);
				break;
			}
			priv->mutex.unlock();
		}
		if (interruptor() >= INTERRUPT_ALL)
			break;
		// Safety
		if (_queued > _threadMax) {
			_queued = 0;
			callProperty(queuedChanged);
			break;
		}
		// Only dequeue if not sticky or have extras
		if (!sticky() || _queued > 0 || threadCount() > 1) {
			if (!_queued || decQueued() == 0)
				break;
		}
		for (auto &iter: signals) {
			mcr_send(&*context(), &*iter);
			/* Go to interrupt check phase */
			if (interruptor() >= INTERRUPT)
				continue;
		}
		onCanPause();
	} while (true);
	onRunComplete();
}

void Macro::copy(const Macro &copytron)
{
	if (&copytron != this) {
		disableEverything();
		{
			CriticalBlock;
			_blocking = copytron._blocking;
			_interruptor = copytron._interruptor;
			_sticky = copytron._sticky;
			_threadMax = copytron._threadMax;
			limitMaxThreads();
			*priv = *copytron.priv;
			for (auto &iter: priv->triggers) {
				triggerMe(&*iter);
			}
			applyDispatch();
		}
		objectChanged();
	}
}

void Macro::applyDispatch()
{
	CriticalSoftBlock;
	removeDispatch();
	if (enabled())
		addDispatch();
}

void Macro::addDispatch()
{
	CriticalSoftBlock;
	auto &activators = priv->activators;
	auto &triggers = priv->triggers;
	if (activators.empty()) {
		if (triggers.empty()) {
			// All generic
			if (mcr_dispatch_add_generic(&*context(), NULL, this, receive))
				throw mcr_read_err();
		} else {
			// Generic to trigger
			for (auto &triggy: triggers) {
				addDispatch(triggy);
			}
		}
	} else {
		for (auto &i: activators) {
			if (triggers.empty()) {
				// Add specific to Macro as receiver (no modifier logic etc.)
				addDispatch(i);
			} else {
				for (auto &j: triggers) {
					addDispatch(i, j);
				}
			}
		}
	}
}

void Macro::removeDispatch()
{
	CriticalSoftBlock;
	mcr_dispatch_remove_all(&*context(), this);
	for (auto &trigPt: priv->triggers) {
		mcr_dispatch_remove_all(&*context(), &trigPt);
	}
}

void Macro::addDispatch(Signal &sigPt)
{
	CriticalSoftBlock;
	if (mcr_dispatch_add(&*context(), &*sigPt, this, receive))
		throw mcr_read_err();
}

void Macro::addDispatch(Trigger &trigPt)
{
	CriticalSoftBlock;
	if (mcr_dispatch_add_generic(&*context(), nullptr, &*trigPt, mcr_Trigger_receive))
		throw mcr_read_err();
}

void Macro::addDispatch(Signal &sigPt, Trigger &trigPt)
{
	CriticalSoftBlock;
	if (mcr_dispatch_add(&*context(), &*sigPt, &*trigPt, mcr_Trigger_receive))
		throw mcr_read_err();
}

int Macro::waitThreadsChanged()
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	std::cv_status unu = priv->cndThreadCount.wait_for(lock, std::chrono::seconds(MCR_MACRO_JOIN_TIMEOUT));
	return static_cast<int>(unu);
}

void Macro::notifyThreadsChanged()
{
	priv->cndThreadCount.notify_all();
}

void Macro::ctor(bool blocking, bool sticky, unsigned int threadMax)
{
	_blocking = blocking;
	_interruptor = DISABLE;
	_sticky = sticky;
	_threadCount = 0;
	_threadMax = threadMax;
	limitMaxThreads();
	_queued = 0;
}

void Macro::disableEverything()
{
	dequeue(DISABLE);
	removeDispatch();
	clearThreads(DISABLE, true);
	callProperty(interruptorChanged);
	callProperty(enabledChanged);
}

void Macro::clearThreads(Interrupt clearType, bool stickyInterrupt)
{
	//! \todo Do not rely on a timeout to say no threads exist
	mcr_throwif(clearType == CONTINUE || clearType == PAUSE, EINVAL);
	mcr_throwif(stickyInterrupt && clearType == INTERRUPT, EINVAL);
	std::cv_status thrdErr;
	dequeue(clearType);
	/* If not sticky, do not apply in a loop, only wait once */
	if (!stickyInterrupt) {
		waitThreadsChanged();
		return;
	}
	std::this_thread::yield();
	/* If threads run smoothly then we continue to apply our interrupt until
	 * no threads are left.  If we are timed out while waiting that means
	 * one signal is blocking the whole thread, and we have lost control. */
	if (running()) do {
		dequeue(clearType);
		thrdErr = static_cast<std::cv_status>(waitThreadsChanged());
	} while (running() && thrdErr != std::cv_status::timeout);

	/* If timed out, we lose control over threads. */
	if (running() && thrdErr == std::cv_status::timeout) {
		dmsg;
		_threadCount = 0;
		notifyThreadsChanged();
		callProperty(threadCountChanged);
	}
}

void Macro::resetThread()
{
	dequeue(INTERRUPT_ALL);
	callProperty(interruptorChanged);
	callProperty(enabledChanged);
	std::this_thread::yield();
	if (running())
		clearThreads(INTERRUPT_ALL, false);
	if (enabled())
		setInterruptor(CONTINUE);
}

void Macro::pauseLoop()
{
	unsigned int pauseIter = MCR_MAX_PAUSE_COUNT + 1;
	while (interruptor() == PAUSE && --pauseIter) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	if (interruptor() == PAUSE)
		setInterruptor(CONTINUE);
}

void Macro::onRunComplete()
{
	--_threadCount;
	notifyThreadsChanged();
	callProperty(threadCountChanged);
}
}
