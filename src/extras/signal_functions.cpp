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

#include "mcr/extras/signal_functions.h"

#include <map>
#include <set>
#include <vector>

#include "mcr/libmacro.h"

#define priv _private

namespace mcr
{
class SignalFunctionsPrivate
{
	friend class SignalFunctions;
public:
	std::vector<SerSignal::get_serializer> serializers;
};

class SerSignalPrivate
{
	friend class SerSignal;
public:
	std::vector<QString> keys;
	std::vector<QString> keysCanonical;
	std::map<QString, SerSignal::get> getMap;
	std::map<QString, SerSignal::set> setMap;
};

SignalFunctions::SignalFunctions(QObject *parent, Libmacro *libmacroPt)
	: QObject(parent), _context(Libmacro::instance(libmacroPt)),
	  priv(new SignalFunctionsPrivate)
{
	size_t i;
	Libmacro &ctx = context();
	mcr_ISignal *isigs[] = {
		&ctx.iHidEcho, &ctx.iKey,
		&ctx.iModifier, &ctx.iMoveCursor,
		&ctx.iNoOp, &ctx.iScroll,
		&*ctx.iCommand, &*ctx.iStringKey,
		&*ctx.iInterrupt
	};
	SerSignal::get_serializer fncs[] = {
		[]() -> SerSignal *{ return new SerHidEcho(); },
		[]() -> SerSignal *{ return new SerKey(); },
		[]() -> SerSignal *{ return new SerModifier(); },
		[]() -> SerSignal *{ return new SerMoveCursor(); },
		[]() -> SerSignal *{ return new SerNoOp(); },
		[]() -> SerSignal *{ return new SerScroll(); },
		[]() -> SerSignal *{ return new SerCommand(); },
		[]() -> SerSignal *{ return new SerStringKey(); },
		[]() -> SerSignal *{ return new SerInterrupt(); }
	};
	i = mcr_arrlen(fncs);
	priv->serializers.resize(i, nullptr);
	while (i--) {
		setSerializer(isigs[i]->interface.id, fncs[i]);
	}
}

SignalFunctions::~SignalFunctions()
{
	delete priv;
}

SerSignal *SignalFunctions::serializer(size_t id)
{
	auto &ser = priv->serializers;
	if (id < static_cast<size_t>(ser.size())) {
		SerSignal::get_serializer fncPt = ser[id];
		if (fncPt)
			return fncPt();
	}
	return nullptr;
}

void SignalFunctions::setSerializer(size_t id, SerSignal *(*serFnc)())
{
	auto &ser = priv->serializers;
	if (id == static_cast<size_t>(~0))
		return;
	if (id >= ser.size())
		ser.resize(id + 1, nullptr);
	/* Since we cannot notify statically, errors will crash */
	ser[id] = serFnc;
}

QVariant SignalFunctions::id(const QString &name) const
{
	mcr_ISignal *isigPt = mcr_ISignal_from_name(&*context(), name.toUtf8().constData());
	return QVariant::fromValue<size_t>(mcr_Interface_id(&isigPt->interface));
}

QString SignalFunctions::name(const QVariant &id) const
{
	return mcr_IRegistry_id_name(mcr_ISignal_registry(&*context()),
								 id.value<size_t>());
}

int SignalFunctions::modifierCount() const
{
	return static_cast<int>(context().serial.modifiersCount());
}

unsigned int SignalFunctions::modifier(const QString &name) const
{
	return context().serial.modifiers(name.toUtf8().constData());
}

QString SignalFunctions::modifierName(unsigned int mod) const
{
	if (mod == static_cast<unsigned int>(-1))
		return "All";
	return context().serial.modifiersName(mod);
}

QStringList SignalFunctions::modifierNames() const
{
	QStringList ret;
	size_t max = context().serial.modifiersMax();
	for (unsigned int i = 1; i <= max; i <<= 1) {
		ret << context().serial.modifiersName(i);
	}
	return ret;
}

int SignalFunctions::echoCount() const
{
	return static_cast<int>(mcr_HidEcho_count(&*context()));
}

int SignalFunctions::echo(const QString &name) const
{
	return static_cast<int>(context().serial.echo(name.toUtf8().constData()));
}

QString SignalFunctions::echoName(int echo) const
{
	return context().serial.echoName(static_cast<size_t>(echo));
}

QStringList SignalFunctions::echoNames() const
{
	QStringList ret;
	size_t count = mcr_HidEcho_count(&*context());
	for (unsigned int i = 0; i < count; i++) {
		ret << context().serial.echoName(i);
	}
	return ret;
}

int SignalFunctions::keyCount() const
{
	return static_cast<int>(context().serial.keyCount());
}

int SignalFunctions::key(const QString &name) const
{
	return context().serial.key(name.toUtf8().constData());
}

QString SignalFunctions::keyName(int code) const
{
	return context().serial.keyName(code);
}

QStringList SignalFunctions::keyNames() const
{
	QStringList ret;
	int count = keyCount();
	for (int i = 0; i < count; i++) {
		ret << context().serial.keyName(i);
	}
	return ret;
}

unsigned int SignalFunctions::keyMod(int code) const
{
	return mcr_key_modifier(&*context(), code);
}

SerSignal::SerSignal(size_t reserveKeys, mcr_ISignal *valueInterface) :
	_valueInterface(valueInterface), priv(new SerSignalPrivate)
{
	priv->keys.reserve(reserveKeys);
	priv->keysCanonical.reserve(reserveKeys);
}

SerSignal::~SerSignal()
{
	delete priv;
}

size_t SerSignal::keyCount(bool canonical) const
{
	auto &keys = canonical ? priv->keysCanonical : priv->keys;
	return keys.size();
}

QString *SerSignal::keysArray(bool canonical) const
{
	auto &keys = canonical ? priv->keysCanonical : priv->keys;
	if (keys.empty())
		return nullptr;
	return &keys.front();
}

QVariant SerSignal::value(const QString &name) const
{
	auto &getMap = priv->getMap;
	auto iter = getMap.find(name);
	if (iter == getMap.end() || !iter->second)
		return QVariant();
	return iter->second(*this);
}

void SerSignal::setValue(const QString &name, const QVariant &val)
{
	auto &setMap = priv->setMap;
	auto iter = setMap.find(name);
	if (iter == setMap.end() || !iter->second)
		return;
	mcr_throwif(!_valueInterface, EFAULT);
	if (isignal() != _valueInterface)
		setISignal(_valueInterface);
	mkdata();
	iter->second(*this, val);
}

void SerSignal::setMaps(const QString &key, get fnGet, set fnSet)
{
	priv->keys.push_back(key);
	priv->getMap[key] = fnGet;
	priv->setMap[key] = fnSet;
}

void SerSignal::setMapsCanonical(const QString &key, get fnGet, set fnSet)
{
	priv->keysCanonical.push_back(key);
	priv->getMap[key] = fnGet;
	priv->setMap[key] = fnSet;
}

SerHidEcho::SerHidEcho() : SerSignal(1)
{
	setValueInterface(&context().iHidEcho);
	setMaps("echo", &echo, &setEcho);
	setMapsCanonical("echoName", &echoName, &setEchoName);
}

QVariant SerHidEcho::echoName(const SerSignal &container)
{
	size_t e = container.empty() ? 0 : container.data<mcr_HidEcho>()->echo;
	return container.context().serial.echoName(e);
}

void SerHidEcho::setEchoName(SerSignal &container, const QVariant &val)
{
	QString n = val.toString();
	container.mkdata().data<mcr_HidEcho>()->echo = container.context().serial.echo(
				n.toUtf8().constData());
}

SerKey::SerKey() : SerSignal(2)
{
	setValueInterface(&context().iKey);
	setMaps("key", &key, &setKey);
	setMapsCanonical("keyName", &keyName, &setKeyName);
	setMapsGeneric("applyType", &applyType, &setApplyType);
}

QVariant SerKey::keyName(const SerSignal &container)
{
	int e = container.empty() ? 0 : container.data<mcr_Key>()->key;
	return container.context().serial.keyName(e);
}

void SerKey::setKeyName(SerSignal &container, const QVariant &val)
{
	QString n = val.toString();
	container.mkdata().data<mcr_Key>()->key = container.context().serial.key(
				n.toUtf8().constData());
}

SerModifier::SerModifier() : SerSignal(2)
{
	setValueInterface(&context().iModifier);
	setMapsGeneric("modifiers", &modifiers, &setModifiers);
	setMapsGeneric("applyType", &applyType, &setApplyType);
}

SerMoveCursor::SerMoveCursor() : SerSignal(4)
{
	setValueInterface(&context().iMoveCursor);
	setMapsGeneric("justify", &justify, &setJustify);
	setMapsGeneric("x", &x, &setX);
	setMapsGeneric("y", &y, &setY);
	setMapsGeneric("z", &z, &setZ);
}

SerNoOp::SerNoOp() : SerSignal(2)
{
	setValueInterface(&context().iNoOp);
	setMapsGeneric("sec", &sec, &setSec);
	setMapsGeneric("msec", &msec, &setMsec);
}

SerScroll::SerScroll() : SerSignal(3)
{
	setValueInterface(&context().iScroll);
	setMapsGeneric("x", &x, &setX);
	setMapsGeneric("y", &y, &setY);
	setMapsGeneric("z", &z, &setZ);
}

SerCommand::SerCommand() : SerSignal(3)
{
	setValueInterface(&*context().iCommand);
	setMapsGeneric("cryptic", &cryptic, &setCryptic);
	setMapsGeneric("file", &file, &setFile);
	setMapsGeneric("args", &args, &setArgs);
}

QVariant SerCommand::args(const SerSignal &container)
{
	QStringList ret;
	size_t count;
	auto *cmd = container.data<ICommand>();
	if (!container.empty()) {
		count = cmd->argCount();
		for (size_t i = 0; i < count; i++) {
			ret << *cmd->arg(i);
		}
	}
	return ret;
}

void SerCommand::setArgs(SerSignal &container, const QVariant &val)
{
	QStringList list = val.toStringList();
	auto *pt = container.mkdata().data<ICommand>();
	pt->setArgCount(list.size());
	for (size_t i = 0; i < (size_t)list.size(); i++) {
		pt->setArg(i, list[(int)i].toUtf8().data());
	}
}

SerInterrupt::SerInterrupt() : SerSignal(2)
{
	setValueInterface(&*context().iInterrupt);
	setMapsGeneric("type", &type, &setType);
	setMapsGeneric("target", &target, &setTarget);
}

SerStringKey::SerStringKey() : SerSignal(4)
{
	setValueInterface(&*context().iStringKey);
	setMapsGeneric("cryptic", &cryptic, &setCryptic);
	setMapsGeneric("sec", &sec, &setSec);
	setMapsGeneric("msec", &msec, &setMsec);
	setMapsGeneric("text", &text, &setText);
}
}
