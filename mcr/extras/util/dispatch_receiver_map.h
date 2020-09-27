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

#ifndef MCR_EXTRAS_DISPATCH_RECEIVER_MAP_H_
#define MCR_EXTRAS_DISPATCH_RECEIVER_MAP_H_

#include "mcr/extras/util/dispatch_receiver_set.h"

#include <algorithm>
#include <functional>
#include <map>
#include <vector>

namespace mcr
{
/*! Exportable interface to std::map<T, DispatchReceiverSet>.
 *  Syncs values with std::vector<mcr_ReceiverMapElement> to use with C struct
 *  bsearch.
 */
template<typename kT>
class DispatchReceiverMap final
{
public:
	struct ReceiverArrayLess :
		std::binary_function<mcr_ReceiverMapElement, mcr_ReceiverMapElement, bool> {
		bool operator()(const mcr_ReceiverMapElement &s1, const mcr_ReceiverMapElement &s2) const
		{
			return reinterpret_cast<const kT &>(s1) < reinterpret_cast<const kT &>(s2);
		}
	};

	DispatchReceiverMap(mcr_ReceiverMapElement **applyReceiversPt = nullptr,
						size_t *applyCountPt = nullptr)
		: _applyReceiversPt(applyReceiversPt), _applyCountPt(applyCountPt)
	{
	}
	DispatchReceiverMap(const DispatchReceiverMap &copytron)
		: _applyReceiversPt(nullptr), _applyCountPt(0),
		  _receiverMap(copytron._receiverMap),
		  _receivers(copytron._receivers)
	{
		updateVectors();
		apply();
	}
	~DispatchReceiverMap()
	{
		apply(nullptr, 0);
	}
	DispatchReceiverMap &operator =(const DispatchReceiverMap &copytron)
	{
		if (&copytron != this) {
			apply(nullptr, 0);
			_receiverMap = copytron._receiverMap;
			_receivers = copytron._receivers;
			updateVectors();
			apply();
		}
		return *this;
	}

	void add(const kT &key, void *receiver, mcr_dispatch_receive_fnc receiverFnc)
	{
		auto mapIt = _receiverMap.find(key);
		// vector is in map (or insert)
		if (mapIt == _receiverMap.end()) {
			// return pair<iterator, bool>
			auto success = _receiverMap.insert({key, {}});
			mcr_throwif(!success.second, errno);
			mapIt = success.first;
		}
		// insert receiver into map's sorted vector
		DispatchReceiverSet &receiverSet = mapIt->second;
		receiverSet.add(receiver, receiverFnc);
		updateVector(key, receiverSet.array(), receiverSet.count());
		apply();
	}
	void clear()
	{
		apply(nullptr, 0);
		_receiverMap.clear();
		_receivers.clear();
	}
	void remove(void *remReceiver)
	{
		apply(nullptr, 0);
		// In map, for each sorted vector search and remove receiver
		for (auto rit = _receiverMap.rbegin(); rit != _receiverMap.rend(); ++rit) {
			DispatchReceiverSet &receiverSet = rit->second;
			if (receiverSet.empty()) {
				/* This set is empty, remove from map and vector. */
				removeKey(rit->first);
				_receiverMap.erase(rit.base());
			} else if (receiverSet.find(remReceiver)) {
				/* Has receiver to remove... */
				receiverSet.remove(remReceiver);
				/* Now empty remove, else update vector array */
				if (receiverSet.empty()) {
					removeKey(rit->first);
					_receiverMap.erase(rit.base());
				} else {
					/* Note: Receiver is likely only registered once, so
					 * updating one vector is heuristically more efficient than
					 * everything every time. */
					updateVector(rit->first, receiverSet.array(), receiverSet.count());
				}
			}
			/* else not found in current set, move to next. */
		}
		apply();
	}
	void trim()
	{
		for (auto it = _receiverMap.begin(); it != _receiverMap.end(); it++) {
			it->second.trim();
		}
		/* Vector references may have changed. */
		updateVectors();
		_receivers.shrink_to_fit();
		apply();
	}

