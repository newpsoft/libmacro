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

#include "mcr/extras/util/dispatch_receiver_map.h"

#define priv _private

namespace mcr
{
class KeyDispatcherPrivate
{
	friend struct KeyDispatcher;
public:

	DispatchReceiverMap<int> keyMaps[2];
	std::vector<mcr_MapElement> keyModifiers;
	std::vector<mcr_MapElement> modifierKeys;
};

struct MapElementKeyLess :
	std::binary_function<mcr_MapElement, mcr_MapElement, bool> {
	bool operator()(const mcr_MapElement &s1, const mcr_MapElement &s2) const
	{
		return s1.first.integer < s2.first.integer;
	}
};
struct MapElementModifierLess :
	std::binary_function<mcr_MapElement, mcr_MapElement, bool> {
	bool operator()(const mcr_MapElement &s1, const mcr_MapElement &s2) const
	{
		return s1.first.u_integer < s2.first.u_integer;
	}
};

KeyDispatcher::KeyDispatcher(Libmacro *context)
	: _context(Libmacro::instance(context)),
	  priv(new KeyDispatcherPrivate)
{
	ctor();
}

KeyDispatcher::KeyDispatcher(const KeyDispatcher &copytron)
	: _context(copytron._context), priv(new KeyDispatcherPrivate)
{
	ctor();
	*priv = *copytron.priv;
	update();
}

KeyDispatcher::~KeyDispatcher()
{
	clear();
	clearModifiers();
	delete priv;
}

KeyDispatcher &KeyDispatcher::operator=(const KeyDispatcher &copytron)
{
	if (&copytron != this) {
		*priv = *copytron.priv;
		priv->keyMaps[0].setApplyReceivers(self.key_receivers + 0,
										   self.key_receiver_count + 0);
		priv->keyMaps[1].setApplyReceivers(self.key_receivers + 1,
										   self.key_receiver_count + 1);
		update();
	}
	return *this;
}

void KeyDispatcher::add(struct mcr_Signal *signalPt, void *receiver,
						mcr_dispatch_receive_fnc receiverFnc)
{
	mcr_Key *keyPt = reinterpret_cast<mcr_Key *>(signalPt ?
					 signalPt->instance.data_member.data : nullptr);
	int key = ANY;
	mcr_ApplyType applyType = MCR_BOTH;
	mcr_assert_receive_function(receiver, receiverFnc);
	if (keyPt) {
		key = keyPt->key;
		applyType = keyPt->apply;
		if (applyType < MCR_BOTH) {
			priv->keyMaps[applyType].add(key, receiver, receiverFnc);
			return;
		}
	}
	/* Generic press/unpress, add to both */
	priv->keyMaps[0].add(key, receiver, receiverFnc);
	priv->keyMaps[1].add(key, receiver, receiverFnc);
}

void KeyDispatcher::clear()
{
	priv->keyMaps[0].clear();
	priv->keyMaps[1].clear();
}

void KeyDispatcher::remove(void *remReceiver)
{
	mcr_throwif(!remReceiver, EFAULT);
	priv->keyMaps[0].remove(remReceiver);
	priv->keyMaps[1].remove(remReceiver);
}

void KeyDispatcher::trim()
{
	priv->keyMaps[0].trim();
	priv->keyMaps[1].trim();
	priv->keyModifiers.shrink_to_fit();
}

unsigned int KeyDispatcher::modifiers(int key) const
{
	return mcr_key_modifier(&**_context, key);
}

int KeyDispatcher::key(unsigned int modifiers) const
{
	return mcr_modifier_key(&**_context, modifiers);
}

void KeyDispatcher::setModifiers(int key, unsigned int modifiers, bool updateFlag)
{
	auto &keyModifiers = priv->keyModifiers;

	mcr_MapElement insert;
	insert.first.integer = key;
	insert.second.u_integer = modifiers;
	auto element = std::lower_bound(keyModifiers.begin(), keyModifiers.end(),
									insert, MapElementKeyLess());
	if (element == keyModifiers.end() || element->first.integer != key) {
		keyModifiers.insert(element, insert);
		if (updateFlag)
			update();
	} else {
		element->second.u_integer = modifiers;
	}

	addModifiers(key, modifiers, updateFlag);
}

void KeyDispatcher::addModifiers(int key, unsigned int modifiers, bool updateFlag)
{
	auto &modifierKeys = priv->modifierKeys;

	mcr_MapElement insert;
	insert.first.u_integer = modifiers;
	insert.second.integer = key;
	auto element = std::lower_bound(modifierKeys.begin(), modifierKeys.end(),
									insert, MapElementModifierLess());
	if (element == modifierKeys.end() || element->first.u_integer != modifiers) {
		modifierKeys.insert(element, insert);
		if (updateFlag)
			update();
	} else {
		element->second.integer = key;
	}
}

void KeyDispatcher::clearModifiers()
{
	_context->self.standard.key_modifiers = nullptr;
	_context->self.standard.key_modifier_count = 0;
	_context->self.standard.modifier_keys = nullptr;
	_context->self.standard.modifier_key_count = 0;
	priv->keyModifiers.clear();
	priv->modifierKeys.clear();
}

void KeyDispatcher::ctor()
{
	size_t i;
	mcr_AbsKeyDispatcher_ctor(&self, &**_context, KeyDispatcher::deinit, KeyDispatcher::add,
							  KeyDispatcher::clear, KeyDispatcher::remove, KeyDispatcher::trim);
	priv->keyMaps[0].setApplyReceivers(self.key_receivers + 0,
									   self.key_receiver_count + 0);
	priv->keyMaps[1].setApplyReceivers(self.key_receivers + 1,
									   self.key_receiver_count + 1);

	priv->keyModifiers.resize(mcr_key_modifier_default_count);
	for (i = 0; i < mcr_key_modifier_default_count; i++) {
		priv->keyModifiers[i] = mcr_key_modifier_defaults[i];
	}
	priv->modifierKeys.resize(mcr_modifier_key_default_count);
	for (i = 0; i < mcr_modifier_key_default_count; i++) {
		priv->modifierKeys[i] = mcr_modifier_key_defaults[i];
	}
	update();
}

void KeyDispatcher::update()
{
	_context->self.standard.key_modifiers = priv->keyModifiers.empty() ? nullptr : &priv->keyModifiers.front();
	_context->self.standard.key_modifier_count = priv->keyModifiers.size();
	_context->self.standard.modifier_keys = priv->modifierKeys.empty() ? nullptr : &priv->modifierKeys.front();
	_context->self.standard.modifier_key_count = priv->modifierKeys.size();
}

int KeyDispatcher::add(struct mcr_IDispatcher *idispPt,
					   struct mcr_Signal *sigPt, void *receiver, mcr_dispatch_receive_fnc receiverFnc)
{
	mcr_throwif(!idispPt, EFAULT);
	try {
		offset(idispPt)->add(sigPt, receiver, receiverFnc);
	} catch (int e) {
		return e;
	}
	return 0;
}

void KeyDispatcher::clear(struct mcr_IDispatcher *idispPt)
{
	mcr_throwif(!idispPt, EFAULT);
	offset(idispPt)->clear();
}

void KeyDispatcher::remove(struct mcr_IDispatcher *idispPt, void *remReceiver)
{
	mcr_throwif(!idispPt, EFAULT);
	offset(idispPt)->remove(remReceiver);
}

void KeyDispatcher::trim(struct mcr_IDispatcher *idispPt)
{
	mcr_throwif(!idispPt, EFAULT);
	offset(idispPt)->trim();
}
}
