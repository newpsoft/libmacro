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
 *  \brief Extra and C++ things not needed for Libmacro to function
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_EXTRAS_H_
#define MCR_EXTRAS_EXTRAS_H_

#include "mcr/extras/signals/command.h"
#include "mcr/extras/signals/interrupt.h"
#include "mcr/extras/signals/string_key.h"
#include "mcr/extras/macro.h"
#include "mcr/extras/macros_interrupted.h"
#include "mcr/extras/registry.h"
#include "mcr/extras/serial.h"
#include "mcr/extras/wrappers.h"
#include "mcr/extras/references/isignal_builder.h"
#include "mcr/extras/references/signal_builder.h"
#include "mcr/extras/references/itrigger_builder.h"
#include "mcr/extras/references/trigger_builder.h"
#include "mcr/extras/dispatcher.h"
#include "mcr/extras/key_dispatcher.h"

namespace mcr
{
/*! Non-exportable members */
class LibmacroPrivate;
/*! Platform-specific */
class LibmacroPlatform;
struct MCR_API Libmacro final // And that's final!
{
	friend class LibmacroPrivate;
	friend class LibmacroPlatform;

	mcr_context self;
	// \todo C-structs inside mcr_context as functions instead of references?

	Registry isignalRegistry;
	Registry itriggerRegistry;

	mcr_ISignal &iHidEcho;
	mcr_ISignal &iKey;
	mcr_ISignal &iModifier;
	mcr_ISignal &iMoveCursor;
	mcr_ISignal &iNoOp;
	mcr_ISignal &iScroll;
	ISignal iCommand;
	ISignal iInterrupt;
	ISignal iStringKey;

	mcr_ITrigger &iAction;
//	//Trigger iAlarm;
//	//Trigger iStaged;

	Dispatcher genericDispatcher;
	Dispatcher hidEchoDispatcher;
	KeyDispatcher keyDispatcher;
	Dispatcher modifierDispatcher;
	Dispatcher moveCursorDispatcher;
	Dispatcher noOpDispatcher;
	Dispatcher scrollDispatcher;
	Dispatcher commandDispatcher;
	Dispatcher interruptDispatcher;
	Dispatcher stringKeyDispatcher;

	//CharacterRegistry characterRegistry
	MacrosInterrupted macrosInterrupted;
	Serial serial;

	/*! \ref ctor
	 *  \param enabled Initial enabled state, must be disabled before
	 *  destruction
	 */
	Libmacro(bool enabled = true);
	Libmacro(const Libmacro &);
	/*! \ref dtor If this is enabled during destruction threading errors
	 *  are likely to happen
	 */
	~Libmacro();
	Libmacro &operator =(const Libmacro &);

	/*! Get the last created \ref Libmacro module
	 *
	 *  Will throw EFAULT if no \ref Libmacro exists, and will not create a
	 *  singleton object
	 */
	static Libmacro *instance();
	static inline Libmacro *instance(Libmacro *contextIfNotNull)
	{
		return contextIfNotNull ? contextIfNotNull : instance();
	}
	/*! Get the C++ Context of a C Context. */

	inline mcr_context &operator *()
	{
		return self;
	}
	inline const mcr_context &operator *() const
	{
		return self;
	}
	static inline Libmacro *offset(mcr_context *originPt)
	{
		return mcr::offset<Libmacro>(originPt);
	}
	static inline const Libmacro *offset(const mcr_context *originPt)
	{
		return mcr::offset<Libmacro>(originPt);
	}

