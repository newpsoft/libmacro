/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

/*! @file
 *  @brief @ref mcr::factory — Central factory for creating Libmacro objects.
 *
 *  Consolidates all create() / createShared() static methods that were
 *  formerly on each interface class into free functions in this namespace.
 */

#pragma once

#include <memory>
#include "mcr/macro.h"
#include "mcr/libmacro.h"
#include "mcr/serial.h"
#include "mcr/macro_registry.h"
#include "mcr/signal_registry.h"
#include "mcr/trigger_registry.h"
#include "mcr/dispatcher.h"

#ifdef __cplusplus

namespace mcr
{

class Libmacro;

/*! @brief Public factory functions for concrete objects of interfaces. */
namespace factory
{

/** @brief Create a new IMacro with default implementation.
 *  @param context Libmacro context (uses instance() if nullptr).
 *  @return Unique pointer to the new macro.
 */
MCR_API std::unique_ptr<IMacro, IMacro::Deleter>
createMacro(Libmacro *context = nullptr);
/** @brief Create a shared IMacro with default implementation.
 *  @param context Libmacro context (uses instance() if nullptr).
 *  @return Shared pointer to the new macro.
 */
MCR_API std::shared_ptr<IMacro> createMacroShared(Libmacro *context = nullptr);

/** @brief Create a new Libmacro context.
 *  @param enabled Whether the context starts enabled.
 *  @return Unique pointer to the new context.
 */
MCR_API std::unique_ptr<Libmacro, Libmacro::Deleter>
createContext(bool enabled = true);
/** @brief Create a shared Libmacro context.
 *  @param enabled Whether the context starts enabled.
 *  @return Shared pointer to the new context.
 */
MCR_API std::shared_ptr<Libmacro> createContextShared(bool enabled = true);

} /* namespace factory */

/*! @brief Used internally by this library. Not intended as public API. */
namespace internal
{
/*! @brief Internal-usage-only factory for creating sub-component interfaces.
 *  Not part of the public API. */
namespace factory
{

/** @brief Create a new ISerial implementation. */
MCR_API std::unique_ptr<ISerial, ISerial::Deleter> createSerial();
/** @brief Create a shared ISerial implementation. */
MCR_API std::shared_ptr<ISerial> createSerialShared();

/** @brief Create a new IMacroRegistry implementation. */
MCR_API std::unique_ptr<IMacroRegistry, IMacroRegistry::Deleter>
createMacroRegistry();
/** @brief Create a shared IMacroRegistry implementation. */
MCR_API std::shared_ptr<IMacroRegistry> createMacroRegistryShared();

/** @brief Create a new ISignalRegistry implementation. */
MCR_API std::unique_ptr<ISignalRegistry, ISignalRegistry::Deleter>
createSignalRegistry();
/** @brief Create a shared ISignalRegistry implementation. */
MCR_API std::shared_ptr<ISignalRegistry> createSignalRegistryShared();

/** @brief Create a new ITriggerRegistry implementation. */
MCR_API std::unique_ptr<ITriggerRegistry, ITriggerRegistry::Deleter>
createTriggerRegistry();
/** @brief Create a shared ITriggerRegistry implementation. */
MCR_API std::shared_ptr<ITriggerRegistry> createTriggerRegistryShared();

/** @brief Create a new generic IDispatcher.
 *  @param context Libmacro context that owns the dispatcher.
 */
MCR_API std::unique_ptr<IDispatcher, IDispatcher::Deleter>
createGenericDispatcher(Libmacro *context);
/** @brief Create a shared generic IDispatcher.
 *  @param context Libmacro context that owns the dispatcher.
 */
MCR_API std::shared_ptr<IDispatcher>
createGenericDispatcherShared(Libmacro *context);

} /* namespace factory */
} /* namespace internal */
} /* namespace mcr */

#endif /* __cplusplus */

