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

#ifndef MCR_EXTRAS_REFERENCES_ISIGNAL_BUILDER_H_
#define MCR_EXTRAS_REFERENCES_ISIGNAL_BUILDER_H_

#include "mcr/extras/wrappers.h"

namespace mcr
{
/*! \ref mcr_ISignal reference editor
 *
 *  Virtual and concrete class
 */
class MCR_API ISignalBuilder
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EFAULT if no context exists
	 */
	ISignalBuilder(Libmacro *context = nullptr);
	ISignalBuilder(const ISignalBuilder &) = default;
	virtual ~ISignalBuilder() {}
	ISignalBuilder &operator =(const ISignalBuilder &copytron) = delete;

	inline ISignalBuilder &build(mcr_ISignal *isigPt)
	{
		setISignal(isigPt);
		return *this;
	}
	inline ISignalBuilder &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline ISignalBuilder &build(const char *name)
	{
		setName(name);
		return *this;
	}

	inline Libmacro &context() const
	{
		return *_context;
	}

	/*! \ref mcr_register this ISignal
	 *
	 *  If the ISignal id is not -1, then mcr_reg_set_name and
	 *  mcr_reg_add_name will be used instead.
	 */
	virtual void registerType(const char *name,
							  const char * const*addNames = nullptr, size_t addNamesCount = 0);
	virtual void unregisterType();

	/*! Get ISignal reference */
	virtual inline mcr_ISignal *isignal() const
	{
		return _isigPt;
	}
	inline mcr_Interface *interface() const
	{
		return _isigPt ? &_isigPt->interface : NULL;
	}
	virtual inline void setISignal(mcr_ISignal *isigPt)
	{
		_isigPt = isigPt;
	}

	/* Interface */
	/*! \ref mcr_Interface.id */
	virtual inline size_t id()
	{
		return mcr_ISignal_id(_isigPt);
	}
	virtual void setId(size_t val);
	//! \todo set/change id/name
	virtual const char *name();
	virtual void setName(const char *name);
	/*! \ref mcr_Interface.data_size */
	virtual inline size_t dataSize() const
	{
		return _isigPt ? _isigPt->interface.data_size : 0;
	}
	virtual inline void setDataSize(size_t newSize)
	{
		if (_isigPt)
			_isigPt->interface.data_size = newSize;
	}

	virtual inline mcr_data_fnc init() const
	{
		return _isigPt ? _isigPt->interface.init : nullptr;
	}
	virtual inline void setInit(mcr_data_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.init = val;
	}

	virtual inline mcr_data_fnc deinit() const
	{
		return _isigPt ? _isigPt->interface.deinit : nullptr;
	}
	virtual inline void setDeinit(mcr_data_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.deinit = val;
	}

	virtual inline mcr_compare_fnc compare() const
	{
		return _isigPt ? _isigPt->interface.compare : nullptr;
	}
	virtual inline void setCompare(mcr_compare_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.compare = val;
	}

	virtual inline mcr_copy_fnc copy() const
	{
		return _isigPt ? _isigPt->interface.copy : nullptr;
	}
	virtual inline void setCopy(mcr_copy_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.copy = val;
	}

	/* ISignal */
	virtual inline mcr_IDispatcher *dispatcher() const
	{
		return _isigPt ? _isigPt->dispatcher_pt : nullptr;
	}
	virtual inline void setDispatcher(mcr_IDispatcher *val)
	{
		if (_isigPt)
			_isigPt->dispatcher_pt = val;
	}

	virtual inline mcr_signal_fnc send() const
	{
		return _isigPt ? _isigPt->send : nullptr;
	}
	virtual inline void setSend(mcr_signal_fnc val)
	{
		if (_isigPt)
			_isigPt->send = val;
	}

	virtual mcr_IRegistry *registry();
private:
	Libmacro *_context;
	mcr_ISignal *_isigPt;
};
}

#endif
