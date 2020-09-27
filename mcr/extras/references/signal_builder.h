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
 *  \brief
 */

#ifndef MCR_EXTRAS_REFERENCES_SIGNAL_BUILDER_H_
#define MCR_EXTRAS_REFERENCES_SIGNAL_BUILDER_H_

#include "mcr/extras/references/isignal_builder.h"

namespace mcr
{
/*! \ref mcr_Signal reference editor
 *
 *  Virtual and concrete class
 */
class MCR_API SignalBuilder
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 */
	SignalBuilder(Libmacro *context = nullptr);
	SignalBuilder(const SignalBuilder &) = default;
	virtual ~SignalBuilder() {}
	inline SignalBuilder &operator =(const SignalBuilder &) = delete;

	/*! Change Signal to reference */
	inline SignalBuilder &build(mcr_Signal *sigPt)
	{
		setSignal(sigPt);
		return *this;
	}

	inline SignalBuilder &build(mcr_ISignal *isigPt)
	{
		setISignal(isigPt);
		return *this;
	}
	inline SignalBuilder &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline SignalBuilder &build(const char *name)
	{
		setName(name);
		return *this;
	}

	/*! \ref send, non-virtual */
	inline SignalBuilder &operator ()()
	{
		send();
		return *this;
	}

	inline mcr_Signal *ptr() const
	{
		return _sigPt;
	}
	inline Libmacro &context() const
	{
		return *_context;
	}

	/* ISignal manipulators, destroy current members when changing isignal */
	virtual inline mcr_ISignal *isignal() const
	{
		return _sigPt ? _sigPt->isignal : nullptr;
	}
	virtual inline mcr_Interface *interface() const
	{
		return _sigPt ? _sigPt->interface : nullptr;
	}
	virtual inline ISignalBuilder isignalBuilder() const
	{
		return ISignalBuilder(&context()).build(isignal());
	}
	virtual void setISignal(mcr_ISignal *isigPt);

	virtual inline size_t id() const
	{
		return isignalBuilder().id();
	}
	virtual inline void setId(size_t val)
	{
		setISignal(isignalBuilder().build(val).isignal());
	}
	virtual inline const char *name() const
	{
		return isignalBuilder().name();
	}
	virtual inline void setName(const char *val)
	{
		setISignal(isignalBuilder().build(val).isignal());
	}

	/* Signal properties */
	virtual inline mcr_Signal *signal() const
	{
		return _sigPt;
	}
	virtual inline mcr_Instance *instance() const
	{
		return _sigPt ? &_sigPt->instance : nullptr;
	}
	virtual inline mcr_DataMember *dataMember() const
	{
		return _sigPt ? &_sigPt->instance.data_member : nullptr;
	}
	virtual inline void setSignal(mcr_Signal *sigPt)
	{
		_sigPt = sigPt;
	}
	inline void setSignal(const SignalBuilder &copytron)
	{
		setSignal(copytron.signal());
	}

	virtual inline bool empty() const
	{
		return !data<void>();
	}
	virtual inline SignalBuilder &clear()
	{
		return deinit();
	}
	virtual inline SignalBuilder &deinit()
	{
		if (_sigPt)
			mcr_Signal_deinit(_sigPt);
		return *this;
	}

	virtual inline bool dispatch() const
	{
		return _sigPt ? _sigPt->dispatch_flag : false;
	}
	virtual inline void setDispatch(bool val)
	{
		if (_sigPt)
			_sigPt->dispatch_flag = val;
	}

	/* Signal functions */
	template <typename T>
	inline const T *data() const
	{
		return data<T>(_sigPt);
	}
	template <typename T>
	inline T *data()
	{
		return data<T>(_sigPt);
	}
	template <typename T>
	static inline const T *data(const mcr_Signal *signalPt)
	{
		return signalPt ? reinterpret_cast<T *>(signalPt->instance.data_member.data) : nullptr;
	}
	template <typename T>
	static inline T *data(mcr_Signal *signalPt)
	{
		return signalPt ? reinterpret_cast<T *>(signalPt->instance.data_member.data) : nullptr;
	}
	virtual inline SignalBuilder &mkdata()
	{
		/* Only create new data if it does not already exist */
		if (signal() && isignal() && empty()) {
			if (mcr_Instance_reset(&_sigPt->instance))
				throw mcr_read_err();
			if (empty())
				throw mcr_read_err();
		}
		return *this;
	}

	virtual bool dispatch();
	virtual void send();
	virtual void copy(const mcr_Signal *copytron);
	inline void copy(const SignalBuilder &copytron)
	{
		copy(copytron.signal());
	}
	virtual inline int compare(const mcr_Signal *rhs) const
	{
		return mcr_Signal_compare(_sigPt, rhs);
	}
	virtual inline int compare(const SignalBuilder &rhs) const
	{
		return mcr_Signal_compare(_sigPt, rhs._sigPt);
	}
private:
	Libmacro *_context;
	mcr_Signal *_sigPt;
};
}

#endif
