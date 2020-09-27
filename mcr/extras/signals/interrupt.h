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

#ifndef MCR_EXTRAS_SIGNALS_INTERRUPT_H_
#define MCR_EXTRAS_SIGNALS_INTERRUPT_H_

#include "mcr/extras/isignal_member.h"
#include "mcr/extras/macro.h"

namespace mcr
{
class MCR_API IInterrupt
{
public:
	MCR_DECL_INTERFACE(IInterrupt)

	/* Interrupt Signal */
	virtual int type() const = 0;
	virtual void setType(int val) = 0;
	virtual const char *target() const = 0;
	virtual void setTarget(const char *val) = 0;
	virtual IInterrupt *iinterrupt() const = 0;
	virtual void setIInterrupt(IInterrupt *val) = 0;
	/* Apply interrupt to target */
	virtual void interrupt(const char *target, int type) = 0;
};

/*! Non-exportable members */
class InterruptPrivate;
class MCR_API Interrupt : public ISignalMember, public IInterrupt
{
	friend class InterruptPrivate;
public:
	/*! Set this to iintercept if none is provided */
	static IInterrupt *defaultInterrupt;
	/*! Set of Macros to do interrupt actions on */

	Interrupt(IInterrupt *iintercept = nullptr,
			  const char *target = "All",
			  int type = Macro::INTERRUPT_ALL);
	Interrupt(const Interrupt &);
	virtual ~Interrupt();
	Interrupt &operator =(const Interrupt &);

	virtual int compare(const IDataMember &rhs) const override;
	virtual void copy(const IDataMember *copytron) override;
	virtual const char *name() const override
	{
		return "Interrupt";
	}
	virtual void send(mcr_Signal *signalPt) override;

	/*! How interrupt is processed
	 *
	 * \ref Macro::Interrupt */
	virtual int type() const override
	{
		return _type;
	}
	virtual void setType(int val) override
	{
		_type = val;
	}

	/*! Null, empty, or name of macro.
	 *
	 *  All, case insensitive, is reserved to interrupt all macros. */
	virtual const char *target() const override;
	virtual void setTarget(const char *val) override;

	virtual IInterrupt *iinterrupt() const override
	{
		return _iInterrupt;
	}
	virtual void setIInterrupt(IInterrupt *val) override
	{
		_iInterrupt = val;
	}

	virtual void interrupt(const char *target, int type) override
	{
		if (_iInterrupt)
			_iInterrupt->interrupt(target, type);
	}
private:
	IInterrupt *_iInterrupt;
	int _type;
	/* Not exportable */
	InterruptPrivate *_private;
};
}

#endif
