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

/*! \file
 */

#ifndef MCR_EXTRAS_MACRO_H_
#define MCR_EXTRAS_MACRO_H_

#include "mcr/extras/wrappers.h"

namespace mcr
{
/*! Non-exportable members */
class MacroPrivate;
/*!
 *  All set and threaded run functions may throw std::logic_error
 *
 *  Virtual functions: objectChanged, start, clearAll, copy\n
 *  Has a mutex locking interruptor, enabled, activators, and triggers, which
 *  are all mutually exclusive.
 */
class MCR_API Macro
{
	friend class MacroPrivate;
public:
	/*! Pause, stop, or continue a running macro */
	enum Interrupt {
		/*! Able to trigger, or continue currently running */
		CONTINUE = 0,
		/*! Briefly stop sending signals. */
		PAUSE,
		/*! One queued macro from one thread will cancel.
		 *  After one is cancelled, CONTINUE is set. */
		INTERRUPT,
		/*! All threads and queued items will cancel.
		 *  After all are cancelled, CONTINUE is set. */
		INTERRUPT_ALL,
		/*! Not able to trigger */
		DISABLE
	};

	static inline bool receive(struct mcr_DispatchReceiver *dispPt,
							   struct mcr_Signal *, unsigned int)
	{
		mcr_throwif(!dispPt || !dispPt->receiver, EFAULT);
		return reference<Macro>(dispPt->receiver)();
	}
	static inline bool trigger(struct mcr_Trigger *trigPt, struct mcr_Signal *,
							   unsigned int)
	{
		mcr_throwif(!trigPt || !trigPt->actor, EFAULT);
		return reference<Macro>(trigPt->actor)();
	}

	Macro(Libmacro *context = nullptr, bool blocking = false,
		  bool sticky = false, unsigned int threadMax = 1,
		  bool enable = false);
	Macro(const Macro &copytron);
	virtual ~Macro();
	inline Macro &operator =(const Macro &copytron)
	{
		copy(copytron);
		return *this;
	}

	inline bool operator()()
	{
		start();
		return blocking();
	}

	/* Properties changed */
	typedef void (*propertyChanged)(void *);

	void *propertyReceiver = nullptr;
	propertyChanged blockingChanged = nullptr;
	propertyChanged enabledChanged = nullptr;
	propertyChanged interruptorChanged = nullptr;
	propertyChanged nameChanged = nullptr;
	propertyChanged stickyChanged = nullptr;
	propertyChanged threadMaxChanged = nullptr;
	/* arrays */
	propertyChanged activatorsChanged = nullptr;
	propertyChanged signalsChanged = nullptr;
	propertyChanged triggersChanged = nullptr;
	/* read-only */
	propertyChanged threadCountChanged = nullptr;
	propertyChanged queuedChanged = nullptr;

	inline void callProperty(propertyChanged propertyFn)
	{
		// safety
		if (_threadMax > MCR_THREAD_MAX)
			_threadMax = MCR_THREAD_MAX;
		mcr_throwif(_threadCount > MCR_THREAD_MAX, -1);
		if (_queued > _threadMax)
			_queued = 0;
		if (propertyFn)
			propertyFn(propertyReceiver);
	}
	virtual inline void objectChanged()
	{
		/* When everything is changed always notify lists and read-only
		 * properties */
		callProperty(activatorsChanged);
		callProperty(signalsChanged);
		callProperty(triggersChanged);
		callProperty(threadCountChanged);
		callProperty(queuedChanged);
	}

	inline Libmacro &context() const
	{
		return *_context;
	}

	inline bool blocking() const
	{
		return _blocking;
	}
	void setBlocking(bool val);

	inline bool enabled() const
	{
		return _interruptor != DISABLE;
	}
	void setEnabled(bool val)
	{
		setInterruptor(val ? CONTINUE : DISABLE);
	}

	inline Interrupt interruptor() const
	{
		return _interruptor;
	}
	void setInterruptor(Interrupt val);

	const char *name() const;
	void setName(const char *val);

	inline bool sticky() const
	{
		return _sticky;
	}
	void setSticky(bool val);

	inline unsigned int threadMax() const
	{
		return _threadMax;
	}
	void setThreadMax(unsigned int val);

	void setActivators(const Signal *array, size_t count);
	/*! \param vals requires size and front reference function */
	template<class T>
	inline void setActivators(const T &vals)
	{
		setActivators(&vals.front(), vals.size());
	}
	inline void clearActivators()
	{
		setActivators(NULL, 0);
	}

