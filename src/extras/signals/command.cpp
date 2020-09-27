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

#include "mcr/extras/extras.h"

#include <vector>

#define priv _private

namespace mcr
{
class CommandPrivate
{
	friend class Command;
public:
	std::vector<SafeString> args;
};

IKeyProvider *Command::_keyProvider = nullptr;

Command::Command(bool cryptic)
	: _file(_keyProvider, "", cryptic), _cryptic(cryptic
			&& _keyProvider), priv(new CommandPrivate)
{
}

Command::Command(const Command &copytron)
	: ISignalMember(), _file(copytron._file), _cryptic(copytron.cryptic()),
	  priv(new CommandPrivate)
{
}

Command::~Command()
{
	delete priv;
}

Command &Command::operator =(const Command &copytron)
{
	if (&copytron != this) {
		clear();
		setCryptic(copytron.cryptic());
		setFile(copytron.file());
		*priv = *copytron.priv;
	}
	return *this;
}

void Command::setCryptic(bool val)
{
	if (val != cryptic() && _keyProvider) {
		_cryptic = val;
		_file.setCryptic(val);
		for (auto &str: priv->args) {
			str.setCryptic(val);
		}
	}
}

size_t Command::argCount() const
{
	return priv->args.size();
}

void Command::setArgCount(size_t count)
{
	priv->args.resize(count);
}

String Command::arg(size_t index) const
{
	mcr_throwif(index >= priv->args.size(), EFAULT);
	return priv->args[index].text();
}

void Command::setArg(size_t index, const String &value)
{
	if (index >= priv->args.size())
		priv->args.resize(index + 1);
	priv->args[index].setText(value);
}

int Command::compare(const Command &rhs) const
{
	int cmp;
	auto &args = priv->args;
	auto &rArgs = rhs.priv->args;
	if (&rhs == this)
		return 0;
	if ((cmp = MCR_CMP(cryptic(), rhs.cryptic())))
		return cmp;
	if ((cmp = MCR_CMP(args.size(), rArgs.size())))
		return cmp;
	if ((cmp = _file.compare(rhs._file)))
		return cmp;
	for (size_t i = 0; i < args.size(); i++) {
		if ((cmp = args[i].compare(rArgs[i])))
			return cmp;
	}
	return 0;
}

void Command::copy(const IDataMember *copytron)
{
	if (copytron == this)
		return;
	if (copytron) {
		*this = *dynamic_cast<const Command *>(copytron);
	} else {
		clear();
		_cryptic = false;
	}
}

SafeString *Command::argsArray() const
{
	if (priv->args.empty())
		return nullptr;
	return &priv->args.front();
}
}
