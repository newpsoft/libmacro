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

#ifndef MCR_EXTRAS_TRIGGER_FUNCTIONS_H_
#define MCR_EXTRAS_TRIGGER_FUNCTIONS_H_

#include <QtCore>

#include "mcr/extras/iserializer.h"
#include "mcr/extras/references/trigger_builder.h"

namespace mcr
{
class SerTrigger;
/*! Non-exportable */
class TriggerFunctionsPrivate;
/*! QML: Acces trigger information.  C++ serialize. */
class MCR_API TriggerFunctions : public QObject
{
	Q_OBJECT
	friend class TriggerFunctionsPrivate;
public:
	explicit TriggerFunctions(QObject *parent = nullptr,
							  Libmacro *libmacroPt = nullptr);
	TriggerFunctions(const TriggerFunctions &) = delete;
	virtual ~TriggerFunctions() override;
	TriggerFunctions &operator =(const TriggerFunctions &) = delete;

	Q_INVOKABLE QVariant id(const QString &name) const;
	Q_INVOKABLE QString name(const QVariant &id) const;

	Libmacro &context() const
	{
		return *_context;
	}

	SerTrigger *serializer(size_t id) const;
	SerTrigger *serializer(const QVariant &id) const
	{
		return serializer(id.value<size_t>());
	}
	void setSerializer(size_t id, SerTrigger *(*serFnc)());
private:
	Libmacro *_context;
	TriggerFunctionsPrivate *_private;
};

/*! Non-exportable */
class SerTriggerPrivate;
class MCR_API SerTrigger : public ISerializer, public TriggerBuilder
{
	friend class SerTriggerPrivate;
public:
	/*! \brief Get a serializer object to get/set all values of another object
	 *
	 *  The serializer object is created with new and must be deleted manually
	 */
	typedef SerTrigger *(*get_serializer)();
	//! \todo Change to TriggerBuilder?
	typedef QVariant (*get)(const SerTrigger &container);
	typedef void (*set)(SerTrigger &container, const QVariant &value);

	SerTrigger(size_t reserveKeys = 0, mcr_ITrigger *valueInterface = nullptr);
	SerTrigger(const SerTrigger &) = default;
	virtual ~SerTrigger() override;
	SerTrigger &operator =(const SerTrigger &) = delete;

	virtual size_t keyCount(bool canonical) const override;
	virtual QString *keysArray(bool canonical) const override;
	virtual QVariant value(const QString &name) const override;
	virtual void setValue(const QString &name, const QVariant &val) override;

	virtual inline void setValueInterface(mcr_ITrigger *itrigPt)
	{
		mcr_throwif(!itrigPt, EFAULT);
		_valueInterface = itrigPt;
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
	mcr_ITrigger *_valueInterface;
	/* Non-exportable */
	SerTriggerPrivate *_private;
};

class MCR_API SerAction : public SerTrigger
{
public:
	SerAction();

	static QVariant modifiers(const SerTrigger &container)
	{
		return container.empty() ? 0 : container.data<mcr_Action>()->modifiers;
	}
	static void setModifiers(SerTrigger &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Action>()->modifiers = val.toUInt();
	}
	static QVariant triggerFlags(const SerTrigger &container)
	{
		return container.empty() ? 0 : container.data<mcr_Action>()->trigger_flags;
	}
	static void setTriggerFlags(SerTrigger &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Action>()->trigger_flags = val.toUInt();
	}
};

class MCR_API SerAlarm : public SerTrigger
{
public:
	SerAlarm();

	static QVariant second(const SerTrigger &container);
	static void setSecond(SerTrigger &container, const QVariant &val);
	static QVariant minute(const SerTrigger &container);
	static void setMinute(SerTrigger &container, const QVariant &val);
	static QVariant hour(const SerTrigger &container);
	static void setHour(SerTrigger &container, const QVariant &val);
	static QVariant days(const SerTrigger &container);
	static void setDays(SerTrigger &container, const QVariant &val);
};

class MCR_API SerStaged : public SerTrigger
{
public:
	SerStaged();

	static QVariant stages(const SerTrigger &container);
	static void setStages(SerTrigger &container, const QVariant &val);
};
}

#endif
