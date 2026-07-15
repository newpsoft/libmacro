/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include "mcr/types.h"
#include "mcr/serial.h"
#include "mcr/factory.h"
#include "mcr/defines.h"
#include "mcr/template/string_less_ci.h"

#include <cctype>
#include <map>
#include <memory>

namespace mcr
{

struct NameValue {
	const char *name;
	unsigned int value;
};

static bool ci_equal(const char *a, const char *b)
{
	if (!a || !b)
		return a == b;
	while (*a && *b) {
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
			return false;
		++a;
		++b;
	}
	return tolower((unsigned char)*a) == tolower((unsigned char)*b);
}

template<typename T>
static T nameValue(const NameValue *table, size_t count,
		   const char *name, T notFound)
{
	for (size_t i = 0; i < count; ++i) {
		if (ci_equal(table[i].name, name))
			return (T)table[i].value;
	}
	return notFound;
}

static const NameValue apply_names[] = {
	{"Set", MCR_SET},
	{"Unset", MCR_UNSET},
	{"Both", MCR_BOTH},
	{"Toggle", MCR_TOGGLE},
};

static const NameValue dimension_names[] = {
	{"X", MCR_X},
	{"Y", MCR_Y},
	{"Z", MCR_Z},
	{"W", MCR_W},
};

static const NameValue interrupt_names[] = {
	{"Continue", IInterrupt::CONTINUE},
	{"Pause", IInterrupt::PAUSE},
	{"Interrupt One", IInterrupt::INTERRUPT},
	{"Interrupt All", IInterrupt::INTERRUPT_ALL},
	{"Disable", IInterrupt::DISABLE},
};

static const NameValue modifier_names[] = {
	{"None", MCR_MF_NONE},
	{"Any", MCR_MF_ANY},
	{"Alt", MCR_ALT},
	{"AltGr", MCR_ALTGR},
	{"Amiga", MCR_AMIGA},
	{"Cmd", MCR_CMD},
	{"Code", MCR_CODE},
	{"Compose", MCR_COMPOSE},
	{"Ctrl", MCR_CTRL},
	{"Fn", MCR_FN},
	{"Front", MCR_FRONT},
	{"Graph", MCR_GRAPH},
	{"Hyper", MCR_HYPER},
	{"Meta", MCR_META},
	{"Shift", MCR_SHIFT},
	{"Super", MCR_SUPER},
	{"Symbol", MCR_SYMBOL},
	{"Top", MCR_TOP},
	{"OS", MCR_OS},
	{"User", MCR_MF_USER},
};

static const NameValue modifier_aliases[] = {
	{"Option", MCR_ALT},
	{"Alt Gr", MCR_ALTGR},
	{"alt_gr", MCR_ALTGR},
	{"Command", MCR_CMD},
	{"Control", MCR_CTRL},
	{"Win", MCR_OS},
	{"Mac", MCR_OS},
	{"Linux", MCR_OS},
	{"Unix", MCR_OS},
	{"Windows", MCR_OS},
	{"Window", MCR_OS},
	{"Apple", MCR_OS},
	{"Macintosh", MCR_OS},
};

static const NameValue trigger_mode_names[] = {
	{"Equal", MCR_TM_EQUAL},
	{"All", MCR_TM_ALL},
	{"None", MCR_TM_NONE},
	{"Exclusive", MCR_TM_EXCLUSIVE},
	{"Inequal", MCR_TM_INEQUAL},
	{"Match", MCR_TM_MATCH},
	{"Any", MCR_TM_ANY},
	{"User", MCR_TM_USER},
};

class SerialImpl : public ISerial {
    public:
	virtual ~SerialImpl() override = default;

