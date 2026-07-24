# Libmacro Architecture

## Overview

Libmacro is organized around a central context (`Libmacro`) that owns all
sub-components: registries, a dispatcher, and a serialization interface.
Signals flow through the dispatcher to receivers (triggers, macros), which
may send new signals in response.

## Class hierarchy

```
IActor                 (mcr/trigger.h)     -- performs an action
  IMacro               (mcr/macro.h)       -- thread-like executor

IReceive               (mcr/dispatcher.h)  -- receives dispatched signals
  Trigger              (mcr/trigger.h)     -- conditionally invokes an IActor
  TriggerList          (mcr/trigger.h)     -- list of Triggers
  IMacro               (mcr/macro.h)       -- also receives signals

IDispatcher            (mcr/dispatcher.h)  -- blocking signal dispatcher
  Dispatcher           (src/dispatcher.cpp) -- generic implementation

Signal                 (mcr/signal.h)      -- unit of communication
SignalList             (mcr/signal.h)      -- list of Signals

Libmacro               (mcr/libmacro.h)    -- context / entry point
  LibmacroImpl         (src/libmacro.cpp)   -- concrete implementation

ISerial                (mcr/serial.h)       -- name<->value serialization
ISignalRegistry        (mcr/signal_registry.h)
ITriggerRegistry       (mcr/trigger_registry.h)
IMacroRegistry         (mcr/macro_registry.h)
```

## Signal dispatch pipeline

The dispatch pipeline is the core flow of Libmacro:

```
  Signal
    |
    v
  IDispatcher::dispatch(signal, mods)
    |
    +---> iterate receivers (IReceive set)
    |       |
    |       +---> IReceive::receive(signal, mods)
    |               |
    |               +---> returns true  => BLOCK, stop dispatching
    |               +---> returns false => CONTINUE to next receiver
    |
    +---> if not blocked, IDispatcher::modifier(signal, &mods)
    |       updates modifier state
    |
    +---> return: true if blocked, false if allowed through
```

### Dispatcher implementation

The concrete `Dispatcher` class (`src/dispatcher.cpp`) uses an
`std::unordered_set<IReceive *>` to store receivers. Dispatch iterates
the set and calls `receive()` on each. If any receiver returns `true`,
dispatch stops immediately (blocking).

### Generic vs. typed dispatch

- **Generic dispatcher**: `Libmacro::genericDispatcher()` receives all
  signal types. Controlled by `genericDispatchFlag`.
- **Signal-specific**: Each `Signal` has a `dispatcherPtr` field pointing
  to its registered dispatcher. Dispatch can be disabled per-signal via
  `dispatchFlag`.

### Dispatch registration

Macros register their activators and triggers with the dispatcher:

```cpp
macro.addDispatch();           // Register all activators + triggers
macro.removeDispatch();        // Unregister all
macro.applyDispatch();         // Re-register (safe to call repeatedly)
```

This connects signal receivers so that dispatched signals reach the macro's
triggers.

## Threading model

### Global thread management

A global mutex (`g_globalThreadMutex`) protects:

| Variable | Type | Purpose |
|----------|------|---------|
| `g_threadMax` | `unsigned int` | Global max threads (default 16) |
| `g_activeThreadCount` | `unsigned int` | Current active thread count |

`Libmacro::globalThreadLimit()` / `setGlobalThreadLimit()` control the cap.

### Per-macro threading

Each `MacroImpl` has:

| Member | Type | Purpose |
|--------|------|---------|
| `_threadCount` | `unsigned int` | Currently active threads |
| `_threadMax` | `unsigned int` | Per-macro thread limit (default 1) |
| `_threads` | `vector<thread>` | Running thread handles |
| `_queued` | `atomic<int>` | Lock-free queue counter |
| `_mutex` | `mutex` | Protects thread state |
| `_cndThreadCount` | `condition_variable` | Wait for thread changes |

**Execution flow**:

1. `start()` checks thread limits and launches a new `std::thread` running `run()`
2. `run()` sends all signals in the `_signals` list via `SignalList::send()`
3. Thread count is decremented when the thread completes

