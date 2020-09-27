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
 *  \brief C++ wrappers and extensions of C types.
 */

#ifndef MCR_EXTRAS_WRAPPERS_H_
#define MCR_EXTRAS_WRAPPERS_H_

#include "mcr/extras/base_cpp.h"

namespace mcr
{
/*!
 */
struct MCR_API ISignal
{
	mcr_ISignal self;
	inline mcr_Interface &interface()
	{
		return self.interface;
	}

	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 *  \param send \ref mcr_ISignal.send
	 *  \param dispatcher \ref opt mcr_ISignal.dispatcher
	 *  exist.
	 */
	ISignal(Libmacro *context = nullptr, mcr_signal_fnc send = nullptr,
			   mcr_IDispatcher *dispatcher = nullptr);
	ISignal(const ISignal &) = default;
	virtual ~ISignal()
	{
		mcr_ISignal_deinit(&self);
	}
	ISignal &operator =(const ISignal &copytron) = default;

	inline mcr_ISignal &operator *()
	{
		return self;
	}
	inline const mcr_ISignal &operator *() const
	{
		return self;
	}
//	static inline ISignal *self(mcr_ISignal *originPt)
//	{
//		return (ISignal *)originPt;
//	}

	inline Libmacro &context() const
	{
		return *_context;
	}

	template<typename ISignalMemberT>
	ISignal &build()
	{
		interface().data_size = sizeof(ISignalMemberT);
		interface().allocate = &ISignal::allocate<ISignalMemberT>;
		interface().deallocate = &ISignal::deallocate<ISignalMemberT>;
		interface().compare = &ISignal::compare<ISignalMemberT>;
		interface().copy = &ISignal::copy<ISignalMemberT>;
		self.send = &ISignal::sendFn<ISignalMemberT>;
		return *this;
	}

	/*! \ref mcr_Interface.allocate */
	template<typename ISignalMemberT>
	static void *allocate()
	{
		return new ISignalMemberT();
	}

	/*! \ref mcr_Interface.deallocate */
	template<typename ISignalMemberT>
	static void deallocate(void *dataPt)
	{
		if (dataPt)
			delete reinterpret_cast<ISignalMemberT *>(dataPt);
	}

	/*! \ref mcr_Interface.compare */
	template<typename ISignalMemberT>
	static int compare(const void *lhsPt, const void *rhsPt)
	{
		if (rhsPt) {
			if (lhsPt) {
				return reinterpret_cast<const ISignalMemberT *>(lhsPt)->compare(*reinterpret_cast<const ISignalMemberT *>
						(rhsPt));
			}
			return -1;
		}
		return !!lhsPt;
	}
	/*! \ref mcr_Interface.copy */
	template<typename ISignalMemberT>
	static int copy(void *dstPt, const void *srcPt)
	{
		if (!dstPt)
			return EFAULT;
		try {
			reinterpret_cast<ISignalMemberT *>(dstPt)->copy(reinterpret_cast<const ISignalMemberT *>(srcPt));
		} catch (int err) {
			return err;
		}
		return 0;
	}

	/*! \ref mcr_ISignal.send */
	template<typename ISignalMemberT>
	static int sendFn(mcr_Signal *signalPt)
	{
		mcr_dassert(signalPt);
		mcr_dassert(signalPt->isignal);
		ISignalMemberT *dataPt = reinterpret_cast<ISignalMemberT *>(signalPt->instance.data_member.data);
		if (dataPt) {
			try {
				dataPt->send(signalPt);
			} catch (int err) {
				return err;
			}
		}
		return 0;
	}
private:
	Libmacro *_context;
};

/*!
 */
struct MCR_API IDispatcher
{
	mcr_IDispatcher self;

	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 *  \param add \ref mcr_IDispatcher.add
	 *  \param clear \ref opt mcr_IDispatcher.clear
	 *  \param dispatch \ref opt mcr_IDispatcher.dispatch
	 *  \param modifier \ref opt mcr_IDispatcher.modifier
	 *  \param remove \ref opt mcr_IDispatcher.remove
	 *  \param trim \ref opt mcr_IDispatcher.trim
	 *  exist.
	 */
	IDispatcher(Libmacro *context = nullptr,
				  void (*deinit)(struct mcr_IDispatcher *) = nullptr,
				  mcr_IDispatcher_add_fnc add = nullptr,
				  void (*clear)(struct mcr_IDispatcher *) = nullptr,
				  mcr_IDispatcher_dispatch_fnc dispatch = nullptr,
				  mcr_IDispatcher_modify_fnc modifier = nullptr,
				  mcr_IDispatcher_remove_fnc remove = nullptr,
				  void (*trim)(struct mcr_IDispatcher *) = nullptr);
	IDispatcher(const IDispatcher &) = default;
	virtual ~IDispatcher() {}
	IDispatcher &operator =(const IDispatcher &) = default;

	inline mcr_IDispatcher &operator *()
	{
		return self;
	}
	inline const mcr_IDispatcher &operator *() const
	{
		return self;
	}
//	static inline IDispatcher *self(mcr_IDispatcher *originPt)
//	{
//		return (IDispatcher *)originPt;
//	}

	inline Libmacro &context() const
	{
		return *_context;
	}
private:
	Libmacro *_context;
};

/*!
 */
struct MCR_API ITrigger
{
	mcr_ITrigger self;
	inline mcr_Interface &interface()
	{
		return self.interface;
	}

	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 *  \param receive \ref opt mcr_ITrigger.receive
	 *  \param interface \ref opt mcr_ITrigger.interface
	 */
	ITrigger(Libmacro *context = nullptr,
				mcr_Trigger_receive_fnc receive = nullptr);
	virtual ~ITrigger() {
		mcr_ITrigger_deinit(&self);
	}
	ITrigger(const ITrigger &) = default;
	ITrigger &operator =(const ITrigger &) = default;

