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
 *  \brief \ref StringKey - Turn a text string into a set of signals, such as a key
 *  press
 */

#ifndef MCR_EXTRAS_SIGNALS_STRING_KEY_H_
#define MCR_EXTRAS_SIGNALS_STRING_KEY_H_

#include "mcr/extras/isignal_member.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
class MCR_API IStringKey
{
public:
	MCR_DECL_INTERFACE(IStringKey)

	virtual bool cryptic() const = 0;
	virtual void setCryptic(bool val) = 0;
	virtual int seconds() const = 0;
	virtual void setSeconds(int val) = 0;
	virtual int milliseconds() const = 0;
	virtual void setMilliseconds(int val) = 0;
	virtual String text() const = 0;
	virtual void setText(const String &val) = 0;
};

/*! Turn a text string into a set of signals, such as a key press */
class MCR_API StringKey: public ISignalMember, public IStringKey
{
public:
	/*! Pause in-between characters */
	mcr_NoOp interval;
	/*! Set of characters to turn into signals */
	SafeString string;

	StringKey(bool cryptic = false)
		: string(_keyProvider, "", cryptic)
	{
		interval.seconds = 0;
		interval.milliseconds = 40;
	}
	StringKey(const StringKey &copytron)
		: ISignalMember(), interval(copytron.interval), string(copytron.string)
	{
	}
	virtual ~StringKey() override = default;
	StringKey &operator =(const StringKey &copytron)
	{
		if (&copytron != this) {
			interval = copytron.interval;
			string = copytron.string;
		}
		return *this;
	}

	static void setKeyProvider(IKeyProvider *provider)
	{
		_keyProvider = provider;
	}

	virtual bool cryptic() const override
	{
		return string.cryptic();
	}
	virtual void setCryptic(bool val) override
	{
		if (val != cryptic())
			string.setCryptic(val);
	}

	virtual int seconds() const override
	{
		return interval.seconds;
	}
	virtual void setSeconds(int val) override
	{
		interval.seconds = val;
	}
	virtual int milliseconds() const override
	{
		return interval.milliseconds;
	}
	virtual void setMilliseconds(int val) override
	{
		interval.milliseconds = val;
	}

	/*! Set of characters to turn into signals */
	virtual String text() const override
	{
		return string.text();
	}
	/*! Set the string to turn into signals */
	virtual void setText(const String &val) override
	{
		string.setText(val);
	}

	/*! \ref mcr_Signal_compare */
	virtual int compare(const IDataMember &rhs) const override
	{
		return compare(dynamic_cast<const StringKey &>(rhs));
	}
	/*! \ref mcr_Signal_compare */
	virtual int compare(const StringKey &rhs) const;
	/*! \ref mcr_Signal_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const IDataMember *copytron) override;
	/*! \ref mcr_ISignal_set_name */
	virtual const char *name() const override
	{
		return "StringKey";
	}
	virtual size_t addNameCount() const override
	{
		return 2;
	}
	virtual void addNames(const char **bufferOut,
						  size_t bufferLength) const override
	{
		const char *names[] = {"String Key", "string_key"};
		size_t i, count = arrlen(names);
		if (!bufferOut)
			return;
		for (i = 0; i < count && i < bufferLength; i++) {
			bufferOut[i] = names[i];
		}
	}
	/*! \ref mcr_send */
	virtual void send(mcr_Signal *signalPt) override;
private:
	static IKeyProvider *_keyProvider;
};
}

#endif