	mcr_ReceiverMapElement *array()
	{
		return _receivers.empty() ? nullptr : &_receivers.front();
	}
	size_t count() const
	{
		return _receiverMap.size();
	}
	inline size_t size() const
	{
		return count();
	}
	/*! Set current receiver set to current applicable receivers and
	 *  count references. */
	void apply()
	{
		apply(array(), count());
	}
	/*! Set given receiver set to current applicable receivers and count
	 *  references. */
	void apply(mcr_ReceiverMapElement *receivers, size_t count)
	{
		if (applicable()) {
			*_applyReceiversPt = receivers;
			*_applyCountPt = count;
		}
	}

	mcr_ReceiverMapElement **applyReceiversPt() const
	{
		return _applyReceiversPt;
	}
	void setApplyReceiversPt(mcr_ReceiverMapElement **applyReceiversPt)
	{
		if (applyReceiversPt != _applyReceiversPt) {
			apply(nullptr, 0);
			_applyReceiversPt = applyReceiversPt;
			apply();
		}
	}
	size_t *applyCountPt() const
	{
		return _applyCountPt;
	}
	void setApplyCountPt(size_t *applyCountPt)
	{
		if (applyCountPt != _applyCountPt) {
			apply(nullptr, 0);
			_applyCountPt = applyCountPt;
			apply();
		}
	}
	inline bool applicable() const
	{
		return _applyReceiversPt && _applyCountPt;
	}
	void setApplyReceivers(mcr_ReceiverMapElement **applyReceiversPt,
						   size_t *applyCountPt)
	{
		if (applyReceiversPt != _applyReceiversPt ||
			applyCountPt != _applyCountPt ) {
			apply(nullptr, 0);
			_applyReceiversPt = applyReceiversPt;
			_applyCountPt = applyCountPt;
			apply();
		}
	}
private:
	mcr_ReceiverMapElement **_applyReceiversPt;
	size_t *_applyCountPt;

	std::map<kT, DispatchReceiverSet> _receiverMap;
	std::vector<mcr_ReceiverMapElement> _receivers;

	void removeKey(const kT &key)
	{
		mcr_ReceiverMapElement insert;
		reinterpret_cast<kT &>(insert) = key;
		auto found = std::lower_bound(_receivers.begin(), _receivers.end(), insert,
									  ReceiverArrayLess());
		if (found != _receivers.end())
			_receivers.erase(found);
	}
	void updateVector(const kT &key, mcr_DispatchReceiver *array, size_t count)
	{
		mcr_ReceiverMapElement insert;
		reinterpret_cast<kT &>(insert) = key;
		auto found = std::lower_bound(_receivers.begin(), _receivers.end(), insert,
									  ReceiverArrayLess());
		/* Not found, create ref. */
		if (found == _receivers.end() || compare(&*found, &key)) {
			insert.receivers = array;
			insert.receiver_count = count;
			_receivers.insert(found, insert);
		} else {
			found->receivers = array;
			found->receiver_count = count;
		}
	}
	void updateVectors()
	{
		_receivers.resize(_receiverMap.size());
		auto receiverArray = _receivers.begin();
		for (auto it = _receiverMap.begin(); it != _receiverMap.end();
			 it++, receiverArray++) {
			reinterpret_cast<kT &>(*receiverArray) = it->first;
			receiverArray->receivers = it->second.array();
			receiverArray->receiver_count = it->second.count();
		}
	}

	static int compare(const void *lhsPt, const void *rhsPt)
	{
		if (rhsPt) {
			if (lhsPt)
				return MCR_CMP_PTR(const kT, lhsPt, rhsPt);
			return -1;
		}
		return !!lhsPt;
	}
};
}

#endif
