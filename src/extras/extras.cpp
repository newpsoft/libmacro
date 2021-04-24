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

#include "mcr/extras/references/isignal_builder.h"

#ifdef linux
	#undef linux
#endif

#include MCR_STR(mcr/extras/MCR_PLATFORM/p_extras.h)

#define priv _private

namespace mcr
{
class LibmacroPrivate
{
	friend struct Libmacro;
public:
	std::map<int, std::vector<Signal>> characters;
	std::vector<mcr_IDispatcher *> dispatchers;
};

/* Reduce bloating with private static void reference */
static std::vector<void *> _registry;

Libmacro::Libmacro(bool enabled)
	: self(), iHidEcho(self.standard.ihid_echo), iKey(self.standard.ikey),
	  iModifier(self.standard.imodifier), iMoveCursor(self.standard.imove_cursor),
	  iNoOp(self.standard.inoop), iScroll(self.standard.iscroll), iCommand(this),
	  iInterrupt(this), iStringKey(this), iAction(self.standard.iaction),
	  genericDispatcher(this), hidEchoDispatcher(this), keyDispatcher(this),
	  modifierDispatcher(this), moveCursorDispatcher(this), noOpDispatcher(this),
	  scrollDispatcher(this), commandDispatcher(this), interruptDispatcher(this),
	  stringKeyDispatcher(this), _enabled(false), priv(new LibmacroPrivate),
	  _platform(new LibmacroPlatform(*this))
{
	construct(enabled);
}

Libmacro::Libmacro(const Libmacro &copytron)
	: self(), iHidEcho(self.standard.ihid_echo), iKey(self.standard.ikey),
	  iModifier(self.standard.imodifier), iMoveCursor(self.standard.imove_cursor),
	  iNoOp(self.standard.inoop), iScroll(self.standard.iscroll), iCommand(this),
	  iInterrupt(this), iStringKey(this), iAction(self.standard.iaction),
	  genericDispatcher(this), hidEchoDispatcher(this), keyDispatcher(this),
	  modifierDispatcher(this), moveCursorDispatcher(this), noOpDispatcher(this),
	  scrollDispatcher(this), commandDispatcher(this), interruptDispatcher(this),
	  stringKeyDispatcher(this), _enabled(false), priv(new LibmacroPrivate),
	  _platform(new LibmacroPlatform(*this))
{
	construct(copytron.enabled());
	/* Cannot copy macros from other context */
}

// Do not throw error in destructor, it may crash program while closing.
Libmacro::~Libmacro()
{
	/* This is supposed to happen in mcr_deinitialize, but some maps seem to
	 * be deleted early. */
//	itriggerRegistry.clear();
//	isignalRegistry.clear();

	/* Deinit before removing from registry. (Libmacro::instance is required.) */
	SafeString::deinitialize();
	deinitialize();
	if (enabled()) {
		fprintf(stderr, "Error: Libmacro context was not disabled "
				"before destruction.  "
				"Threading errors may occur.\n");
		fprintf(stderr, "Warning: mcr_deinitialize errors are ignored.\n");
	}
	if (mcr_deinitialize(&self))
		dmsg;
	auto iter = std::find(_registry.begin(), _registry.end(), this);
	/* Assume placed only once */
	if (iter != _registry.end())
		_registry.erase(iter);

	delete priv;
	delete _platform;
}

Libmacro &Libmacro::operator=(const Libmacro &copytron)
{
	if (&copytron != this)
		setEnabled(copytron.enabled());
	/* Cannot copy macros from other context */
	return *this;
}

Libmacro *Libmacro::instance()
{
	mcr_throwif(_registry.size() == 0, EFAULT);
	return reinterpret_cast<Libmacro *>(_registry.back());
}

void Libmacro::setEnabled(bool val)
{
	if (val != _enabled) {
		_enabled = val;
		mcr_dispatch_set_enabled_all(&self, val);
		/* Operation not permitted if not elevated permissions */
		if (mcr_intercept_set_enabled(&self, val) &&
			mcr_err != EPERM) {
			throwError(MCR_LINE, mcr_read_err());
		}
	}
}

size_t Libmacro::characterCount() const
{
	return priv->characters.size();
}

size_t Libmacro::characterCount(int c) const
{
	auto found = priv->characters.find(c);
	if (found == priv->characters.end())
		return 0;
	return found->second.size();
}

Signal *Libmacro::characterData(int c) const
{
	auto found = priv->characters.find(c);
	if (found == priv->characters.end())
		return nullptr;
	return &found->second.front();
}

void Libmacro::setCharacterKey(int c, int key, long msecDelay,
							   bool shiftFlag)
{
	Signal delaySig(&iNoOp), keySig(&iKey);
	SignalBuilder ref;
	size_t i;
	/* Shift: 2 keys to press */
	int gearShift = shiftFlag ? 2 : 1;
	mcr_Key *kPt;
	/* With shift will be: down, pause, down, pause, up, pause, up */
	std::vector<Signal> insert(shiftFlag ? 7 : 3);
	for (i = 0; i < insert.size(); i++) {
		ref.build(&*insert[i]);
		/* First set ISignal and instance data. Odd numbers are delays */
		if (i % 2) {
			ref.build(&iNoOp).mkdata();
			ref.data<mcr_NoOp>()->seconds = msecDelay / 1000;
			ref.data<mcr_NoOp>()->milliseconds = msecDelay % 1000;
		} else {
			kPt = ref.build(&iKey).mkdata().data<mcr_Key>();
			kPt->key = key;
			if (gearShift) {
				--gearShift;
				kPt->apply = MCR_SET;
			} else {
				kPt->apply = MCR_UNSET;
			}
		}
	}
	if (shiftFlag) {
		gearShift = mcr_modifier_key(&self, MCR_SHIFT);
		insert[0].data<mcr_Key>()->key = gearShift;
		insert[6].data<mcr_Key>()->key = gearShift;
	}
	priv->characters[c] = insert;
}

void Libmacro::setCharacterDelays(mcr_NoOp delayValue)
{
	SignalBuilder siggy;
	mcr_ISignal *isigPt = &iNoOp;
	for (auto &i: priv->characters) {
		for (auto &iter: i.second) {
			siggy.build(&*iter);
			if (siggy.isignal() == isigPt)
				*siggy.mkdata().data<mcr_NoOp>() = delayValue;
		}
	}
}

void Libmacro::removeCharacter(int c)
{
	auto found = priv->characters.find(c);
	if (found != priv->characters.end())
		priv->characters.erase(c);
}

void Libmacro::trimCharacters()
{
	for (auto iter = priv->characters.begin(); iter != priv->characters.end();) {
		if (iter->second.empty()) {
			priv->characters.erase(iter);
			// Map erase iterator is unstable
			iter = priv->characters.begin();
		} else {
			iter->second.shrink_to_fit();
			++iter;
		}
	}
}

void Libmacro::clearCharacters()
{
	priv->characters.clear();
}

mcr_IDispatcher *Libmacro::dispatcher(size_t id) const
{
	return id < priv->dispatchers.size() ? priv->dispatchers[id] : nullptr;
}

void Libmacro::setDispatcher(size_t id, mcr_IDispatcher *idispatcherPt)
{
	if (id == (size_t)~0) {
		self.base.generic_dispatcher_pt = idispatcherPt;
	} else {
		if (id >= priv->dispatchers.size())
			priv->dispatchers.resize(id + 1, nullptr);
		priv->dispatchers[id] = idispatcherPt;
		if (mcr_dispatch_set_dispatchers(&self, &priv->dispatchers.front(),
										 priv->dispatchers.size()))
			throw mcr_read_err();
	}
}

void Libmacro::clearDispatchers()
{
	if (mcr_dispatch_set_dispatchers(&self, NULL, 0))
		throw mcr_read_err();
	priv->dispatchers.clear();
}

void Libmacro::construct(bool enabled)
{
	mcr_ISignal *isignals[] = {
		&iHidEcho, &iKey, &iModifier, &iMoveCursor, &iNoOp, &iScroll,
		&*iCommand, &*iInterrupt, &*iStringKey
	};
	mcr_IDispatcher *idispatchers[] = {
		&hidEchoDispatcher.self.idispatcher, &keyDispatcher.self.idispatcher,
		&modifierDispatcher.self.idispatcher, &moveCursorDispatcher.self.idispatcher,
		&noOpDispatcher.self.idispatcher, &scrollDispatcher.self.idispatcher,
		&commandDispatcher.self.idispatcher, &interruptDispatcher.self.idispatcher,
		&stringKeyDispatcher.self.idispatcher
	};
	/* Register completed Libmacro context, remove on dtor */
	_registry.push_back(this);
	if (mcr_initialize(&self))
		throwError(MCR_LINE, mcr_read_err());
	self.base.isignal_registry_pt = &*isignalRegistry;
	self.base.itrigger_registry_pt = &*itriggerRegistry;
	/* Types and contract */
	if (mcr_load_contracts(&self)) {
		dmsg;
		mcr_deinitialize(&self);
		throwError(MCR_LINE, mcr_read_err());
	}
	registerSignal<Command>(iCommand.build<Command>());
	registerSignal<Interrupt>(iInterrupt.build<Interrupt>());
	registerSignal<StringKey>(iStringKey.build<StringKey>());

	initialize();
	SafeString::initialize();
	setEnabled(enabled);
	if (!Interrupt::defaultInterrupt)
		Interrupt::defaultInterrupt = &macrosInterrupted;

	setGenericDispatcher(&genericDispatcher.self.idispatcher, true);
	for (size_t i = arrlen(idispatchers); i--;) {
		setDispatcher(isignals[i], idispatchers[i], true);
	}
	mcr_trim(&self);
}

void Libmacro::setCharacter(int c, const Signal *valueArray, size_t count)
{
	mcr_throwif(count && !valueArray, EINVAL);
	if (!count) {
		removeCharacter(c);
		return;
	}
	auto found = priv->characters.find(c);
	if (found == priv->characters.end()) {
		auto insertSuccess = priv->characters.insert({c, {}});
		mcr_throwif(!insertSuccess.second, errno);
		found = insertSuccess.first;
	}
	auto &chara = found->second;
	chara.resize(count);
	for (size_t i = 0; i < count; i++) {
		chara[i] = valueArray[i];
	}
}
}
