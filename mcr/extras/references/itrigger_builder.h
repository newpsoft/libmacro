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

#ifndef MCR_EXTRAS_REFERENCES_ITRIGGER_BUILDER_H_
#define MCR_EXTRAS_REFERENCES_ITRIGGER_BUILDER_H_

#include "mcr/extras/wrappers.h"

namespace mcr
{
/*! \ref mcr_ITrigger reference editor
 *
 *  Virtual and concrete class
 */
class MCR_API ITriggerBuilder
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 */
	ITriggerBuilder(Libmacro *context = nullptr);
	ITriggerBuilder(const ITriggerBuilder &) = default;
	virtual ~ITriggerBuilder() {}
	inline ITriggerBuilder &operator =(const ITriggerBuilder &copytron) = delete;

	inline ITriggerBuilder &build(mcr_ITrigger *itrigPt)
	{
		setITrigger(itrigPt);
		return *this;
	}
	inline ITriggerBuilder &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline ITriggerBuilder &build(const char *name)
	{
		setName(name);
		return *this;
	}

	inline Libmacro &context() const
	{
		return *_context;
	}

	/*! \ref mcr_register this ITrigger
	 *
	 *  If the ITrigger id is not -1, then mcr_reg_set_name and
	 *  mcr_reg_add_name will be used instead.
	 */
	virtual void registerType(const char *name,
							  const char * const*addNames = nullptr, size_t addNamesCount = 0);
	virtual void unregisterType();

	/*! Get ITrigger reference */
	virtual inline mcr_ITrigger *itrigger() const
	{
		return _itrigPt;
	}
	inline mcr_Interface *interface() const
	{
		return _itrigPt ? &_itrigPt->interface : NULL;
	}
	virtual inline void setITrigger(mcr_ITrigger *itrigPt)
	{
		_itrigPt = itrigPt;
	}

	/* Interface */
	/*! \ref mcr_Interface.id */
	virtual inline size_t id()
	{
		return mcr_ITrigger_id(_itrigPt);
	}
	virtual void setId(size_t val);
	//! \todo set/change id/name
	virtual const char *name();
	virtual void setName(const char *name);
	/*! \ref mcr_Interface.data_size */
	virtual inline size_t dataSize() const
	{
		return _itrigPt ? _itrigPt->interface.data_size : 0;
	}
	virtual inline void setDataSize(size_t newSize)
	{
		if (_itrigPt)
			_itrigPt->interface.data_size = newSize;
	}

	virtual inline mcr_data_fnc init() const
	{
		return _itrigPt ? _itrigPt->interface.init : nullptr;
	}
	virtual inline void setInit(mcr_data_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.init = val;
	}

	virtual inline mcr_data_fnc deinit() const
	{
		return _itrigPt ? _itrigPt->interface.deinit : nullptr;
	}
	virtual inline void setDeinit(mcr_data_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.deinit = val;
	}

	virtual inline mcr_compare_fnc compare() const
	{
		return _itrigPt ? _itrigPt->interface.compare : nullptr;
	}
	virtual inline void setCompare(mcr_compare_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.compare = val;
	}

	virtual inline mcr_copy_fnc copy() const
	{
		return _itrigPt ? _itrigPt->interface.copy : nullptr;
	}
	virtual inline void setCopy(mcr_copy_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.copy = val;
	}

	/* ITrigger */
	virtual inline mcr_Trigger_receive_fnc receive() const
	{
		return _itrigPt ? _itrigPt->receive : nullptr;
	}
	virtual inline void setReceive(mcr_Trigger_receive_fnc fnc)
	{
		if (_itrigPt)
			_itrigPt->receive = fnc;
	}

	virtual mcr_IRegistry *registry();
private:
	Libmacro *_context;
	mcr_ITrigger *_itrigPt;
};
}

#endif
