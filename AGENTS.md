# Libmacro — Agent Guide

## Build

```sh
cmake -B build -DBUILD_TESTING=ON -DTEST_TERMINAL=OFF .
cmake --build build
```

- Library target on Linux: `macro` (`libmacro.so`). On Windows: `libmacro` (`libmacro.dll`). Use `mcr::Libmacro` alias for portable linking.
- C standard: C17, C++ standard: C++17.
- On Linux, requires `Threads`, `Qt6Core`+`Qt6Test` (testing only).
- VSCode config sets `CMAKE_PREFIX_PATH=/usr/lib64/cmake/Qt6`.
- Version = `MCR_VER.GIT_REVISION` where GIT_REVISION = commit count on `master`.

## Tests

- **All tests**: `cmake --build build --target tst_libmacro && build/tst_libmacro`
- **Single test suite**: `cmake --build build --target tcreate && build/tcreate` (also `taction`, `tdispatcher`)
- `TEST_TERMINAL=OFF` skips tests needing a terminal (set in CI).
- Many tests are commented-out WIP code — verify before assuming test coverage.
- `TEST_INDIVIDUAL` macro: defined for standalone test builds, uses `QTEST_GUILESS_MAIN`.

## Style

- clang-format KNF (linux style) via `scripts/style` — run from any directory.
- Config: .clang-format (force tabs, 80-char max, no backup).
- `#include` ordering: alphabetical within each group (Google style).

## Key architecture

- Public C API: `mcr/api.h`
- Public C++ API: `mcr/libmacro.h` — class `mcr::Libmacro`.
- Import header: `Libmacro` (project root, no extension) — `#include "mcr/libmacro.h"` for convenience compilation.
- Library alias: `mcr::Libmacro`. Singleton via `Libmacro::instance()`.
- Platform dispatch via `MCR_PLATFORM` (`linux`, `windows`, `apple`). Platform sources in `src/<platform>/`.
- `mcr::Libmacro` must be **disabled** (`setEnabled(false)`) before destruction to avoid threading errors.
- Coverage flags (`--coverage`) always on for GCC/Clang; `scripts/coverage.sh` for HTML reports.
- `mcr::internal` namespace holds implementation details used only inside the
  library (e.g., `mcr::internal::factory`). Not intended as public API.

## Python bindings

- **Build**: add `-DBUILD_PYTHON=ON` to cmake. Module is `_libmacro.cpython-*.so` in `build/python/`.
- **Import**: `import _libmacro as m` (single-file module, no subpackage).
- **Test**: `python3 python/test_bindings.py`

### Usage patterns

- **Singleton**: only one `mcr::Libmacro` instance at a time. Create via `ctx = m.create_context()`, access via `m.Libmacro.instance()`.
- **Disable before cleanup**: always call `lib.enabled = False` before the context goes out of scope to avoid threading errors. Clean up macros before the context.
- **Factory functions**:
  - `m.create_context()` — create and register the Libmacro singleton
  - `m.create_macro(context)` — create a macro bound to a context
- **Signal/Trigger `name()`**: exposed as a **method**, not a property: `sig.name()`, not `sig.name`.
- **set_signals / set_triggers**: Python lists of heap-allocated objects cannot form a contiguous C array without corrupting vtables. The Python wrapper stores `_held_signals` / `_held_triggers` as `py::list` attributes (requires `py::dynamic_attr()`) to prevent GC, clears the internal C++ list via `set*(nullptr, 0)`, and registers dispatch relationships directly. Example:
  ```python
  macro.set_signals([modifier_signal, noop_signal])
  macro.set_triggers([action_trigger])
  ```
- **ISerial methods**: accessed via `lib.serial()`, not on signal/trigger registries.
- **Flags helpers**: `m.flags_combine()` takes an **iterable** (list/tuple), not varargs: `m.flags_combine([m.ModFlags.ALT, m.ModFlags.CTRL])`.
- **Factory functions accept optional context**: `m.create_macro(context=lib)` (default `nullptr`).
- **No IMacro trampoline**: users create macros via `factory::createMacro()`, not by subclassing.
- **Cleanup ordering**: delete macros before the Libmacro context to avoid dangling pointers.

### Registered types

| Python name | C++ type | Notes |
|---|---|---|
| `Dimension` | `mcr::Dimension` | X=0, Y=1, Z=2, W=3 |
| `ApplyValue` | `mcr::ApplyValue` | SET, UNSET, BOTH, TOGGLE |
| `ModFlags` | `mcr::ModFlags` | ALT, CTRL, SHIFT, etc. (bit flags) |
| `TriggerMode` | `mcr::TriggerMode` | EQUAL, ALL, NONE, EXCLUSIVE, etc. |
| `InterruptValue` | `IInterrupt::Value` | CONTINUE, PAUSE, INTERRUPT, etc. |
| `SpacePosition` | `mcr::SpacePosition` | `.x`, `.y`, `.z`, `.w` properties |
| `Libmacro` | `mcr::Libmacro` | Singleton, via `instance()` |
| `Macro` | `mcr::IMacro` | Created via `create_macro()` |
| `ISerial` | `mcr::ISerial` | Name/value conversion registry |
| `IMacroRegistry` | `mcr::IMacroRegistry` | Macro lifecycle |
| `ISignalRegistry` | `mcr::ISignalRegistry` | Signal allocation |
| `ITriggerRegistry` | `mcr::ITriggerRegistry` | Trigger allocation |
| `Signal` / `Modifier` / `NoOp` / `Interrupt` / `FunctorSignal` | `mcr::Signal` subclasses | Use `set_signals()` on macros |
| `Trigger` / `Action` / `FunctorTrigger` | `mcr::Trigger` subclasses | Use `set_triggers()` on macros |

## Project state

- `SPEC.MD` file holds issues to be fixed.
- `TODO.md` file holds TODOs and other decisions to be made about issues or architecture.

