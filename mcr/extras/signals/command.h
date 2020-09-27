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
 *  \brief \ref Command - Execute a shell command, similar to \ref execvp,
 *  with restrictions and without closing current process.
 */

#ifndef MCR_EXTRAS_SIGNALS_COMMAND_H_
#define MCR_EXTRAS_SIGNALS_COMMAND_H_

#include "mcr/extras/isignal_member.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
class MCR_API ICommand
{
public:
	MCR_DECL_INTERFACE(ICommand)

	virtual bool cryptic() const = 0;
	virtual void setCryptic(bool val) = 0;
	/*! exec file */
	virtual String file() const = 0;
	/*! exec file */
	virtual void setFile(const String &val) = 0;
	/*! Number of exec args */
	virtual size_t argCount() const = 0;
	virtual void setArgCount(size_t val) = 0;
	virtual String arg(size_t index) const = 0;
	virtual void setArg(size_t index, const String &val) = 0;
};

/*! Non-exportable members */
class CommandPrivate;
/*! Execute a shell command, similar to \ref execvp \todo QtProcess */
class MCR_API Command : public ISignalMember, public ICommand
{
	friend class CommandPrivate;
public:
	Command(bool cryptic = false);
	Command(const Command &copytron);
	virtual ~Command() override;
	Command &operator =(const Command &copytron);

	static void setKeyProvider(IKeyProvider *provider)
	{
		_keyProvider = provider;
	}

	/*! If true, encrypt all command strings */
	virtual bool cryptic() const override
	{
		return _cryptic;
	}
	/*! Set encryption state for all command strings */
	virtual void setCryptic(bool val) override;

	/*! exec file */
	virtual String file() const override
	{
		return _file.text();
	}
	/*! exec file */
	virtual void setFile(const String &val) override
	{
		_file.setText(val);
	}

	/*! Number of exec args */
	virtual size_t argCount() const override;
	virtual void setArgCount(size_t count) override;
	virtual String arg(size_t index) const override;
	virtual void setArg(size_t index, const String &value) override;
	inline void setArg(size_t index, const std::string &value)
	{
		setArg(index, String(value.c_str(), value.size()));
	}

	/*! exec args
	 *
	 *  Requires vector-equivalent functions reserve and push_back.
	 *  Requires std::string elements.
	 */
	template<class T>
	inline T args() const
	{
		T ret;
		auto ptr = argsArray();
		size_t i, count = argCount();
		ret.reserve(count);
		for (i = 0; i < count; i++) {
			ret.push_back(*ptr[i].text());
		}
		return ret;
	}
	/*! exec args
	 *
	 *  Requires vector or initializer-equivalent function size.
	 *  Requires std::string elements.
	 */
	template<class T>
	inline void setArgs(const T &val)
	{
		size_t i = 0;
		setArgCount(val.size());
		for (auto &iter: val) {
			setArg(i++, iter);
		}
	}

	/*! \ref mcr_Signal_compare */
	virtual int compare(const IDataMember &rhs) const override
	{
		return compare(dynamic_cast<const Command &>(rhs));
	}
	/*! \ref mcr_Signal_compare */
	virtual int compare(const Command &rhs) const;
	/*! \ref mcr_Signal_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const IDataMember *copytron) override;
	/*! \ref mcr_ISignal_set_name */
	virtual const char *name() const override
	{
		return "Command";
	}
	virtual size_t addNameCount() const override
	{
		return 1;
	}
	virtual void addNames(const char **bufferOut,
						  size_t bufferLength) const override
	{
		if (bufferOut && bufferLength)
			bufferOut[0] = "Cmd";
	}
	/*! \ref mcr_platform \ref mcr_send */
	virtual void send(mcr_Signal *signalPt) override;

	inline void clear()
	{
		_file.clear();
		setArgCount(0);
	}
private:
	static IKeyProvider *_keyProvider;
	/*! Executable file */
	SafeString _file;
	bool _cryptic;
	/* Not exportable */
	CommandPrivate *_private;

	SafeString *argsArray() const;
};
}

#endif
