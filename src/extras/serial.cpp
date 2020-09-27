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

#include "mcr/extras/util/contract.h"
#include "mcr/extras/util/string_less_ci.h"

#define priv _private

namespace mcr
{
class SerialPrivate
{
public:
	std::map<std::string, int, string_less_ci> applyNameMap;
	std::map<std::string, int, string_less_ci> dimensionNameMap;
	std::map<std::string, int, string_less_ci> interruptNameMap;
	Contract<size_t> echoContract;
	Contract<int> keyContract;
	std::map<std::string, unsigned int, string_less_ci> modifiersNameMap;
	std::map<std::string, unsigned int, string_less_ci> triggerFlagsNameMap;

	SerialPrivate() : echoContract(MCR_HIDECHO_ANY), keyContract(MCR_KEY_ANY) {}
};

Serial::Serial()
	: priv(new SerialPrivate)
{
	int iIt, iMax;
	auto &iMap = priv->applyNameMap;
	unsigned int uIt, uMax;
	auto &uMap = priv->modifiersNameMap;
	unsigned int addMods[] = {  MCR_MOD_ANY,
								MCR_ALT, MCR_ALTGR, MCR_CMD, MCR_CTRL, MCR_WIN
							 };
	const char *addModNames[][2] = { {"Any", nullptr},
		{"Option", nullptr}, {"Alt Gr", "alt_gr"}, {"Command", nullptr},
		{"Control", nullptr}, {"Window", "Windows"}
	};
	dassert(arrlen(addMods) == arrlen(addModNames));

	platformInitialize();

	iMax = applyTypeMax();
	for (iIt = 0; iIt <= iMax; iIt++) {
		iMap[applyTypeName(iIt)] = iIt;
	}

	iMap = priv->dimensionNameMap;
	iMax = dimensionMax();
	for (iIt = 0; iIt <= iMax; iIt++) {
		iMap[dimensionName(iIt)] = iIt;
	}

	iMap = priv->interruptNameMap;
	iMax = interruptMax();
	for (iIt =0; iIt <= iMax; iIt++) {
		iMap[interruptName(iIt)] = iIt;
	}

	uMax = modifiersMax();
	uMap[modifiersName(0)] = 0;
	for (uIt = 1; uIt <= uMax; uIt <<= 1) {
		uMap[modifiersName(uIt)] = uIt;
	}
	iMax = arrlen(addMods);
	for (iIt = 0; iIt < iMax; iIt++) {
		uMap[addModNames[iIt][0]] = addMods[iIt];
		if (addModNames[iIt][1])
			uMap[addModNames[iIt][1]] = addMods[iIt];
	}

	uMap = priv->triggerFlagsNameMap;
	uMax = triggerFlagsMax();
	uMap[triggerFlagsName(0)] = 0;
	for (uIt = 1; uIt <= uMax; uIt <<= 1) {
		uMap[triggerFlagsName(uIt)] = uIt;
	}
}

Serial::~Serial()
{
	delete priv;
}

int Serial::applyType(const char *name) const
{
	return nameValue<int>(priv->applyNameMap, name, -1);
}

const char *Serial::applyTypeName(int value) const
{
	switch (value) {
	case MCR_SET:
		return "Set";
	case MCR_UNSET:
		return "Unset";
	case MCR_BOTH:
		return "Both";
	case MCR_TOGGLE:
		return "Toggle";
	}
	return nullptr;
}

const char *Serial::keyPressName(int value) const
{
	if (value == MCR_SET)
		return "Press";
	if (value == MCR_UNSET)
		return "Release";
	return applyTypeName(value);
}

int Serial::dimension(const char *name) const
{
	return nameValue<int>(priv->dimensionNameMap, name, -1);
}

const char *Serial::dimensionName(int value) const
{
	switch (value) {
	case MCR_X:
		return "X";
	case MCR_Y:
		return "Y";
	case MCR_Z:
		return "Z";
	case MCR_W:
		return "W";
	}
	return nullptr;
}

size_t Serial::echo(const char *name) const
{
	return priv->echoContract.value(name);
}

size_t Serial::echoMax() const
{
	return priv->echoContract.maximum();
}

const char *Serial::echoName(size_t value) const
{
	return priv->echoContract.name(value);
}

void Serial::setEcho(size_t value, const char *name)
{
	priv->echoContract.set(value, name);
}

void Serial::addEcho(size_t value, const char * const*addNames, size_t count)
{
	priv->echoContract.add(value, addNames, count);
}

void Serial::mapEcho(size_t value, const char *name, const char * const*addNames,
					 size_t count)
{
	priv->echoContract.setMap(value, name, addNames, count);
}

int Serial::key(const char *name) const
{
	return priv->keyContract.value(name);
}

int Serial::keyMax() const
{
	return priv->keyContract.maximum();
}

size_t Serial::keyCount() const
{
	return priv->keyContract.map.size();
}

const char *Serial::keyName(int value) const
{
	return priv->keyContract.name(value);
}

void Serial::setKey(int value, const char *name)
{
	priv->keyContract.set(value, name);
}

void Serial::addKey(int value, const char * const*addNames, size_t count)
{
	priv->keyContract.add(value, addNames, count);
}

void Serial::mapKey(int value, const char *name, const char * const*addNames,
					size_t count)
{
	priv->keyContract.setMap(value, name, addNames, count);
}

unsigned int Serial::modifiers(const char *name) const
{
	return nameValue<unsigned int>(priv->modifiersNameMap, name, MCR_MOD_ANY);
}

unsigned int Serial::modifiersCount() const
{
	unsigned int flag = MCR_MF_USER, ret = 0;
	while ((flag >>= 1)) {
		++ret;
	}
	return ret;
}

const char *Serial::modifiersName(unsigned int value) const
{
	switch (value) {
	case MCR_MF_NONE:
		return "None";
	case MCR_ALT:
		return "Alt";
	case MCR_ALTGR:
		return "AltGr";
	case MCR_AMIGA:
		return "Amiga";
	case MCR_CMD:
		return "Cmd";
	case MCR_CODE:
		return "Code";
	case MCR_COMPOSE:
		return "Compose";
	case MCR_CTRL:
		return "Ctrl";
	case MCR_FN:
		return "Fn";
	case MCR_FRONT:
		return "Front";
	case MCR_GRAPH:
		return "Graph";
	case MCR_HYPER:
		return "Hyper";
	case MCR_META:
		return "Meta";
	case MCR_SHIFT:
		return "Shift";
	case MCR_SUPER:
		return "Super";
	case MCR_SYMBOL:
		return "Symbol";
	case MCR_TOP:
		return "Top";
	case MCR_WIN:
		return "Win";
	case MCR_MF_USER:
		return "User";
	}
	return nullptr;
}

unsigned int Serial::triggerFlags(const char *name) const
{
	return nameValue<unsigned int>(priv->triggerFlagsNameMap, name, -1);
}

unsigned int Serial::triggerFlagsCount() const
{
	unsigned int flag = MCR_TF_USER, ret = 0;
	while ((flag >>= 1)) {
		++ret;
	}
	return ret;
}

const char *Serial::triggerFlagsName(unsigned int value) const
{
	switch (value) {
	case MCR_TF_UNDEFINED:
		return "Undefined";
	case MCR_TF_NONE:
		return "None";
	case MCR_TF_SOME:
		return "Some";
	case MCR_TF_ALL:
		return "All";
	case MCR_TF_USER:
		return "User";
	}
	return nullptr;
}

int Serial::interrupt(const char *name) const
{
	return nameValue<int>(priv->interruptNameMap, name, -1);
}

int Serial::interruptMax() const
{
	return Macro::DISABLE;
}

int Serial::interruptCount() const
{
	return Macro::DISABLE + 1;
}

const char *Serial::interruptName(int value)
{
	switch (value) {
	case Macro::CONTINUE:
		return "Continue";
	case Macro::PAUSE:
		return "Pause";
	case Macro::INTERRUPT:
		return "Interrupt One";
	case Macro::INTERRUPT_ALL:
		return "Interrupt All";
	case Macro::DISABLE:
		return "Disable";
	}
	return nullptr;
}
}
