# Libmacro Usage

## Extending name-value mappings

`ISerial` manages string-to-enum and enum-to-string conversions for
modifiers and trigger modes. Consumers can register custom names at runtime.

### Accessing the serial interface

```cpp
#include "mcr/libmacro.h"

auto ctx = mcr::factory::createContext();
auto &serial = ctx->serial();
```

### Custom modifier names

Register a custom modifier name and its display name:

```cpp
serial.setModifierName("JoyButton1", MCR_MF_USER);
serial.setModifierValueName(MCR_MF_USER, "Joystick Button 1");

// Lookup by name
mcr_ModFlags val = serial.modifiers("JoyButton1"); // MCR_MF_USER

// Reverse lookup by value
const char *name = serial.modifiersName(MCR_MF_USER); // "Joystick Button 1"
```

Remove a previously registered name:

```cpp
serial.removeModifierName("JoyButton1");
serial.removeModifierName(nullptr); // no-op (safe)
```

### Custom trigger mode names

Same API shape for trigger modes:

```cpp
serial.setTriggerModeName("CustomMode", MCR_TM_USER);
serial.setTriggerModeValueName(MCR_TM_USER, "Custom Mode");

mcr_TriggerMode mode = serial.triggerMode("CustomMode"); // MCR_TM_USER
const char *label = serial.triggerModeName(MCR_TM_USER); // "Custom Mode"

serial.removeTriggerModeName("CustomMode");
```

### Lookup priority

User-registered entries take priority over built-in names in both
directions (name→value and value→name). Lookup order:

1. Extension map (highest priority)
2. Primary name table (built-in)
3. Alias table (built-in modifiers only, e.g. "Option" → `MCR_ALT`)
4. Default return value (`MCR_MF_ANY` for modifiers, `-1` for trigger modes)

### Other registries

The same `map()` / `set()` pattern is used by signal, trigger, and macro
registries:

```cpp
// Signal registry
auto &sigReg = ctx->signalRegistry();
sigReg.map<mcr::Modifier>();

// Trigger registry
auto &trigReg = ctx->triggerRegistry();
trigReg.map<mcr::Action>();

// Macro registry
auto &macroReg = ctx->macroRegistry();
macroReg.map("myMacro", &someMacro);
```
