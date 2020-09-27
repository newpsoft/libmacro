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

#include "mcr/libmacro.h"

#include <algorithm>
#include <cstdio>
#include <map>
#include <vector>

#include "mcr/extras/util/dispatch_receiver_set.h"
#include "mcr/extras/util/dispatch_receiver_less.h"

#define priv _private

namespace mcr
{
class DispatchReceiverSetPrivate
{
	friend class DispatchReceiverSet;
public:
	std::vector<mcr_DispatchReceiver> receivers;
};

DispatchReceiverSet::DispatchReceiverSet(mcr_DispatchReceiver
		**applyReceiversPt, size_t *applyCountPt)
	: _applyReceiversPt(applyReceiversPt), _applyCountPt(applyCountPt),
	  priv(new DispatchReceiverSetPrivate)
{
}

DispatchReceiverSet::DispatchReceiverSet(const DispatchReceiverSet &copytron)
	: _applyReceiversPt(nullptr), _applyCountPt(0),
	  priv(new DispatchReceiverSetPrivate)
{
	*priv = *copytron.priv;
	apply();
}

DispatchReceiverSet::~DispatchReceiverSet()
{
	apply(nullptr, 0);
	delete priv;
}

DispatchReceiverSet &DispatchReceiverSet::operator=(const DispatchReceiverSet
		&copytron)
{
	if (&copytron != this) {
		*priv = *copytron.priv;
		apply();
	}
	return *this;
}

void DispatchReceiverSet::add(void *receiver,
							  mcr_dispatch_receive_fnc receiverFnc)
{
	mcr_assert_receive_function(receiver, receiverFnc);
	mcr_DispatchReceiver insert;
	insert.receiver = receiver;
	insert.receive = receiverFnc;
	auto &receivers = priv->receivers;
	auto found = std::lower_bound(receivers.begin(), receivers.end(), insert, dispatch_receiver_less());
	receivers.insert(found, insert);
	apply();
}

void DispatchReceiverSet::clear()
{
	apply(nullptr, 0);
	priv->receivers.clear();
}

void DispatchReceiverSet::remove(void *remReceiver)
{
	mcr_DispatchReceiver receiver = { remReceiver, nullptr };
	auto &receivers = priv->receivers;
	auto range = std::equal_range(receivers.begin(), receivers.end(), receiver, dispatch_receiver_less());
	if (range.second != receivers.end()) {
		receivers.erase(range.first, range.second);
		apply();
	}
}

void DispatchReceiverSet::trim()
{
	priv->receivers.shrink_to_fit();
	apply();
}

mcr_DispatchReceiver *DispatchReceiverSet::array() const
{
	return priv->receivers.empty() ? nullptr : &priv->receivers.front();
}

mcr_DispatchReceiver *DispatchReceiverSet::find(void *receiver) const
{
	if (priv->receivers.empty())
		return nullptr;
	return reinterpret_cast<mcr_DispatchReceiver *>(bsearch(&receiver, &priv->receivers.front(), priv->receivers.size(),
															sizeof(mcr_DispatchReceiver), mcr_ref_compare));
}

size_t DispatchReceiverSet::count() const
{
	return priv->receivers.size();
}

void DispatchReceiverSet::apply()
{
	if (applicable())
		apply(array(), priv->receivers.size());
}

void DispatchReceiverSet::setApplyReceiversPt(mcr_DispatchReceiver **applyReceiversPt)
{
	if (applyReceiversPt != _applyReceiversPt) {
		apply(nullptr, 0);
		_applyReceiversPt = applyReceiversPt;
		apply();
	}
}

void DispatchReceiverSet::setApplyCountPt(size_t *applyCountPt)
{
	if (applyCountPt != _applyCountPt) {
		apply(nullptr, 0);
		_applyCountPt = applyCountPt;
		apply();
	}
}

void DispatchReceiverSet::setApplyReceivers(mcr_DispatchReceiver **applyReceiversPt,
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
}