	virtual mcr_ApplyValue applyValue(const char *name) const override;
	virtual const char *applyValue(mcr_ApplyValue value) const override;
	virtual const char *keyPressName(mcr_Press_t value) const override;
	virtual mcr_Dimension dimension(const char *name) const override;
	virtual const char *dimension(mcr_Dimension value) const override;
	virtual mcr_ModFlags modifiers(const char *name) const override;
	virtual mcr_index_t modifiersCount() const override;
	virtual const char *modifiersName(mcr_ModFlags value) const override;
	virtual mcr_TriggerMode triggerMode(const char *name) const override;
	virtual mcr_index_t triggerModeCount() const override;
	virtual const char *
	triggerModeName(mcr_TriggerMode value) const override;
	virtual IInterrupt::Value interrupt(const char *name) const override;
	virtual IInterrupt::Value interruptMax() const override;
	virtual mcr_index_t interruptCount() const override;
	virtual const char *interruptName(IInterrupt::Value value) override;

	virtual void setModifierName(const char *name,
				    mcr_ModFlags value) override;
	virtual void
	setModifierValueName(mcr_ModFlags value, const char *name) override;
	virtual void removeModifierName(const char *name) override;
	virtual void setTriggerModeName(const char *name,
					mcr_TriggerMode value) override;
	virtual void setTriggerModeValueName(mcr_TriggerMode value,
					     const char *name) override;
	virtual void removeTriggerModeName(const char *name) override;

    private:
	std::map<std::string, unsigned int, string_less_ci>
	_modifierNameExt;
	std::map<unsigned int, std::string> _modifierValueExt;
	std::map<std::string, unsigned int, string_less_ci>
	_triggerModeNameExt;
	std::map<unsigned int, std::string> _triggerModeValueExt;
};

void ISerial::Deleter::operator()(ISerial *ptr) const
{
	delete ptr;
}

namespace internal
{
namespace factory
{

std::unique_ptr<ISerial, ISerial::Deleter> createSerial()
{
	return std::unique_ptr<ISerial, ISerial::Deleter>(new SerialImpl());
}

std::shared_ptr<ISerial> createSerialShared()
{
	return createSerial();
}

}
}

/* Extensibility — modifier extension maps */

void SerialImpl::setModifierName(const char *name, mcr_ModFlags value)
{
	if (name)
		_modifierNameExt[name] = value;
}

void SerialImpl::setModifierValueName(mcr_ModFlags value, const char *name)
{
	if (name)
		_modifierValueExt[value] = name;
}

void SerialImpl::removeModifierName(const char *name)
{
	if (name)
		_modifierNameExt.erase(name);
}

void SerialImpl::setTriggerModeName(const char *name, mcr_TriggerMode value)
{
	if (name)
		_triggerModeNameExt[name] = value;
}

void SerialImpl::setTriggerModeValueName(mcr_TriggerMode value,
					 const char *name)
{
	if (name)
		_triggerModeValueExt[value] = name;
}

void SerialImpl::removeTriggerModeName(const char *name)
{
	if (name)
		_triggerModeNameExt.erase(name);
}

mcr_ApplyValue SerialImpl::applyValue(const char *name) const
{
	if (!name)
		return (mcr_ApplyValue)-1;
	return (mcr_ApplyValue)nameValue<int>(apply_names,
					      mcr_arrlen(apply_names),
					      name, -1);
}

const char *SerialImpl::applyValue(mcr_ApplyValue value) const
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

const char *SerialImpl::keyPressName(mcr_Press_t value) const
{
	switch (value) {
	case MCR_PRESS:
		return "Press";
	case MCR_UNPRESS:
		return "Release";
	default:
		break;
	}
	return applyValue(value);
}

mcr_Dimension SerialImpl::dimension(const char *name) const
{
	if (!name)
		return (mcr_Dimension)-1;
	return (mcr_Dimension)nameValue<int>(dimension_names,
					      mcr_arrlen(dimension_names),
					      name, -1);
}

const char *SerialImpl::dimension(mcr_Dimension value) const
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
	default:
		break;
	}
	return nullptr;
}

