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

#include <string>

#define priv _private

namespace mcr
{
class InterruptPrivate
{
	friend class Interrupt;
public:
	std::string target;
};

IInterrupt *Interrupt::defaultInterrupt = nullptr;

Interrupt::Interrupt(IInterrupt *iintercept,
					 const char *target,
					 int type) : _iInterrupt(iintercept), _type(type), priv(new InterruptPrivate)
{
	if (target)
		priv->target = target;
}

Interrupt::Interrupt(const Interrupt &copytron) : ISignalMember(), _iInterrupt(
		copytron._iInterrupt), _type(copytron._type),
	priv(new InterruptPrivate)
{
	*priv = *copytron.priv;
}

Interrupt &Interrupt::operator =(const Interrupt &copytron)
{
	if (&copytron != this) {
		_iInterrupt = copytron._iInterrupt;
		_type = copytron._type;
		*priv = *copytron.priv;
	}
	return *this;
}

Interrupt::~Interrupt()
{
	delete priv;
}

int Interrupt::compare(const IDataMember &rhs) const
{
	const Interrupt &rig = dynamic_cast<const Interrupt &>(rhs);
	if (_type != rig._type)
		return _type < rig._type ? -1 : 1;
	return priv->target.compare(rig.priv->target);
}

void Interrupt::copy(const IDataMember *copytron)
{
	const Interrupt *rPt = dynamic_cast<const Interrupt *>(copytron);
	if (rPt) {
		if (rPt != this)
			*this = *rPt;
	} else {
		*this = Interrupt();
	}
}

void Interrupt::send(mcr_Signal *)
{
	if (_iInterrupt) {
		_iInterrupt->interrupt(target(), _type);
	} else if(defaultInterrupt) {
		defaultInterrupt->interrupt(target(), _type);
	}
}

const char *Interrupt::target() const
{
	return priv->target.c_str();
}

void Interrupt::setTarget(const char *val)
{
	priv->target = val;
}
}
