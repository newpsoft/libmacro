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

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_DISPATCH_RECEIVER_SET_H_
#define MCR_EXTRAS_DISPATCH_RECEIVER_SET_H_

#include "mcr/extras/base_cpp.h"

namespace mcr
{
/*! Non-exportable members */
class DispatchReceiverSetPrivate;
/*! Exportable interface to std::vector<mcr_DispatchReceiver>. Can apply own
 *  vector to mcr_DispatchReceiver * and size_t variables. These are usable
 *  in C structs.
 */
class MCR_API DispatchReceiverSet
{
	friend class DispatchReceiverSetPrivate;
public:
	DispatchReceiverSet(mcr_DispatchReceiver **applyReceiversPt = nullptr,
						size_t *applyCountPt = nullptr);
	DispatchReceiverSet(const DispatchReceiverSet &copytron);
	virtual ~DispatchReceiverSet();
	DispatchReceiverSet &operator =(const DispatchReceiverSet &);

	virtual void add(void *receiver, mcr_dispatch_receive_fnc receiverFnc);
	virtual void clear();
	virtual void remove(void *remReceiver);
	virtual void trim();

	virtual mcr_DispatchReceiver *array() const;
	virtual mcr_DispatchReceiver *find(void *receiver) const;
	virtual size_t count() const;
	inline size_t size() const
	{
		return count();
	}
	inline bool empty() const
	{
		return !count();
	}
	/*! Set current receiver set to current applicable receivers and
	 *  count references. */
	virtual void apply();
	/*! Set given receiver set to current applicable receivers and count
	 *  references. */
	virtual void apply(mcr_DispatchReceiver *receivers, size_t count)
	{
		if (applicable()) {
			*_applyReceiversPt = receivers;
			*_applyCountPt = count;
		}
	}

	virtual mcr_DispatchReceiver **applyReceiversPt() const
	{
		return _applyReceiversPt;
	}
	virtual void setApplyReceiversPt(mcr_DispatchReceiver **applyReceiversPt);
	virtual size_t *applyCountPt() const
	{
		return _applyCountPt;
	}
	virtual void setApplyCountPt(size_t *applyCountPt);
	inline bool applicable() const
	{
		return _applyReceiversPt && _applyCountPt;
	}
	virtual void setApplyReceivers(mcr_DispatchReceiver **applyReceiversPt,
								   size_t *applyCountPt);
private:
	mcr_DispatchReceiver **_applyReceiversPt;
	size_t *_applyCountPt;

	DispatchReceiverSetPrivate *_private;
};
}

#endif