	/*! Cannot change signals while enabled. */
	void setSignals(const Signal *array, size_t count);
	/*! \param vals requires size and front reference function */
	template<class T>
	inline void setSignals(const T &vals)
	{
		setSignals(&vals.front(), vals.size());
	}
	inline void clearSignals()
	{
		setSignals(NULL, 0);
	}

	void setTriggers(const Trigger *array, size_t count);
	/*! \param vals requires size and for-each iterator */
	template<class T>
	inline void setTriggers(const T &vals)
	{
		setTriggers(&vals.front(), vals.size());
	}
	inline void clearTriggers()
	{
		setTriggers(NULL, 0);
	}

	/* Read-only */
	inline int threadCount() const
	{
		return _threadCount;
	}
	inline unsigned queued() const
	{
		return _queued;
	}

	inline void clearAll()
	{
		clearActivators();
		clearSignals();
		clearTriggers();
	}

	//! Create a thread and call run.
	virtual void start();
	//! To be run inside separate thread.  Call start to run this function.
	virtual void run();
	inline bool running()
	{
		return _threadCount;
	}
	virtual void copy(const Macro &copytron);

	inline void triggerMe(mcr_Trigger *trigPt)
	{
		mcr_throwif(!trigPt, EFAULT);
		triggerMe(*trigPt);
	}
	inline void triggerMe(mcr_Trigger &trigger)
	{
		trigger.actor = this;
		trigger.trigger = Macro::trigger;
	}

	/*! If enabled addDispatch, otherwise removeDispatch */
	void applyDispatch();
	/*! Dispatch to currently set signals and triggers
	 *
	 *  Will not remove dispatch first, so may cause a multiple-dispatch
	 *  error. */
	void addDispatch();
	/*! Removes this macro and all triggers from active dispatching */
	void removeDispatch();
	// Maybe needs to be private
	//! Dispatch specific signal to this Macro as receiver
	void addDispatch(Signal &sigPt);
	//! Dispatch any signal to a trigger as receiver
	void addDispatch(Trigger &trigPt);
	//! Dispatch specific signal to a trigger as receiver
	void addDispatch(Signal &sigPt, Trigger &trigPt);

protected:
	inline unsigned int decQueued()
	{
		if (_queued > 0) {
			--_queued;
			if (_queued < 0)
				_queued = 0;
			callProperty(queuedChanged);
		}
		return _queued;
	}
	inline unsigned int incQueued()
	{
		if (_queued < _threadMax) {
			++_queued;
			callProperty(queuedChanged);
		}
		return _queued;
	}
	inline bool noQueued()
	{
		if (_queued < 0)
			_queued = 0;
		return _queued <= 0;
	}
	// \todo threadMax, interruptor, and enabled changed property
	/*! Will not notify property changed. */
	inline void dequeue(Interrupt val)
	{
		_queued = 0;
		_interruptor = val;
	}
	/*! Will not notify property changed. */
	inline void limitMaxThreads()
	{
		if (!_threadMax) {
			_threadMax = 1;
		} else if (_threadMax > MCR_THREAD_MAX) {
			_threadMax = MCR_THREAD_MAX;
		}
	}

	/*! Block until notifyThreadsChanged is called from another thread
	 *  \return std::cv_status */
	int waitThreadsChanged();
	/*! Wake up waitThreadsChanged */
	void notifyThreadsChanged();

private:
	Libmacro *_context;

	bool _blocking;
	Interrupt _interruptor;
	bool _sticky;
	unsigned int _threadCount;
	unsigned int _threadMax;
	unsigned int _queued;
	/* non-export */
	MacroPrivate *_private;

	/*! \ref ctor Start disabled, enable after this */
	void ctor(bool blocking = false, bool sticky = false, unsigned int threadMax = 1);

	void disableEverything();
	/*! Apply an interrupt and wait for threads to close.
	 *
	 *  \param clearType Apply this interrupt.  Throws an error if CONTINUE or
	 *  PAUSE and not something to close threads.
	 *  \param stickyInterrupt If true the clearType interrupt will continue
	 *  apply until successful. If true, then clearType also cannot be
	 *  INTERRUPT, since all threads cannot clear out. */
	void clearThreads(Interrupt clearType, bool stickyInterrupt);

	/*! Wait for all to finish with INTERRUPT_ALL, then reenable. */
	void resetThread();
	inline void onCanPause()
	{
		if (interruptor() == PAUSE)
			pauseLoop();
	}
	/*! Wait for interruptor to unpause, or reenable after
	 *  \ref MCR_MAX_PAUSE_COUNT has been reached. */
	void pauseLoop();
	void onRunComplete();
};
}
#endif
