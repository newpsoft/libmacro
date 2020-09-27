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

#include "mcr/extras/trigger_functions.h"

#include "mcr/libmacro.h"

#define priv _private

namespace mcr
{
class TriggerFunctionsPrivate
{
	friend class TriggerFunctions;
public:
	std::vector<SerTrigger::get_serializer> serializers;
};

class SerTriggerPrivate
{
	friend class SerTrigger;
public:
	std::vector<QString> keys;
	std::vector<QString> keysCanonical;
	std::map<QString, SerTrigger::get> getMap;
	std::map<QString, SerTrigger::set> setMap;
};

TriggerFunctions::TriggerFunctions(QObject *parent, Libmacro *libmacroPt)
	: QObject(parent), _context(Libmacro::instance(libmacroPt)),
	  priv(new TriggerFunctionsPrivate)
{
	priv->serializers.resize(3, nullptr);
	setSerializer(context().iAction.interface.id, []() ->
				  SerTrigger * { return new SerAction(); });
}

TriggerFunctions::~TriggerFunctions()
{
	delete priv;
}

QVariant TriggerFunctions::id(const QString &name) const
{
	auto itrigPt = mcr_ITrigger_from_name(&*context(), name.toUtf8().constData());
	return QVariant::fromValue<size_t>(mcr_Interface_id(&itrigPt->interface));
}

QString TriggerFunctions::name(const QVariant &id) const
{
	return mcr_IRegistry_id_name(mcr_ITrigger_registry(&*context()),
								 id.value<size_t>());
}

SerTrigger *TriggerFunctions::serializer(size_t id) const
{
	auto &ser = priv->serializers;
	if (id < static_cast<size_t>(ser.size())) {
		SerTrigger::get_serializer fncPt = ser[id];
		if (fncPt)
			return fncPt();
	}
	return nullptr;
}

void TriggerFunctions::setSerializer(size_t id, SerTrigger *(*serFnc)())
{
	auto &ser = priv->serializers;
	if (id == static_cast<size_t>(-1))
		return;
	if (id >= ser.size())
		ser.resize(id + 1, nullptr);
	/* Since we cannot notify statically, errors will crash */
	ser[id] = serFnc;
}

SerTrigger::SerTrigger(size_t reserveKeys, mcr_ITrigger *valueInterface) :
	_valueInterface(valueInterface),
	priv(new SerTriggerPrivate)
{
	priv->keys.reserve(reserveKeys);
	priv->keysCanonical.reserve(reserveKeys);
}

SerTrigger::~SerTrigger()
{
	delete priv;
}

size_t SerTrigger::keyCount(bool canonical) const
{
	auto &keys = canonical ? priv->keysCanonical : priv->keys;
	return keys.size();
}

QString *SerTrigger::keysArray(bool canonical) const
{
	auto &keys = canonical ? priv->keysCanonical : priv->keys;
	if (keys.empty())
		return nullptr;
	return &keys.front();
}

QVariant SerTrigger::value(const QString &name) const
{
	auto &getMap = priv->getMap;
	auto iter = getMap.find(name);
	if (iter == getMap.end() || !iter->second)
		return QVariant();
	return iter->second(*this);
}

void SerTrigger::setValue(const QString &name, const QVariant &val)
{
	auto &setMap = priv->setMap;
	auto iter = setMap.find(name);
	if (iter == setMap.end() || !iter->second)
		return;
	mcr_throwif(!_valueInterface, EFAULT);
	if (itrigger() != _valueInterface)
		setITrigger(_valueInterface);
	mkdata();
	iter->second(*this, val);
}

void SerTrigger::setMaps(const QString &key, get fnGet, set fnSet)
{
	priv->keys.push_back(key);
	priv->getMap[key] = fnGet;
	priv->setMap[key] = fnSet;
}

void SerTrigger::setMapsCanonical(const QString &key, get fnGet, set fnSet)
{
	priv->keysCanonical.push_back(key);
	priv->getMap[key] = fnGet;
	priv->setMap[key] = fnSet;
}

SerAction::SerAction() : SerTrigger(2)
{
	setValueInterface(&context().iAction);
	setMapsGeneric("modifiers", &modifiers, &setModifiers);
	setMapsGeneric("triggerFlags", &triggerFlags, &setTriggerFlags);
}

SerStaged::SerStaged() : SerTrigger(1)
{
}

QVariant SerStaged::stages(const SerTrigger &)
{
	return QVariant();
}

void SerStaged::setStages(SerTrigger &, const QVariant &)
{
}
}