	inline mcr_ITrigger &operator *()
	{
		return self;
	}
	inline const mcr_ITrigger &operator *() const
	{
		return self;
	}
//	static inline ITrigger *self(mcr_ITrigger *originPt)
//	{
//		return (ITrigger *)originPt;
//	}

	inline Libmacro &context() const
	{
		return *_context;
	}
private:
	Libmacro *_context;
};

/*! Can subclass, but has no virtual functions.
 */
struct MCR_API Signal
{
	mcr_Signal self;
	inline mcr_ISignal *isignal()
	{
		return self.isignal;
	}
	inline mcr_Instance &instance()
	{
		return self.instance;
	}
	inline mcr_Interface *interface()
	{
		return self.interface;
	}

	/*! \param isignal \ref mcr_Signal.isignal
	 *  \param isDispatch \ref mcr_Signal.dispatch_flag
	 */
	Signal(mcr_ISignal *isignal = nullptr, bool dispatchFlag = false)
	{
		mcr_Signal_init(&self);
		self.isignal = isignal;
		self.dispatch_flag = dispatchFlag;
	}
	Signal(const Signal &copytron)
	{
		mcr_Signal_init(&self);
		copy(&copytron.self);
	}
	Signal(const mcr_Signal &copytron)
	{
		mcr_Signal_init(&self);
		copy(&copytron);
	}
	virtual ~Signal()
	{
		mcr_Signal_deinit(&self);
	}
	inline Signal &operator =(const Signal &copytron)
	{
		copy(&copytron.self);
		return *this;
	}
	inline Signal &operator =(const mcr_Signal &copytron)
	{
		copy(&copytron);
		return *this;
	}
	inline Signal &operator =(mcr_ISignal *isignal)
	{
		setISignal(isignal);
		return *this;
	}

	inline mcr_Signal &operator *()
	{
		return self;
	}
	inline const mcr_Signal &operator *() const
	{
		return self;
	}
//	static inline Signal *self(mcr_Signal *originPt)
//	{
//		return (Signal *)originPt;
//	}

	/*! \ref mcr_Signal_copy */
	void copy(const mcr_Signal *copytron)
	{
		if (copytron != &self) {
			if (mcr_Signal_copy(&self, copytron))
				throw mcr_read_err();
			mcr_dassert(instance().data_member.data != copytron->instance.data_member.data);
			mcr_dassert(instance().data_member.deallocate);
		}
	}
	/*! \ref mcr_Signal_deinit + set \ref mcr_Signal.isignal */
	void setISignal(mcr_ISignal *isignal);

	void reset()
	{
		if (mcr_Instance_reset(&instance()))
			throw mcr_read_err();
	}

	template <typename T>
	inline const T *data() const
	{
		return reinterpret_cast<const T *>(self.instance.data_member.data);
	}
	template <typename T>
	inline T *data()
	{
		return reinterpret_cast<T *>(instance().data_member.data);
	}
};

/*! Can subclass, but has no virtual functions.
 */
struct MCR_API Trigger
{
	mcr_Trigger self;
	inline mcr_ITrigger *itrigger()
	{
		return self.itrigger;
	}
	inline mcr_Instance &instance()
	{
		return self.instance;
	}
	inline mcr_Interface *interface()
	{
		return self.interface;
	}

	/*! \param itrigger \ref mcr_Trigger.itrigger
	 *  \param trigger \ref mcr_Trigger.trigger
	 *  \param actor \ref mcr_Trigger.actor
	 */
	Trigger(mcr_ITrigger *itrigger = nullptr,
			mcr_Trigger_receive_fnc trigger = nullptr, void *actor = nullptr)
	{
		mcr_Trigger_init(&self);
		self.itrigger = itrigger;
		self.trigger = trigger;
		self.actor = actor;
	}
	/*! \ref mcr_Trigger_copy */
	Trigger(const Trigger &copytron)
	{
		mcr_Trigger_init(&self);
		copy(&*copytron);
	}
	/*! \ref mcr_Trigger_copy */
	Trigger(const mcr_Trigger &copytron)
	{
		mcr_Trigger_init(&self);
		copy(&copytron);
	}
	virtual ~Trigger()
	{
		mcr_Trigger_deinit(&self);
	}
	inline Trigger &operator =(const Trigger &copytron)
	{
		copy(&*copytron);
		return *this;
	}
	inline Trigger &operator =(const mcr_Trigger &copytron)
	{
		copy(&copytron);
		return *this;
	}
	/*! \ref mcr_Trigger_deinit + set \ref mcr_Trigger.itrigger */
	inline Trigger &operator =(mcr_ITrigger *itrigger)
	{
		setITrigger(itrigger);
		return *this;
	}

	inline mcr_Trigger &operator *()
	{
		return self;
	}
	inline const mcr_Trigger &operator *() const
	{
		return self;
	}
//	static inline Trigger *self(mcr_Trigger *originPt)
//	{
//		return (Trigger *)originPt;
//	}

	/*! \ref mcr_Trigger_copy */
	void copy(const mcr_Trigger *copytron)
	{
		if (copytron != &self) {
			if (mcr_Trigger_copy(&self, copytron))
				throw mcr_read_err();
		}
	}
	/*! \ref mcr_Trigger_deinit + set \ref mcr_Trigger.itrigger */
	void setITrigger(mcr_ITrigger *itrigger);
};
}

#endif
