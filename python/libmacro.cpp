/* Libmacro - Python bindings via pybind11
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "mcr/api.h"
#include "mcr/libmacro.h"
#include "mcr/factory.h"
#include "mcr/signal/interrupt.h"
#include "mcr/signal/modifier.h"
#include "mcr/signal/noop.h"
#include "mcr/signal/functor.h"
#include "mcr/trigger/action.h"
#include "mcr/trigger/functor.h"

namespace py = pybind11;
using namespace mcr;

/* ── Trampoline classes ────────────────────────────────────────────────── */

class PySignal : public Signal {
public:
	const char *name() const override
	{
		PYBIND11_OVERRIDE_PURE(const char *, Signal, name);
	}
	void send() override
	{
		PYBIND11_OVERRIDE_PURE(void, Signal, send);
	}
	const char *alias(unsigned int aliasNumber) const override
	{
		PYBIND11_OVERRIDE(const char *, Signal, alias, aliasNumber);
	}
};

class PyTrigger : public Trigger {
public:
	const char *name() const override
	{
		PYBIND11_OVERRIDE_PURE(const char *, Trigger, name);
	}
	bool receive(Signal *signalPtr, unsigned int mods) override
	{
		PYBIND11_OVERRIDE_PURE(bool, Trigger, receive, signalPtr,
			mods);
	}
	const char *alias(unsigned int aliasNumber) const override
	{
		PYBIND11_OVERRIDE(const char *, Trigger, alias, aliasNumber);
	}
};

/* ── Module ────────────────────────────────────────────────────────────── */

