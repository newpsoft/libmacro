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

#ifndef MCR_EXTRAS_ISERIALIZER_H_
#define MCR_EXTRAS_ISERIALIZER_H_
#ifndef MCR_NOQT

#include <QString>
#include <QVariant>

#include "mcr/extras/base_cpp.h"

namespace mcr
{
/*! \brief Get/Set all values of an object */
class MCR_API ISerializer
{
public:
	/*! \brief Get a serializer object to get/set all values of another object
	 *
	 *  The serializer object is created with new and must be deleted manually
	 */
	typedef ISerializer *(*get)();

	MCR_DECL_INTERFACE(ISerializer)

	virtual size_t keyCount(bool canonical) const = 0;
	virtual QString *keysArray(bool canonical) const = 0;
	/*! \brief Get a named value */
	virtual QVariant value(const QString &name) const = 0;
	virtual void setValue(const QString &name, const QVariant &val) = 0;

	/* Helper functions */
	inline QStringList keys(bool canonical) const
	{
		QStringList ret;
		auto ptr = keysArray(canonical);
		for (int i = 0; i < (int)keyCount(canonical); i++) {
			ret.insert(i, ptr[i]);
		}
		return ret;
	}

	inline ISerializer &build(const QString &name, const QVariant &val)
	{
		setValue(name, val);
		return *this;
	}

	/*! \brief Get all values of the object (Serialize)
	 *
	 *  \param canonical Use string values over integers. */
	inline QVariantMap values(bool canonical)
	{
		QVariantMap ret;
		auto kz = keys(canonical);
		for (auto &i: kz) {
			ret.insert(i, value(i));
		}
		return ret;
	}
	/*! \brief Set all values of the object (Deserialize)*/
	inline void setValues(const QVariantMap &vals)
	{
		for (auto &z: vals.keys()) {
			setValue(z, vals.value(z));
		}
	}
};
}

#endif
#endif
