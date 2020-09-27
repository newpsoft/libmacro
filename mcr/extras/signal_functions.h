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

#ifndef MCR_EXTRAS_SIGNAL_FUNCTIONS_H_
#define MCR_EXTRAS_SIGNAL_FUNCTIONS_H_

#include <QtCore>

#include "mcr/extras/iserializer.h"
#include "mcr/extras/signals/command.h"
#include "mcr/extras/signals/interrupt.h"
#include "mcr/extras/signals/string_key.h"
#include "mcr/extras/references/signal_builder.h"

namespace mcr
{
class SerSignal;
class SignalFunctionsPrivate;
/*! QML: Acces signal information.  C++ serialize. */
class MCR_API SignalFunctions : public QObject
{
	Q_OBJECT
	friend class SignalFunctionsPrivate;
public:
	explicit SignalFunctions(QObject *parent = nullptr,
							 Libmacro *libmacroPt = nullptr);
	SignalFunctions(const SignalFunctions &) = delete;
	virtual ~SignalFunctions() override;
	SignalFunctions &operator =(const SignalFunctions &) = delete;

	Libmacro &context() const
	{
		return *_context;
	}
	/* ID of ISignal name */
	Q_INVOKABLE QVariant id(const QString &name) const;
	/* Name of ISignal ID */
	Q_INVOKABLE QString name(const QVariant &id) const;

	Q_INVOKABLE int modifierCount() const;
	Q_INVOKABLE unsigned int modifier(const QString &name) const;
	Q_INVOKABLE QString modifierName(unsigned int mod) const;
	Q_INVOKABLE QStringList modifierNames() const;

	Q_INVOKABLE int echoCount() const;
	Q_INVOKABLE int echo(const QString &name) const;
	Q_INVOKABLE QString echoName(int echo) const;
	Q_INVOKABLE QStringList echoNames() const;

	Q_INVOKABLE int keyCount() const;
	Q_INVOKABLE int key(const QString &name) const;
	Q_INVOKABLE QString keyName(int code) const;
	Q_INVOKABLE QStringList keyNames() const;
	Q_INVOKABLE unsigned int keyMod(int code) const;

	SerSignal *serializer(size_t id);
	SerSignal *serializer(const QVariant &id)
	{
		return serializer(id.value<size_t>());
	}
	void setSerializer(size_t id, SerSignal *(*serFnc)());
private:
	Libmacro *_context;
	/* non-exportable */
	SignalFunctionsPrivate *_private;
};

class SerSignalPrivate;
class MCR_API SerSignal : public ISerializer, public SignalBuilder
{
	friend class SerSignalPrivate;
public:
	/*! \brief Get a serializer object to get/set all values of another object
	 *
	 *  The serializer object is created with new and must be deleted manually
	 */
	typedef SerSignal *(*get_serializer)();
	//! \todo Change to SignalBuilder?
	typedef QVariant (*get)(const SerSignal &container);
	typedef void (*set)(SerSignal &container, const QVariant &value);

	SerSignal(size_t reserveKeys = 0, mcr_ISignal *valueInterface = nullptr);
	SerSignal(const SerSignal &) = default;
	virtual ~SerSignal() override;
	SerSignal &operator =(const SerSignal &) = delete;

	virtual size_t keyCount(bool canonical) const override;
	virtual QString *keysArray(bool canonical) const override;
	virtual QVariant value(const QString &name) const override;
	virtual void setValue(const QString &name, const QVariant &val) override;

	virtual inline void setValueInterface(mcr_ISignal *isigPt)
	{
		mcr_throwif(!isigPt, EFAULT);
		_valueInterface = isigPt;
	}
	/* Will modify keys, getMap, and setMap */
	virtual void setMaps(const QString &key, get fnGet, set fnSet);
	/* Will modify keysCanonical, getMap, and setMap */
	virtual void setMapsCanonical(const QString &key, get fnGet, set fnSet);
	/* Will add to both keys and keysCanonical, and add to both getMap and setMap. */
	virtual void setMapsGeneric(const QString &key, get fnGet, set fnSet)
	{
		setMaps(key, fnGet, fnSet);
		setMapsCanonical(key, fnGet, fnSet);
	}
private:
	mcr_ISignal *_valueInterface;
	/* Not exportable */
	SerSignalPrivate *_private;
};

class MCR_API SerHidEcho : public SerSignal
{
public:
	SerHidEcho();

	static QVariant echo(const SerSignal &container)
	{
		return QVariant::fromValue<size_t>(container.empty() ? 0 :
										   container.data<mcr_HidEcho>()->echo);
	}
	static void setEcho(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_HidEcho>()->echo = val.value<size_t>();
	}
	static QVariant echoName(const SerSignal &container);
	static void setEchoName(SerSignal &container, const QVariant &val);
};

class MCR_API SerKey : public SerSignal
{
public:
	SerKey();

