# Libmacro Architecture

## Lookup System — Name↔Value Serialization

`ISerial` (`mcr/serial.h`) provides string-to-enum and enum-to-string
conversions for modifiers, trigger modes, apply values, dimensions, and
interrupts. It is implemented by `SerialImpl` (`src/serial.cpp`).

### Built-in lookups

Each category has a `static const NameValue` array at file scope acting as
the authoritative built-in mapping. Value-to-name lookups use `switch`
statements on the enum values. These are compile-time constants — no runtime
initialization overhead.

### User extensibility

Consumers can register custom modifier and trigger-mode entries at runtime:

| Method | Direction | Scope |
|--------|-----------|-------|
| `setModifierName(name, value)` | name → value | modifier |
| `setModifierValueName(value, name)` | value → name | modifier |
| `removeModifierName(name)` | remove entry | modifier |
| `setTriggerModeName(name, value)` | name → value | trigger mode |
| `setTriggerModeValueName(value, name)` | value → name | trigger mode |
| `removeTriggerModeName(name)` | remove entry | trigger mode |

**Lookup priority** (both directions):
1. Extension map (user-registered, highest priority)
2. Primary name table (built-in, e.g. `modifier_names[]`)
3. Alias table (built-in, e.g. `modifier_aliases[]`, modifiers only)
4. Default return value (`MCR_MF_ANY` / `-1` / `nullptr`)

Value→name lookups check the extension map first, then the compiled `switch`
statement, then `nullptr`.

### Example — Custom modifier alias

```cpp
auto ctx = mcr::factory::createContext(false);
auto &serial = ctx->serial();

serial.setModifierName("JoyButton1", MCR_MF_USER);
serial.setModifierValueName(MCR_MF_USER, "Joystick Button 1");

mcr_ModFlags val = serial.modifiers("JoyButton1");   // MCR_MF_USER
const char *name = serial.modifiersName(MCR_MF_USER); // "Joystick Button 1"
```

## Extensibility Pattern

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
