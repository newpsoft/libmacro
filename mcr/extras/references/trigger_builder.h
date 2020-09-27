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

#ifndef MCR_EXTRAS_REFERENCES_TRIGGER_BUILDER_H_
#define MCR_EXTRAS_REFERENCES_TRIGGER_BUILDER_H_

#include "mcr/extras/references/itrigger_builder.h"

namespace mcr
{
/*! \ref mcr_Trigger reference editor
 *
 *  Virtual and concrete class
 */
class MCR_API TriggerBuilder
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 */
	TriggerBuilder(Libmacro *context = nullptr);
	TriggerBuilder(const TriggerBuilder &) = default;
	virtual ~TriggerBuilder() {}
	inline TriggerBuilder &operator =(const TriggerBuilder &copytron) = delete;

	/*! Change Trigger to reference */
	inline TriggerBuilder &build(mcr_Trigger *trigPt)
	{
		setTrigger(trigPt);
		return *this;
	}

	inline TriggerBuilder &build(mcr_ITrigger *itrigPt)
	{
		setITrigger(itrigPt);
		return *this;
	}
	inline TriggerBuilder &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline TriggerBuilder &build(const char *name)
	{
		setName(name);
		return *this;
	}

	/*! Get Trigger reference, non-virtual */
	inline mcr_Trigger *ptr() const
	{
		return _trigPt;
	}
	inline Libmacro &context() const
	{
		return *_context;
	}

	/* ITrigger manipulators */
	virtual inline mcr_ITrigger *itrigger() const
	{
		return _trigPt ? _trigPt->itrigger : nullptr;
	}
	virtual inline mcr_Interface *interface() const
	{
		return _trigPt ? _trigPt->interface : nullptr;
	}
	virtual inline ITriggerBuilder itriggerBuilder() const
	{
		return ITriggerBuilder(_context).build(itrigger());
	}
	virtual void setITrigger(mcr_ITrigger *itrigPt);

	virtual inline size_t id() const
	{
		return itriggerBuilder().id();
	}
	virtual void setId(size_t val)
	{
		setITrigger(itriggerBuilder().build(val).itrigger());
	}
	virtual inline const char *name() const
	{
		return itriggerBuilder().name();
	}
	virtual void setName(const char *val)
	{
		setITrigger(itriggerBuilder().build(val).itrigger());
	}

	/* Trigger properties */
	virtual inline mcr_Trigger *trigger() const
	{
		return _trigPt;
	}
	virtual inline mcr_Instance *instance() const
	{
		return _trigPt ? &_trigPt->instance : nullptr;
	}
	virtual inline mcr_DataMember *dataMember() const
	{
		return _trigPt ? &_trigPt->instance.data_member : nullptr;
	}
	virtual inline void setTrigger(mcr_Trigger *trigPt)
	{
		_trigPt = trigPt;
	}
	inline void setTrigger(const TriggerBuilder &copytron)
	{
		setTrigger(copytron.trigger());
	}

	inline bool empty() const
	{
		return !data<void>();
	}
	virtual inline TriggerBuilder &clear()
	{
		return deinit();
	}
	virtual inline TriggerBuilder &deinit()
	{
		if (_trigPt)
			mcr_Trigger_deinit(_trigPt);
		return *this;
	}

	virtual inline mcr_Trigger_receive_fnc triggerDispatch() const
	{
		return _trigPt ? _trigPt->trigger : nullptr;
	}
	virtual inline void setTriggerDispatch(mcr_Trigger_receive_fnc val)
	{
		if (_trigPt)
			_trigPt->trigger = val;
	}
	virtual inline void *actor() const
	{
		return _trigPt ? _trigPt->actor : nullptr;
	}
	virtual inline void setActor(void *val)
	{
		if (_trigPt)
			_trigPt->actor = val;
	}

	/* Trigger functions */
	template <typename T>
	inline const T *data() const
	{
		return data<T>(_trigPt);
	}
	template <typename T>
	inline T *data()
	{
		return data<T>(_trigPt);
	}
	template <typename T>
	static inline const T *data(const mcr_Trigger *trigPt)
	{
		return trigPt ? reinterpret_cast<T *>(trigPt->instance.data_member.data) : nullptr;
	}
	template <typename T>
	static inline T *data(mcr_Trigger *trigPt)
	{
		return trigPt ? reinterpret_cast<T *>(trigPt->instance.data_member.data) : nullptr;
	}
	virtual inline TriggerBuilder &mkdata()
	{
		if (trigger() && itrigger() && empty()) {
			if (mcr_Instance_reset(&_trigPt->instance))
				throw mcr_read_err();
			if (empty())
				throw mcr_read_err();
		}
		return *this;
	}

	virtual void copy(const mcr_Trigger *copytron);
	inline void copy(const TriggerBuilder &copytron)
	{
		copy(copytron.trigger());
	}
	virtual inline int compare(const mcr_Trigger *rhs) const
	{
		return mcr_Trigger_compare(_trigPt, rhs);
	}
	virtual inline int compare(const TriggerBuilder &rhs) const
	{
		return mcr_Trigger_compare(_trigPt, rhs._trigPt);
	}
private:
	Libmacro *_context;
	mcr_Trigger *_trigPt;
};
}

#endif