mcr_ModFlags SerialImpl::modifiers(const char *name) const
{
	if (!name)
		return MCR_MF_ANY;
	{
		auto it = _modifierNameExt.find(name);
		if (it != _modifierNameExt.end())
			return (mcr_ModFlags)it->second;
	}
	{
		mcr_ModFlags ret = (mcr_ModFlags)nameValue<unsigned int>(
			modifier_names, mcr_arrlen(modifier_names), name,
			MCR_MF_ANY);
		if (ret != MCR_MF_ANY)
			return ret;
	}
	{
		mcr_ModFlags ret = (mcr_ModFlags)nameValue<unsigned int>(
			modifier_aliases, mcr_arrlen(modifier_aliases), name,
			MCR_MF_ANY);
		if (ret != MCR_MF_ANY)
			return ret;
	}
	return MCR_MF_ANY;
}

mcr_index_t SerialImpl::modifiersCount() const
{
	mcr_index_t flag = MCR_MF_USER, ret = 0;
	while ((flag >>= 1)) {
		++ret;
	}
	return ret;
}

const char *SerialImpl::modifiersName(mcr_ModFlags value) const
{
	{
		auto it = _modifierValueExt.find(value);
		if (it != _modifierValueExt.end())
			return it->second.c_str();
	}
	switch (value) {
	case MCR_MF_NONE:
		return "None";
	case MCR_MF_ANY:
		return "Any";
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
	case MCR_OS:
		return "OS";
	case MCR_MF_USER:
		return "User";
	}
	return nullptr;
}

mcr_TriggerMode SerialImpl::triggerMode(const char *name) const
{
	if (!name)
		return (mcr_TriggerMode)-1;
	{
		auto it = _triggerModeNameExt.find(name);
		if (it != _triggerModeNameExt.end())
			return (mcr_TriggerMode)it->second;
	}
	return (mcr_TriggerMode)nameValue<unsigned int>(
		trigger_mode_names, mcr_arrlen(trigger_mode_names),
		name, -1);
}

mcr_index_t SerialImpl::triggerModeCount() const
{
	mcr_index_t flag = MCR_TM_USER, ret = 0;
	while ((flag >>= 1)) {
		++ret;
	}
	return ret;
}

const char *SerialImpl::triggerModeName(mcr_TriggerMode value) const
{
	{
		auto it = _triggerModeValueExt.find(value);
		if (it != _triggerModeValueExt.end())
			return it->second.c_str();
	}
	switch (value) {
	case MCR_TM_EQUAL:
		return "Equal";
	case MCR_TM_ALL:
		return "All";
	case MCR_TM_NONE:
		return "None";
	case MCR_TM_EXCLUSIVE:
		return "Exclusive";
	case MCR_TM_INEQUAL:
		return "Inequal";
	// case MCR_TM_EQUAL_OR_NONE:
	// 	return "All or Nothing";
	// case MCR_TM_SOME:
	// 	return "Some";
	case MCR_TM_ANY:
		return "Any";
	case MCR_TM_USER:
		return "User";
	case MCR_TM_MATCH:
		return "Match";
	}
	return nullptr;
}

IInterrupt::Value SerialImpl::interrupt(const char *name) const
{
	if (!name)
		return (IInterrupt::Value)-1;
	return (IInterrupt::Value)nameValue<int>(
		interrupt_names, mcr_arrlen(interrupt_names),
		name, -1);
}

IInterrupt::Value SerialImpl::interruptMax() const
{
	return IInterrupt::DISABLE;
}

mcr_index_t SerialImpl::interruptCount() const
{
	return IInterrupt::DISABLE + 1;
}

const char *SerialImpl::interruptName(IInterrupt::Value value)
{
	switch (value) {
	case IInterrupt::CONTINUE:
		return "Continue";
	case IInterrupt::PAUSE:
		return "Pause";
	case IInterrupt::INTERRUPT:
		return "Interrupt One";
	case IInterrupt::INTERRUPT_ALL:
		return "Interrupt All";
	case IInterrupt::DISABLE:
		return "Disable";
	}
	return nullptr;
}
}
