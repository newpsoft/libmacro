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
 *  \brief \ref IProperties - String based property manager, and a common
 *  interface to flatten objects
 */

#ifndef MCR_EXTRAS_PROPERTIES_H_
#define MCR_EXTRAS_PROPERTIES_H_

#include "mcr/extras/util/string.h"

namespace mcr
{
/*! Notifier object for property changes */
class MCR_API IOnPropertyChanged
{
public:
	MCR_DECL_INTERFACE(IOnPropertyChanged)

	virtual void onPropertyChanged(const char *propertyName) = 0;
};

/*! Object which has string properties to manage */
class MCR_API IProperties
{
public:
	MCR_DECL_INTERFACE(IProperties)

	/*! Enumerate list of properties this object manages */
	virtual const char * const*properties() const = 0;
	/*! String value of a property */
	virtual String property(const char *name) const = 0;
	/*! Set value of a property from a string */
	virtual void setProperty(const char *name, const String &value) = 0;
	/*! Get a reference to actual value of a property */
	virtual void *value(const char *name) = 0;
	/*! Copy an object reference to property */
	virtual void setValue(const char *name, void *value) = 0;
	/*! Notify a property has changed */
	virtual void setOnPropertyChanged(IOnPropertyChanged *listener) = 0;
};
}

#endif