**Thread limiting**: `_queued` is `std::atomic<int>` for lock-free
increment/decrement. When `_queued > _threadMax`, it resets to 0 (safety
valve). The global thread limit is checked via `g_threadMax`.

### Interrupt mechanism

Macros support interruption via `IInterrupt::Value`:

| Value | Behavior |
|-------|----------|
| `DISABLE` | Macro is disabled |
| `CONTINUE` | Normal execution |
| `PAUSE` | Macro pauses (resumes after `MCR_MAX_PAUSE_COUNT` cycles) |
| `STOP` | Macro stops immediately |

### Sticky mode

When `sticky` is enabled, the macro retains its state across trigger events
rather than resetting on each dispatch.

## Memory ownership

- `LibmacroImpl` owns all sub-components via `std::unique_ptr`:
  - `_serial`, `_macroRegistry`, `_signalRegistry`, `_triggerRegistry`
  - `_genericDispatcherInstancePt`
- `MacroImpl` stores raw pointers to signals and triggers (non-owning)
- The `mcr::factory` namespace provides `createContext()`,
  `createMacro()`, etc. returning `unique_ptr` with custom deleters
- **Critical**: `Libmacro::setEnabled(false)` must be called before
  destruction to avoid threading errors

## Context registry

Multiple `Libmacro` instances can coexist. A global registry (protected by
`_registryMutex`) tracks all instances:

- `_registryStack` (`vector<Libmacro *>`): ordered by creation time
- `_registrySet` (`unordered_set<Libmacro *>`): fast lookup
- `Libmacro::instance()` returns the last created instance

## Lookup system -- name/value serialization

`ISerial` (`mcr/serial.h`) provides string-to-enum and enum-to-string
conversions for modifiers, trigger modes, apply values, dimensions, and
interrupts. It is implemented by `SerialImpl` (`src/serial.cpp`).

### Built-in lookups

Each category has a `static const NameValue` array at file scope acting as
the authoritative built-in mapping. Value-to-name lookups use `switch`
statements on the enum values. These are compile-time constants -- no runtime
initialization overhead.

### User extensibility

Consumers can register custom modifier and trigger-mode entries at runtime:

| Method | Direction | Scope |
|--------|-----------|-------|
| `setModifierName(name, value)` | name -> value | modifier |
| `setModifierValueName(value, name)` | value -> name | modifier |
| `removeModifierName(name)` | remove entry | modifier |
| `setTriggerModeName(name, value)` | name -> value | trigger mode |
| `setTriggerModeValueName(value, name)` | value -> name | trigger mode |
| `removeTriggerModeName(name)` | remove entry | trigger mode |

**Lookup priority** (both directions):
1. Extension map (user-registered, highest priority)
2. Primary name table (built-in, e.g. `modifier_names[]`)
3. Alias table (built-in, e.g. `modifier_aliases[]`, modifiers only)
4. Default return value (`MCR_MF_ANY` / `-1` / `nullptr`)

## Extensibility pattern

Libmacro exposes several `map()`-style extension interfaces backed by private
`std::unordered_map` or `std::map` storage:

| Interface | Storage | Key | Value |
|-----------|---------|-----|-------|
| `ISignalRegistry` | `unordered_map<string, SignalAllocator>` | signal name | create/destroy allocators |
| `ITriggerRegistry` | `unordered_map<string, TriggerAllocator>` | trigger name | create/destroy allocators |
| `IMacroRegistry` | `unordered_map<string, IMacro*>` | macro name | raw pointer |
| `ISerial` | `map<string, unsigned int, string_less_ci>` | modifier/trigger name | enum value |

All four follow the same contract: the interface declares virtual
`set`/`map`/`remove`/`unmap` methods, the implementation stores entries in a
private map, and the lookup paths consult the map before falling back to
built-in defaults.

## Platform abstraction

Platform-specific code lives in `src/<platform>/` directories. The
`MCR_PLATFORM` macro (set by CMake) selects the active platform. See
[platform.md](platform.md) for details on platform symbols and
[cmake_modules.md](cmake_modules.md) for how platform detection works.