	static QVariant key(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Key>()->key;
	}
	static void setKey(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Key>()->key = val.toInt();
	}
	static QVariant applyType(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Key>()->apply;
	}
	static void setApplyType(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Key>()->apply = static_cast<mcr_ApplyType>
				(val.toInt());
	}
	static QVariant keyName(const SerSignal &container);
	static void setKeyName(SerSignal &container, const QVariant &val);
};

class MCR_API SerModifier : public SerSignal
{
public:
	SerModifier();

	static QVariant modifiers(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Modifier>()->modifiers;
	}
	static void setModifiers(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Modifier>()->modifiers = val.toUInt();
	}
	static QVariant applyType(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Modifier>()->apply;
	}
	static void setApplyType(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Modifier>()->apply = static_cast<mcr_ApplyType>
				(val.toInt());
	}
};

class MCR_API SerMoveCursor : public SerSignal
{
public:
	SerMoveCursor();

	static QVariant justify(const SerSignal &container)
	{
		return container.empty() ? false :
			   container.data<mcr_MoveCursor>()->justify_flag;
	}
	static void setJustify(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->justify_flag = val.toBool();
	}
	static QVariant x(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_MoveCursor>()->position[MCR_X];
	}
	static void setX(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->position[MCR_X] = val.toLongLong();
	}
	static QVariant y(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_MoveCursor>()->position[MCR_Y];
	}
	static void setY(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->position[MCR_Y] = val.toLongLong();
	}
	static QVariant z(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_MoveCursor>()->position[MCR_Z];
	}
	static void setZ(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->position[MCR_Z] = val.toLongLong();
	}
};

class MCR_API SerNoOp : public SerSignal
{
public:
	SerNoOp();

	static QVariant sec(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_NoOp>()->seconds;
	}
	static void setSec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_NoOp>()->seconds = val.toInt();
	}
	static QVariant msec(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_NoOp>()->milliseconds;
	}
	static void setMsec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_NoOp>()->milliseconds = val.toInt();
	}
};

class MCR_API SerScroll : public SerSignal
{
public:
	SerScroll();

	static QVariant x(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Scroll>()->dimensions[MCR_X];
	}
	static void setX(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Scroll>()->dimensions[MCR_X] = val.toLongLong();
	}
	static QVariant y(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Scroll>()->dimensions[MCR_Y];
	}
	static void setY(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Scroll>()->dimensions[MCR_Y] = val.toLongLong();
	}
	static QVariant z(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<mcr_Scroll>()->dimensions[MCR_Z];
	}
	static void setZ(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Scroll>()->dimensions[MCR_Z] = val.toLongLong();
	}
};

class MCR_API SerCommand : public SerSignal
{
public:
	SerCommand();

	static QVariant cryptic(const SerSignal &container)
	{
		return container.empty() ? false : container.data<ICommand>()->cryptic();
	}
	static void setCryptic(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<ICommand>()->setCryptic(val.toBool());
	}
	static QVariant file(const SerSignal &container)
	{
		return QString::fromStdString(container.empty() ? "" :
									  *container.data<ICommand>()->file());
	}
	static void setFile(SerSignal &container, const QVariant &val)
	{
		QString f = val.toString();
		container.mkdata().data<ICommand>()->setFile(f.toUtf8().constData());
	}
	static QVariant args(const SerSignal &container);
	static void setArgs(SerSignal &container, const QVariant &val);
};

class MCR_API SerInterrupt : public SerSignal
{
public:
	SerInterrupt();

	static QVariant type(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<IInterrupt>()->type();
	}
	static void setType(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<IInterrupt>()->setType(val.toInt());
	}
	static QVariant target(const SerSignal &container)
	{
		return container.empty() ? "" : container.data<IInterrupt>()->target();
	}
	static void setTarget(SerSignal &container, const QVariant &val)
	{
		QString t = val.toString();
		container.mkdata().data<IInterrupt>()->setTarget(t.toUtf8().constData());
	}
};

class MCR_API SerStringKey : public SerSignal
{
public:
	SerStringKey();

	static QVariant cryptic(const SerSignal &container)
	{
		return container.empty() ? false : container.data<IStringKey>()->cryptic();
	}
	static void setCryptic(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<IStringKey>()->setCryptic(val.toBool());
	}
	static QVariant sec(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<IStringKey>()->seconds();
	}
	static void setSec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<IStringKey>()->setSeconds(val.toInt());
	}
	static QVariant msec(const SerSignal &container)
	{
		return container.empty() ? 0 : container.data<IStringKey>()->milliseconds();
	}
	static void setMsec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<IStringKey>()->setMilliseconds(val.toInt());
	}
	static QVariant text(const SerSignal &container)
	{
		return QString::fromStdString(container.empty() ? "" :
									  *container.data<IStringKey>()->text());
	}
	static void setText(SerSignal &container, const QVariant &val)
	{
		QString f = val.toString();
		container.mkdata().data<IStringKey>()->setText(f.toUtf8().constData());
	}
};
}

#endif