	template<typename ISignalMemberT>
	ISignal &registerSignal(ISignal &isignal)
	{
		ISignalMemberT inst;
		size_t count = inst.addNameCount();
		const char **addN = count ? new const char *[count] : nullptr;
		/* If not implemented this will do nothing \ref IDataMember.addNames */
		inst.addNames(addN, count);
		mcr_register(mcr_ISignal_registry(&self), &isignal.interface(), inst.name(), addN, count);
		if (addN)
			delete []addN;
		if (mcr_err)
			throw mcr_read_err();
		return isignal;
	}
	template<typename ITriggerMemberT>
	ITrigger &registerTrigger(ITrigger &itrigger)
	{
		ITriggerMemberT inst;
		size_t count = inst.addNameCount();
		const char * const*addN = count ? new const char *[count] : nullptr;
		/* If not implemented this will do nothing \ref IDataMember.addNames */
		inst.addNames(addN, count);
		mcr_register(mcr_ITrigger_registry(&self), &itrigger.interface(), inst.name(), addN, count);
		if (addN)
			delete []addN;
		if (mcr_err)
			throw mcr_read_err();
		return itrigger;
	}

	// \todo iAlarm + iStaged
	/*! Generic enabler for mcr_context functions, such as
	 *  hardware hooks and any sort of threading.
	 */
	inline bool enabled() const
	{
		return _enabled;
	}
	/*! Set current enabled state.  Please disable before the
	 *  destructor to avoid threading errors.
	 */
	void setEnabled(bool val);

	//! \todo Find a place for Key registry characters
	size_t characterCount() const;
	size_t characterCount(int c) const;
	Signal *characterData(int c) const;

	/*! Set signals for a character as a list of signals. */
	template<typename T>
	inline void setCharacter(int c, const T &val)
	{
		mcr_throwif(c < 0, EINVAL);
		setCharacter(c, &val.front(), val.size());
	}
	/*! Set signals for a character that will press, pause, and release a
	 *  key.
	 *
	 *  \param c Character to set signals for
	 *  \param key Keyboard key to press and release
	 *  \param msecDelay Milliseconds to pause between key press and release
	 *  \param shiftFlag If true a shift key will press before the key, and
	 *  release after the key.
	 *  \return \ref reterr
	 */
	void setCharacterKey(int c, int key, long msecDelay, bool shiftFlag);
	/*! Set all \ref mcr_NoOp delay values for \ref StringKey
	 *
	 *  \param delayValue Apply this pause time
	 */
	void setCharacterDelays(mcr_NoOp delayValue);
	void removeCharacter(int c);
	void trimCharacters();
	void clearCharacters();

	mcr_IDispatcher *dispatcher(size_t id) const;
	inline mcr_IDispatcher *dispatcher(mcr_ISignal *isignalPt) const
	{
		return isignalPt ? dispatcher(isignalPt->interface.id) : nullptr;
	}
	inline mcr_IDispatcher *dispatcher(mcr_Signal *signalPt) const
	{
		return signalPt && signalPt->isignal ? dispatcher(signalPt->interface->id) : nullptr;
	}
	void setDispatcher(size_t id, mcr_IDispatcher *idispatcherPt);
	inline void setGenericDispatcher(mcr_IDispatcher *idispatcherPt, bool enable = false)
	{
		setDispatcher(-1, idispatcherPt);
		self.base.generic_dispatcher_flag = enable;
	}
	inline void setDispatcher(mcr_ISignal *isignalPt, mcr_IDispatcher *idispatcherPt, bool enable = false)
	{
		if (isignalPt) {
			setDispatcher(mcr_ISignal_id(isignalPt), idispatcherPt);
			isignalPt->dispatcher_pt = enable ? idispatcherPt : nullptr;
		} else {
			setGenericDispatcher(idispatcherPt, enable);
		}
	}
	inline void setDispatcher(mcr_Signal *signalPt, mcr_IDispatcher *idispatcherPt, bool enable = false)
	{
		mcr_throwif(!signalPt, EFAULT);
		mcr_throwif(!signalPt->isignal, EFAULT);
		setDispatcher(signalPt->isignal, idispatcherPt, enable);
	}
	void clearDispatchers();

private:
	bool _enabled;
	/* non-export */
	LibmacroPrivate *_private;
	LibmacroPlatform *_platform;

	void construct(bool enabled);
	/*! \ref mcr_platform Platform initializer
	 */
	void initialize();
	/*! \ref mcr_platform Platform deinitializer
	 */
	void deinitialize();

	void setCharacter(int c, const Signal *valueArray, size_t count);
};
}

#endif