PYBIND11_MODULE(_libmacro, m)
{
	m.doc() = "Libmacro - A multi-platform macro and hotkey library";

	/* ── Exception translation ─────────────────────── */
	static py::exception<mcr::Error> exc(m, "LibmacroError");
	py::register_exception_translator([](std::exception_ptr p) {
		try {
			if (p)
				std::rethrow_exception(p);
		} catch (const mcr::Error &e) {
			py::set_error(exc, e.what());
		}
	});

	/* ── Enums ─────────────────────────────────────── */
	py::enum_<mcr::Dimension>(m, "Dimension")
		.value("X", MCR_X)
		.value("Y", MCR_Y)
		.value("Z", MCR_Z)
		.value("W", MCR_W);

	py::enum_<mcr_ApplyValue>(m, "ApplyValue")
		.value("SET", MCR_SET)
		.value("UNSET", MCR_UNSET)
		.value("BOTH", MCR_BOTH)
		.value("TOGGLE", MCR_TOGGLE);

	py::enum_<mcr_ModFlags>(m, "ModFlags")
		.value("NONE", MCR_MF_NONE)
		.value("ALT", MCR_ALT)
		.value("ALTGR", MCR_ALTGR)
		.value("AMIGA", MCR_AMIGA)
		.value("CMD", MCR_CMD)
		.value("CODE", MCR_CODE)
		.value("COMPOSE", MCR_COMPOSE)
		.value("CTRL", MCR_CTRL)
		.value("FN", MCR_FN)
		.value("FRONT", MCR_FRONT)
		.value("GRAPH", MCR_GRAPH)
		.value("HYPER", MCR_HYPER)
		.value("META", MCR_META)
		.value("SHIFT", MCR_SHIFT)
		.value("SUPER", MCR_SUPER)
		.value("SYMBOL", MCR_SYMBOL)
		.value("TOP", MCR_TOP)
		.value("OS", MCR_OS)
		.value("WIN", MCR_WIN)
		.value("USER", MCR_MF_USER)
		.value("ANY", MCR_MF_ANY);

	py::enum_<mcr_TriggerMode>(m, "TriggerMode")
		.value("EQUAL", MCR_TM_EQUAL)
		.value("ALL", MCR_TM_ALL)
		.value("NONE", MCR_TM_NONE)
		.value("EXCLUSIVE", MCR_TM_EXCLUSIVE)
		.value("INEQUAL", MCR_TM_INEQUAL)
		.value("MATCH", MCR_TM_MATCH)
		.value("ANY", MCR_TM_ANY)
		.value("USER", MCR_TM_USER);

	py::enum_<IInterrupt::Value>(m, "InterruptValue")
		.value("CONTINUE", IInterrupt::CONTINUE)
		.value("PAUSE", IInterrupt::PAUSE)
		.value("INTERRUPT", IInterrupt::INTERRUPT)
		.value("INTERRUPT_ALL", IInterrupt::INTERRUPT_ALL)
		.value("DISABLE", IInterrupt::DISABLE);

	/* ── SpacePosition ────────────────────────────── */
	py::class_<mcr::SpacePosition>(m, "SpacePosition")
		.def(py::init<>())
		.def_property_readonly("array",
			[](mcr::SpacePosition &sp) {
				py::list lst;
				for (int i = 0; i < MCR_DIMENSION_COUNT; i++)
					lst.append(sp.array[i]);
				return lst;
			})
		.def_property("x",
			[](mcr::SpacePosition &sp) { return sp.point.x; },
			[](mcr::SpacePosition &sp, long long v) {
				sp.point.x = v; })
		.def_property("y",
			[](mcr::SpacePosition &sp) { return sp.point.y; },
			[](mcr::SpacePosition &sp, long long v) {
				sp.point.y = v; })
		.def_property("z",
			[](mcr::SpacePosition &sp) { return sp.point.z; },
			[](mcr::SpacePosition &sp, long long v) {
				sp.point.z = v; })
		.def_property("w",
			[](mcr::SpacePosition &sp) { return sp.point.w; },
			[](mcr::SpacePosition &sp, long long v) {
				sp.point.w = v; });

	/* ── C API functions ──────────────────────────── */
	m.def("flags_combine", [](py::iterable iter) {
		std::vector<unsigned int> v;
		for (auto item : iter)
			v.push_back(item.cast<unsigned int>());
		return mcr_flags_combine(v.data(), v.size());
	});
	m.def("flags_add", &mcr_flags_add);
	m.def("flags_has", &mcr_flags_has);
	m.def("flags_remove", &mcr_flags_remove);

	/* ── IInterrupt (base) ────────────────────────── */
	py::class_<IInterrupt>(m, "IInterrupt")
		.def("interrupt", &IInterrupt::interrupt);

	/* ── IReceive (base) ──────────────────────────── */
	py::class_<IReceive>(m, "IReceive")
		.def("receive", &IReceive::receive);

	/* ── IActor (base) ────────────────────────────── */
	py::class_<IActor>(m, "IActor")
		.def("act", &IActor::act);

	/* ── IDispatcher ──────────────────────────────── */
	py::class_<IDispatcher>(m, "IDispatcher")
		.def("add",
			[](IDispatcher &self, Signal *signalPtr,
			   IReceive *receiverPtr) {
				self.add(signalPtr, receiverPtr);
			})
		.def("clear", &IDispatcher::clear)
		.def("dispatch", &IDispatcher::dispatch)
		.def("modifier",
			[](IDispatcher &self, Signal *signalPtr,
			   unsigned int mods) {
				self.modifier(signalPtr, &mods);
				return mods;
			})
		.def("remove", &IDispatcher::remove)
		.def("trim", &IDispatcher::trim)
		.def("count", &IDispatcher::count)
		.def("empty", &IDispatcher::empty)
		.def("__len__", &IDispatcher::count);

	/* ── Signal + subclasses ─────────────────────── */
	py::class_<Signal, PySignal>(m, "Signal")
		.def(py::init<>())
		.def_readwrite("dispatcher_ptr", &Signal::dispatcherPtr)
		.def_readwrite("dispatch_flag", &Signal::dispatchFlag)
		.def("name", &Signal::name)
		.def("alias", &Signal::alias)
		.def("send", &Signal::send);

	py::class_<Modifier, Signal>(m, "Modifier")
		.def(py::init<Libmacro *>(), py::arg("context") = nullptr)
		.def_readwrite("context", &Modifier::context)
		.def_readwrite("modifiers", &Modifier::modifiers)
		.def_readwrite("apply", &Modifier::apply);

	py::class_<NoOp, Signal>(m, "NoOp")
		.def(py::init<>())
		.def_readwrite("seconds", &NoOp::seconds)
		.def_readwrite("milliseconds", &NoOp::milliseconds);

	py::class_<Interrupt, Signal>(m, "Interrupt")
		.def(py::init<>())
		.def_readwrite("target", &Interrupt::target)
		.def_readwrite("value", &Interrupt::value);

	py::class_<FunctorSignal, Signal>(m, "FunctorSignal")
		.def(py::init([](py::function fn) {
			return std::unique_ptr<FunctorSignal>(
				new FunctorSignal([fn]() { fn(); }));
		}));

	/* ── Trigger + subclasses ────────────────────── */
	py::class_<Trigger, PyTrigger>(m, "Trigger")
		.def(py::init<>())
		.def_readwrite("actor_ptr", &Trigger::actorPtr)
		.def_readwrite("blocking_flag", &Trigger::blockingFlag)
		.def("name", &Trigger::name)
		.def("alias", &Trigger::alias)
		.def("receive", &Trigger::receive)
		.def("trigger", &Trigger::trigger);

	py::class_<Action, Trigger>(m, "Action")
		.def(py::init<>())
		.def_readwrite("modifiers", &Action::modifiers)
		.def_readwrite("trigger_mode", &Action::triggerMode);

	py::class_<FunctorTrigger, Trigger>(m, "FunctorTrigger")
		.def(py::init([](py::function fn) {
			return std::unique_ptr<FunctorTrigger>(
				new FunctorTrigger(
					[fn](Signal *s, unsigned int m) {
						return fn(s, m)
							.template cast<bool>();
					}));
		}));

	/* ── IMacro ──────────────────────────────────── */
	py::class_<IMacro,
		std::unique_ptr<IMacro, IMacro::Deleter>>(m, "Macro",
			py::dynamic_attr())
		.def_property("context",
			py::overload_cast<>(&IMacro::context,
				py::const_),
			&IMacro::setContext)
		.def_property("blocking",
			&IMacro::blocking, &IMacro::setBlocking)
		.def_property("enabled",
			[](IMacro &self) { return self.enabled(); },
			[](IMacro &self, bool val) { self.setEnabled(val); })
		.def_property("interruptor",
			&IMacro::interruptor, &IMacro::setInterruptor)
		.def_property("name",
			[](IMacro &self) {
				auto *n = self.name();
				return n ? py::object(py::str(n)) : py::object(py::none());
			},
			[](IMacro &self, const char *val) {
				self.setName(val); })
		.def_property("sticky",
			&IMacro::sticky, &IMacro::setSticky)
		.def_property("thread_max",
			&IMacro::threadMax, &IMacro::setThreadMax)
		.def_property("apply_dispatch_enabled",
			&IMacro::applyDispatchEnabled,
			&IMacro::setApplyDispatchEnabled)
		.def_property_readonly("thread_count", &IMacro::threadCount)
		.def_property_readonly("queued", &IMacro::queued)
		.def_property_readonly("global_active_thread_count",
			&IMacro::globalActiveThreadCount)
		.def_property_readonly("running",
			[](IMacro &self) { return self.threadCount(); })

		/*
		 *  set_activators / set_signals / set_triggers
		 *
		 *  The C++ API takes a contiguous C array (const Signal* / const
		 *  Trigger*) and stores the address of each element. Python-owned
		 *  objects are individually heap-allocated and cannot form a
		 *  contiguous array without slicing, which corrupts the vtable
		 *  of polymorphic types.
		 *
		 *  Instead, these methods:
		 *    1. Store a py::list reference on the Python wrapper (prevents GC)
		 *    2. Clear the internal C++ lists via set*(nullptr, 0)
		 *    3. Register dispatch relationships directly through the
		 *       dispatcher API (for activators/triggers). For the signal
		 *       list (the signals a macro sends when it runs), we store
		 *       them and the user should invoke them via add_dispatch
		 *       or direct signal.send() calls in a custom thread.
		 */
		.def("set_activators",
			[](py::object self, py::iterable iter) {
				auto &macro = self.cast<IMacro &>();
				py::list lst;
				std::vector<Signal *> ptrs;
				for (auto item : iter) {
					lst.append(item);
					ptrs.push_back(
						&item.cast<Signal &>());
				}
				self.attr("_held_activators") = lst;
				macro.setActivators(nullptr, 0);
				for (auto *sig : ptrs) {
					auto *d = sig->dispatcherPtr;
					if (d)
						d->add(sig, &macro);
				}
			})
		.def("set_signals",
			[](py::object self, py::iterable iter) {
				auto &macro = self.cast<IMacro &>();
				py::list lst;
				for (auto item : iter) {
					lst.append(item);
				}
				self.attr("_held_signals") = lst;
				macro.setSignals(nullptr, 0);
			})
		.def("set_triggers",
			[](py::object self, py::iterable iter) {
				auto &macro = self.cast<IMacro &>();
				py::list lst;
				std::vector<Trigger *> ptrs;
				for (auto item : iter) {
					lst.append(item);
					ptrs.push_back(
						&item.cast<Trigger &>());
				}
				self.attr("_held_triggers") = lst;
				macro.setTriggers(nullptr, 0);
				for (auto *trig : ptrs) {
					trig->actorPtr = &macro;
					/* triggers use generic dispatcher
					 * when no activators are set */
				}
			})
		.def("clear_activators",
			[](IMacro &self) {
				self.setActivators(nullptr, 0); })
		.def("clear_signals",
			[](IMacro &self) {
				self.setSignals(nullptr, 0); })
		.def("clear_triggers",
			[](IMacro &self) {
				self.setTriggers(nullptr, 0); })
		.def("clear_all",
			[](IMacro &self) {
				self.setActivators(nullptr, 0);
				self.setSignals(nullptr, 0);
				self.setTriggers(nullptr, 0);
			})
		.def("start", &IMacro::start)
		.def("run", &IMacro::run)
		.def("copy",
			[](IMacro &self, IMacro &other) {
				self.copy(other);
			})
		.def("apply_dispatch", &IMacro::applyDispatch)
		.def("add_dispatch",
			py::overload_cast<>(&IMacro::addDispatch))
		.def("remove_dispatch", &IMacro::removeDispatch)
		.def("add_dispatch_for_signal",
			[](IMacro &self, Signal &signalPtr) {
				self.addDispatch(signalPtr);
			})
		.def("add_dispatch_for_trigger",
			[](IMacro &self, Trigger &trigPtr) {
				self.addDispatch(trigPtr);
			})
		.def("add_dispatch_for_pair",
			[](IMacro &self, Signal &signalPtr,
			   Trigger &trigPtr) {
				self.addDispatch(signalPtr, trigPtr);
			});

	/* ── ISerial ─────────────────────────────────── */
	py::class_<ISerial,
		std::unique_ptr<ISerial, ISerial::Deleter>>(m,
		"ISerial")
		.def("apply_value",
			py::overload_cast<const char *>(
				&ISerial::applyValue, py::const_))
		.def("apply_value_name",
			py::overload_cast<mcr_ApplyValue>(
				&ISerial::applyValue, py::const_))
		.def("apply_type_max", &ISerial::applyTypeMax)
		.def("apply_type_count", &ISerial::applyTypeCount)
		.def("key_press", &ISerial::keyPress)
		.def("key_press_max", &ISerial::keyPressMax)
		.def("key_press_count", &ISerial::keyPressCount)
		.def("key_press_name", &ISerial::keyPressName)
		.def("dimension",
			py::overload_cast<const char *>(
				&ISerial::dimension, py::const_))
		.def("dimension_name",
			py::overload_cast<mcr_Dimension>(
				&ISerial::dimension, py::const_))
		.def("dimension_max", &ISerial::dimensionMax)
		.def("dimension_count", &ISerial::dimensionCount)
		.def("modifiers",
			py::overload_cast<const char *>(
				&ISerial::modifiers, py::const_))
		.def("modifiers_name", &ISerial::modifiersName)
		.def("modifiers_max", &ISerial::modifiersMax)
		.def("modifiers_count", &ISerial::modifiersCount)
		.def("trigger_mode",
			py::overload_cast<const char *>(
				&ISerial::triggerMode, py::const_))
		.def("trigger_mode_name", &ISerial::triggerModeName)
		.def("trigger_mode_max", &ISerial::triggerModeMax)
		.def("trigger_mode_count", &ISerial::triggerModeCount)
		.def("interrupt",
			py::overload_cast<const char *>(
				&ISerial::interrupt, py::const_))
		.def("interrupt_name", &ISerial::interruptName)
		.def("interrupt_max", &ISerial::interruptMax)
		.def("interrupt_count", &ISerial::interruptCount)
		.def("set_modifier_name", &ISerial::setModifierName)
		.def("set_modifier_value_name",
			&ISerial::setModifierValueName)
		.def("remove_modifier_name", &ISerial::removeModifierName)
		.def("set_trigger_mode_name", &ISerial::setTriggerModeName)
		.def("set_trigger_mode_value_name",
			&ISerial::setTriggerModeValueName)
		.def("remove_trigger_mode_name",
			&ISerial::removeTriggerModeName);

	/* ── IMacroRegistry ──────────────────────────── */
	py::class_<IMacroRegistry,
		std::unique_ptr<IMacroRegistry,
			IMacroRegistry::Deleter>>(m, "IMacroRegistry")
		.def("interrupt", &IMacroRegistry::interrupt)
		.def("map",
			[](IMacroRegistry &self, const char *name,
			   IMacro &macro) { self.map(name, &macro); })
		.def("unmap", &IMacroRegistry::unmap)
		.def("find",
			[](IMacroRegistry &self, const char *name) {
				auto *found = self.find(name);
				return found
					? py::cast(found,
						py::return_value_policy::
							reference)
					: py::none();
			})
		.def("clear", &IMacroRegistry::clear);

	/* ── ISignalRegistry ─────────────────────────── */
	py::class_<ISignalRegistry,
		std::unique_ptr<ISignalRegistry,
			ISignalRegistry::Deleter>>(m, "ISignalRegistry")
		.def("allocate",
			[](ISignalRegistry &self, const char *name) {
				auto *ptr = self.allocate(name);
				return ptr
					? py::cast(ptr,
						py::return_value_policy::
							take_ownership)
					: py::none();
			})
		.def("deallocate", &ISignalRegistry::deallocate);

	/* ── ITriggerRegistry ────────────────────────── */
	py::class_<ITriggerRegistry,
		std::unique_ptr<ITriggerRegistry,
			ITriggerRegistry::Deleter>>(m,
		"ITriggerRegistry")
		.def("allocate",
			[](ITriggerRegistry &self, const char *name) {
				auto *ptr = self.allocate(name);
				return ptr
					? py::cast(ptr,
						py::return_value_policy::
							take_ownership)
					: py::none();
			})
		.def("deallocate", &ITriggerRegistry::deallocate);

	/* ── Libmacro ────────────────────────────────── */
	py::class_<Libmacro,
		std::unique_ptr<Libmacro, Libmacro::Deleter>>(
		m, "Libmacro")
		.def_static("instance", []() {
			return py::cast(Libmacro::instance(),
				py::return_value_policy::reference);
		})
		.def_static("has_instance", &Libmacro::hasInstance)
		.def_property("blockable_flag",
			&Libmacro::blockableFlag,
			&Libmacro::setBlockableFlag)
		.def_property("generic_dispatch_flag",
			&Libmacro::genericDispatchFlag,
			&Libmacro::setGenericDispatchFlag)
		.def_property("modifiers",
			&Libmacro::modifiers,
			&Libmacro::setModifiers)
		.def_property("enabled",
			&Libmacro::enabled,
			&Libmacro::setEnabled)
		.def_property("global_thread_limit",
			&Libmacro::globalThreadLimit,
			&Libmacro::setGlobalThreadLimit)
		.def_property("generic_dispatcher",
			&Libmacro::genericDispatcher,
			&Libmacro::setGenericDispatcher,
			py::return_value_policy::reference_internal)
		.def("serial",
			py::overload_cast<>(&Libmacro::serial),
			py::return_value_policy::reference_internal)
		.def("macro_registry",
			py::overload_cast<>(&Libmacro::macroRegistry),
			py::return_value_policy::reference_internal)
		.def("signal_registry",
			py::overload_cast<>(&Libmacro::signalRegistry),
			py::return_value_policy::reference_internal)
		.def("trigger_registry",
			py::overload_cast<>(&Libmacro::triggerRegistry),
			py::return_value_policy::reference_internal);

	/* ── Factory functions ───────────────────────── */
	m.def("create_context", &factory::createContext,
		py::arg("enabled") = true);
	m.def("create_context_shared", &factory::createContextShared,
		py::arg("enabled") = true);
	m.def("create_macro",
		[](Libmacro *context) {
			return factory::createMacro(context);
		},
		py::arg("context") = nullptr);
	m.def("create_macro_shared",
		[](Libmacro *context) {
			return factory::createMacroShared(context);
		},
		py::arg("context") = nullptr);
}
