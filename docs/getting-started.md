# Getting Started

This tutorial walks through creating a Libmacro context, registering signal
and trigger types, and dispatching a signal.

## Prerequisites

- CMake 3.10+
- GCC 7+, Clang 5+, or MSVC 2017+
- Qt6Core and Qt6Test (for building tests only)

## Build the library

```sh
cmake -B build -DBUILD_TESTING=ON .
cmake --build build
```

## Basic usage

### 1. Create a context

The `mcr::Libmacro` class is the entry point. It owns all sub-components
(registries, dispatcher, serial).

```cpp
#include "mcr/libmacro.h"

int main()
{
    auto ctx = mcr::factory::createContext(true);
    // ... use ctx ...
    // Disabling before destruction avoids threading errors
    ctx->setEnabled(false);
    return 0;
}
```

Or on the stack:

```cpp
mcr::Libmacro ctx;
ctx.setEnabled(true);
// ... use ctx ...
ctx.setEnabled(false);
```

### 2. Register signal types

Signals are the basic unit of communication. Register built-in or custom
signal types:

```cpp
auto &sigReg = ctx.signalRegistry();
sigReg.map<mcr::Modifier>();  // Built-in modifier signal
```

### 3. Register trigger types

Triggers receive dispatched signals and invoke actions:

```cpp
auto &trigReg = ctx.triggerRegistry();
trigReg.map<mcr::Action>();  // Built-in action trigger
```

### 4. Create a macro

Macros are thread-like executors that send signals when triggered:

```cpp
mcr::Macro macro(&ctx);
macro.setName("myMacro");
macro.setEnabled(true);
```

### 5. Set activators and triggers

Activators are signals that can trigger the macro. Triggers define
conditions for activation:

```cpp
// Set activator signals
std::vector<mcr::Signal> activators;
// ... populate activators ...
macro.setActivators(activators);

// Set triggers
std::vector<mcr::Trigger> triggers;
// ... populate triggers ...
macro.setTriggers(triggers);
```

### 6. Dispatch a signal

Signals can be dispatched through the dispatcher, which notifies all
registered receivers:

```cpp
auto &dispatcher = *ctx.genericDispatcher();
mcr::Signal *signal = /* your signal */;
unsigned int mods = 0;

dispatcher.dispatch(signal, mods);
```

### 7. Send signals directly

Signals can also be sent directly without dispatch:

```cpp
signal->send();
```

## Using the factory

The `mcr::factory` namespace provides convenience functions:

```cpp
// Create a context
auto ctx = mcr::factory::createContext(true);

// Create a macro
auto macro = mcr::factory::createMacro(ctx.get());

// Create with shared ownership
auto sharedCtx = mcr::factory::createContextShared(true);
```

## Serialization

Map names to enum values for modifiers and trigger modes:

```cpp
auto &serial = ctx.serial();

// Register custom modifier name
serial.setModifierName("JoyButton1", MCR_MF_USER);
serial.setModifierValueName(MCR_MF_USER, "Joystick Button 1");

// Lookup
mcr_ModFlags val = serial.modifiers("JoyButton1");   // MCR_MF_USER
const char *name = serial.modifiersName(MCR_MF_USER); // "Joystick Button 1"
```

See [usage.md](usage.md) for more serialization examples.

## Next steps

- Read [architecture.md](architecture.md) to understand the dispatch pipeline
  and threading model
- See [platform.md](platform.md) for platform-specific details
- Check the public API headers in `mcr/` for full Doxygen documentation
