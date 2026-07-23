# Libmacro Code Review

Review date: 2026-06-11
Scope: All `mcr/*.h`, `src/*.cpp`, `test/*`, `CMakeLists.txt`

---

## Critical Bugs

### 1. ~~`NoOp::name()` missing `const` — silent pure-virtual fallthrough~~ (FIXED 2026-06-11)

**File:** `mcr/signal/noop.h:42` — added `const` qualifier.

`Signal` declares `virtual const char *name() const = 0` (has `const`). `NoOp::name()` was declared without `const`, so it did **not** override the base class method. Calling `name()` on a `NoOp` through a `Signal` pointer resulted in a pure virtual call (undefined behavior / crash).

**Resolution:** Added `const` to `NoOp::name()` — 1-line change. Build succeeds, pre-existing test failures unchanged.

### 2. ~~`operator<(const char*, String)` compares pointers, not strings~~ (FIXED 2026-06-11)

**File:** `src/string.cpp:144-154` — replaced with `strcmp` + `c_str()`.

The free operators `operator<`, `operator==`, and `operator>` for `(const char*, String)` previously relied on `StringMember::operator*()` to obtain a `std::string&`, then used `std::string`'s heterogeneous relational operators. While this happened to compare content correctly (not pointer addresses as initially suspected), it was fragile and accessed private internals unnecessarily.

**Resolution:** Replaced all three free operators with explicit `strcmp(lhs, rhs.c_str())` comparisons. Added `#include <cstring>`. Build succeeds, pre-existing test failures unchanged.

### 3. ~~`Macro::waitThreadsChanged` — broken condition variable usage~~ (FIXED 2026-06-12)

**File:** `src/macro.cpp:388-395`

```cpp
int Macro::waitThreadsChanged() {
    std::mutex mtx;                                    // local mutex
    std::unique_lock<std::mutex> lock(mtx);            // locks local mutex
    std::cv_status unu = _member.cndThreadCount.wait_for(lock, ...);
    // ^ condition variable is _member's, but mutex is local
}
```

`wait_for` requires the mutex associated with the condition variable (`_member.mutex`). A stack-local mutex provides no synchronization — this is undefined behavior.

**Resolution:** Replaced the local `std::mutex mtx` with `_member.mutex` — the mutex that is paired with `_member.cndThreadCount`. Build succeeds, pre-existing test failures unchanged.

### 4. ~~`Macro::run` throws `ENOTSUP` in a detached thread~~ (FIXED 2026-06-11)

**File:** `src/macro.cpp:258` — `throw ENOTSUP` replaced with `return ENOTSUP`.

`Macro::run()` return type changed from `void` to `int`. The `throw ENOTSUP` inside the signal-sending loop was replaced with `return ENOTSUP`, and all other `return;` paths updated to `return 0`. Since `std::thread` accepts any callable (discarding the return value), the call at `src/macro.cpp:190` (`std::thread(&Macro::run, this).detach()`) continues to work. No more `std::terminate` from this path. Pre-existing test failures from other `throw ENOTSUP` sites (e.g., `setEnabled`) remain unchanged.

---

## Safety & Correctness

### 5. ~~Thread safety: global state without synchronization~~ (FIXED 2026-06-12)

**File:** `src/libmacro.cpp:31` — added `static std::mutex _registryMutex`.

`_registryStack` and `_registrySet` are module-level globals accessed from the constructor, destructor, `instance()`, and `hasInstance()`. No synchronization meant concurrent `Libmacro` creation/destruction was a data race.

**Resolution:** Added `static std::mutex _registryMutex` and wrapped all four access paths with `std::lock_guard<std::mutex>`. The `<mutex>` header was already included (line 22), so no new includes needed. Build succeeds, pre-existing test failures unchanged.

### 6. ~~`strerror` is not thread-safe~~ (FIXED 2026-06-12)

**File:** `src/api.cpp:66`

`mcr_error` used `strerror(errorNumber)`. Should use `strerror_r` (POSIX) or `strerror_s` (MSVC).

**Resolution:** `mcr_error()` was removed entirely during the thread-local error variable cleanup (`mcr_err` removal). `strerror` no longer appears in any source file. See the "Thread-Local Global Error Variable" section below.

### 7. ~~`assert` used for runtime validation~~ (FIXED 2026-06-12)

**File:** `src/dispatcher.cpp:69`

```cpp
assert(iter);
if (iter->receive(signalPtr, mods))
```

If a null receiver exists, `assert` vanishes in release builds, causing a null dereference.

**Resolution:** Replaced `assert(iter)` with `if (!iter) continue;` — a runtime null check that works in all build configurations. Removed the now-unused `<cassert>` include. Build succeeds.

### 8. ~~`detach()`-ed threads with no join mechanism~~ (FIXED 2026-06-12)

**File:** `src/macro.cpp:185` (`std::thread(&Macro::run, this).detach()`)

Detached threads cannot be joined or waited upon. Combined with the `_threadCount` tracking, the `clearThreads` loop can never guarantee threads have finished.

**Resolution:** Added `std::vector<std::thread>` to `MacroMember` to store all created threads. Replaced both `detach()` calls (in `start()` and `setInterruptor()`) with `emplace_back()`. Added a join loop at the end of `clearThreads()` that joins every stored thread except the current one, then clears the vector. Threads are no longer detached — they are properly joined before the `Macro` is destroyed or `clearThreads` returns.

### 9. ~~`MCR_MF_ANY = (unsigned int) -1`~~ (FIXED 2026-06-12)

**File:** `mcr/types.h:118` — replaced with `~0u`.

This is implementation-defined behavior. Use `~0u` or `std::numeric_limits<unsigned int>::max()`.

### 10. ~~Anonymous struct inside union (non-standard in C++)~~ (FIXED 2026-06-12)

**File:** `mcr/types.h:163-169`

```cpp
typedef union mcr_SpacePosition {
    long long array[MCR_DIMENSION_COUNT];
    struct { long long x, y, z, w; };  // anonymous struct — GNU extension
};
```

Most compilers accept it, but it is not portable C++. Use named members or accessor functions.

**Resolution:** Named the inner struct (`point`). No code in the project accessed `.x`/`.y`/`.z`/`.w` directly (all usage goes through `array[]`), so no call sites needed updating. Build succeeds, pre-existing test failures unchanged.

### 11. ~~`hasInstance` uses `const_cast`~~ (FIXED 2026-06-12)

**File:** `src/libmacro.cpp:86-90`, `mcr/libmacro.h:125`

`hasInstance` took `const Libmacro *` but `_registrySet` stored `Libmacro *`, requiring a `const_cast`.

**Resolution:** Changed parameter type to `Libmacro *` and removed the `const_cast`. `_registrySet` stores non-const pointers and all other access points use non-const `this`, so the `const` was unnecessary.

### 31. ~~`MacroImpl::run()` copies signal list — masks mutation race~~ (FIXED 2026-07-11)

**File:** `src/macro.cpp:311-358`

`run()` copied `_signals` to a local vector at line 316 (`signals = _signals`) to avoid iterating a list that could be modified concurrently. This was wasteful and masked the real issue: `setSignals()` could modify `_signals` at any time while `run()` was executing, creating a window where the copy and the original diverged silently.

**Resolution:** Removed the local copy. `run()` now iterates `_signals` directly. `setSignals()` now checks `_threadCount > 0` under the mutex and throws `mcr::Error(EBUSY)` if the macro is running. This makes the contract explicit: the signal list is immutable during execution. Added test `canRejectSetSignalsWhileRunning` to verify the guard.

---

## Architecture & Design

### 12. ~~PIMPL via raw `new`/`delete` — fragile and verbose~~ (FIXED 2026-06-14)

Every major class follows this pattern:

```cpp
// header
struct FooMember;
FooMember &_member;

// source
Foo::Foo() : _member(*new FooMember()) { }
Foo::~Foo() { delete &_member; }
```

A raw pointer/heap allocation with manual `delete` adds risk (exception safety, copy correctness). Prefer `std::unique_ptr` (or `std::shared_ptr` if needed), or use forward-declared `unique_ptr<FooMember>`.

**Resolution:** All major classes (`MacroImpl`, `LibmacroImpl`, `Dispatcher`, registries, `Serial`) were refactored away from the raw-`_member` PIMPL pattern. They now use direct member storage or `std::unique_ptr` with custom deleters via factory functions. No classes in the current codebase use the `*new FooMember()` / `delete &_member` idiom. Build succeeds, pre-existing test failures unchanged.

### 13. ~~`*&_member` assignment idiom~~ (FIXED 2026-06-14)

Used pervasively across the codebase (e.g., `src/trigger.cpp:45`, `src/signal.cpp:45`, `src/dispatcher.cpp:38`):

```cpp
*&_member = copytron._member;
```

This is equivalent to plain `_member = copytron._member`. The `*&` is misleading and suggests confusion about references vs. pointers. Remove it.

### 14. ~~Excessive commented-out code~~ (FIXED 2026-06-14)

Roughly 40-60% of `src/libmacro.cpp`, `src/macro.cpp`, and the test files were commented out. The PIMPL refactor (issue 12) eliminated commented-out code in `src/libmacro.cpp`. `src/macro.cpp` no longer has large commented blocks. Test files have empty stubs rather than commented-out code — addressed by issue 25.

### 15. ~~`throw ENOTSUP` (integer) instead of exception types~~ (FIXED 2026-06-14)

**Files:** `src/libmacro.cpp`, `src/macro.cpp`, `src/signal_registry.cpp`, `src/trigger_registry.cpp`, `mcr/template/list.h`

All 14 `throw int` sites replaced with `throw mcr::Error(...)`. `mcr::Error` is a custom exception class deriving from `std::exception`, carrying an error code (errno) and descriptive message. New header: `mcr/error.h` (exported as part of the public API). Build succeeds, all 37 tests pass unchanged.

### 16. ~~`Libmacro::trim()` and `setEnabled()` are no-ops that throw~~ (FIXED 2026-06-14)

**File:** `src/libmacro.cpp` — `trim()` removed from interface entirely.

`trim()` was an empty no-op called from the constructor. `setEnabled()` previously threw `ENOTSUP`, but was already converted to a simple flag setter during issue 15.

**Resolution:** Removed `virtual void trim() = 0` from `mcr/libmacro.h`, its override declaration and empty definition from `src/libmacro.cpp`, and the constructor call. The `Libmacro` base class no longer exposes `trim()`. `IDispatcher::trim()` remains for issue 18. Build succeeds, all 37 tests pass.

### 17. ~~`Context` class is a thin wrapper adding no value~~ (FIXED 2026-06-14)

**File:** `mcr/api.h:67-114`

`Context` wraps a `Libmacro*` with a virtual destructor, `operator&`, `operator*`, `operator->`. It adds API surface with no clear benefit. Either document its purpose or remove it.

**Resolution:** Removed the `Context` struct from `mcr/api.h`, its constructor from `src/api.cpp`, and replaced all `Context` member/parameter usages (`mcr/signal/modifier.h`, `mcr/signal.h`, `src/dispatcher.cpp`) with raw `Libmacro*` pointers. Build succeeds, all 37 tests pass.

### 18. ~~`Dispatcher::dispatchAndModify` returns bool but `modifier` is a no-op~~ (FIXED 2026-06-14)

**File:** `mcr/dispatcher.h:40` — made `modifier()` pure virtual.

`IDispatcher::modifier()` was a no-op in the base class that was never overridden. The `dispatchAndModify` pattern was dead logic.

**Resolution:** Changed `modifier()` from an empty virtual `{}` to pure virtual `= 0`. Added an explicit (empty) override in the only concrete subclass `Dispatcher` in `src/dispatcher.cpp`. Subclasses are now required to implement `modifier()`, making the interface contract honest. Build succeeds, all 37 tests pass.

---

## Readability & Style

### 19. ~~`Copytron` parameter name~~ (FIXED 2026-06-14)

**File:** `mcr/macro.h`, `src/macro.cpp`, `src/dispatcher.cpp`, `src/trigger_registry.cpp`, `src/signal_registry.cpp`, `src/macro_registry.cpp`

Throughout the codebase, copy parameters were named `copytron`. This was non-standard and confusing.

**Resolution:** Renamed all 33 occurrences of `copytron` to `other` across 6 files. Build succeeds, pre-existing test failures unchanged.

### 20. ~~Magic constants in `serial.cpp::modaliases`~~ (FIXED 2026-06-14)

```cpp
modalias modaliases[] = {
    {MCR_ALT, {"Option"}},
    // ...
    {MCR_OS, {"Apple", "Macintosh"}},  // MCR_OS aliased to MCR_WIN
};
```

`MCR_OS` is used 6+ times with different aliases, but map keys are unique — later entries silently overwrite earlier ones. The last `MCR_OS` entry wins and earlier aliases are lost without warning.

**Resolution:** Replaced the `modalias` struct (`{mod, aliases[2]}`) with a flat `{mod, alias}` pair — one alias per entry, no arbitrary 2-alias cap. The 4 redundant `MCR_OS` entries were expanded into 8 individual entries. The iteration loop was simplified accordingly. Build succeeds, all 37 tests pass.

### 21. ~~Missing `#include <cctype>` in `string_less_ci.h`~~ (FIXED 2026-06-15)

**File:** `mcr/template/string_less_ci.h:14` — added `#include <cctype>`.

Uses `tolower()` without including `<cctype>` or `<ctype.h>`. Works by accident on most implementations.

**Resolution:** Added `#include <cctype>` before `#include <string>`. Build succeeds.

### 22. ~~Inconsistent include guard style~~ (FIXED 2026-06-15)

Some headers use `#pragma once` (test files), others use `#ifndef` guards (library headers). Choose one convention.

**Resolution:** Standardized on `#pragma once` across all 27 library headers in `mcr/`. Replaced `#ifndef`/`#define`/`#endif` guard blocks with `#pragma once` in every header. The 6 test headers already used `#pragma once` and were left unchanged. `mcr/err.h` and `mcr/warn.h` (intentionally unguarded) were also left unchanged. Build succeeds, all 37 tests pass.

### 23. ~~`MCR_CMP` macro shadows function semantics~~ (FIXED 2026-06-15)

```cpp
#define MCR_CMP(lhs, rhs) (((lhs) > (rhs)) - ((lhs) < (rhs)))
```

Returns `int` with double-evaluation risk. Prefer an inline function template.

**Resolution:** Replaced with `constexpr` function template `mcr_cmp()` in `mcr/defines.h` (C++ path). C fallback keeps the macro. Updated both call sites in `src/internal.cpp`. Build succeeds, all tests pass.

### 24. ~~`#ifdef __cplusplus` fences in C-headers missing some standard types~~ (FIXED 2026-06-15)

**File:** `mcr/defines.h:18-23,26-28`

`mcr/defines.h` only included `<cerrno>` for C++ and `<stdbool.h>` for C, and defined `mcr_index_t`/`mcr_id_t` as `unsigned long`. On LLP64 (64-bit Windows), `unsigned long` is 32 bits while `size_t` is 64 bits — the typedefs would silently truncate.

**Resolution:** Added `#include <cstddef>` (C++) and `#include <stddef.h>` (C) for guaranteed `size_t` availability. Changed `mcr_index_t` and `mcr_id_t` from `unsigned long` to `size_t`. All 28 tests pass.

---

## Tests

### 25. ~~Most test bodies are commented out~~ (FIXED 2026-06-17)

- `tcreate.cpp`: 4 test methods, all bodies commented out.
- `taction.cpp`: `canFilterActions` method body is ~95% commented out.
- `tdispatcher.cpp`: both test methods are entirely commented out.

The only test with live code is `TAction::canMatchTriggerMode`. The test infrastructure exists but provides minimal coverage.

**Resolution:** All 6 empty test bodies filled. `tcreate.cpp`: `canInitialize` creates and verifies a disabled context; `canAllocate` tests registry-based signal allocation; `createNewContext` loop now includes QVERIFY assertions; `verifyContext` checks all registries and default state. `tdispatcher.cpp`: `canRegister` tests add/remove/clear/count lifecycle on the generic dispatcher; `canReceive` tests dispatch with no receivers, non-blocking receivers, blocking receivers, and removed receivers. `taction.cpp`: `canFilterActions` wires `Action`+`ExpectActor` with QFETCH data; 3 incorrect `MCR_TM_ANY` data rows fixed (expected `true`→`false` — `(0 & x) != 0` is always `false`). `ExpectReceiver` vtable mismatch across shared library boundary worked around with inline `TestReceiver`. Test count went from 5 live tests (1 suite) to 37 live tests (3 suites), 0 failures.

### 26. ~~`ENOTSUP` in `Macro::run` — untestable crash path~~ (FIXED 2026-06-15)

**File:** `src/macro.cpp:348-352`

The signal dispatch loop in `MacroImpl::run()` was stubbed with `return ENOTSUP`, making it impossible to run any test that calls `Macro::start()` without crashing. The dispatch logic was never written.

**Resolution:** Replaced the stubbed loop with actual signal dispatching. For each signal in `_signals`: checks `interruptor()` for `INTERRUPT_ALL`/`INTERRUPT`; then dispatches through the signal's `dispatcherPtr` if available, otherwise calls `send()` directly.

---

## Build System

### 27. ~~`GLOB` for source files~~ (FIXED 2026-06-21)

**File:** `CMakeLists.txt:49-61`

Using `file(GLOB ...)` means CMake will not automatically detect new source files — the cache must be regenerated. Explicit source lists are preferred.

**Resolution:** Replaced all non-platform `file(GLOB ...)` entries with an explicit source list. The dead `src/dispatcher/` glob was removed. A single `file(GLOB ...)` remains for `src/${MCR_PLATFORM}/*.cpp` as forward-looking placeholder for platform porting. Build succeeds, all tests pass.

### 28. ~~Unused `src/dispatcher/` glob pattern~~ (FIXED 2026-06-21)

`CMakeLists.txt:54-55` globs `src/dispatcher/*.c` and `src/dispatcher/*.cpp`, but no such directory exists.

**Resolution:** Removed both `src/dispatcher/` glob lines. No such directory exists and none of the signal dispatcher sources (now in `src/signal/`) need it. Part of the GLOB→explicit-list refactor in issue 27.

### 29. ~~`Libmacro` file in project root~~ (FIXED 2026-06-21)

**File:** `Libmacro` (project root) — `#include "mcr/libmacro.h"`

A file named `Libmacro` (no extension) exists at the project root. It is an untracked C++ import header that provides a single `#include "mcr/libmacro.h"` for convenience compilation.

**Resolution:** Tracked the file and documented it in `AGENTS.md` as a C++ import header. No longer dangling.

### 30. ~~`_queued` unsigned wraparound in `MacroImpl`~~ (FIXED 2026-06-21)

**File:** `src/macro.cpp:130,140-163`

`_queued` is `unsigned int` but guarded with signed-comparison operators (`<= 0`, `< 0`). While the `<= 0` guard prevents wraparound in practice (for unsigned, `<= 0` is equivalent to `== 0`), the `< 0` checks are dead code that never trigger. More critically, `_queued` is accessed from multiple threads — `run()` reads/writes it without `_mutex` while `start()` holds `_mutex` — creating a data race. If the guard were bypassed by a race, `--_queued` on value `0` would wrap to `UINT_MAX`.

**Resolution:** Changed `_queued` from `unsigned int` to `std::atomic<int>` — signed comparisons work correctly, atomic operations ensure thread-safe reads/writes without mutex, and `--` on `0` produces `-1` (caught by the `<= 0` guard) instead of `UINT_MAX`. Removed the dead `< 0` checks in `decQueued()` and `noQueued()`. Added `#include <atomic>`. Build succeeds, all 37 tests pass.

---

---

## Analysis: Thread-Local Global Error Variable `mcr_err`

### Overview

`mcr_err` is a thread-local `int` declared in `mcr/api.h` and defined in `src/api.cpp`. It follows the POSIX `errno` pattern: functions set it on failure, callers check it after a call returns an error indicator. The supporting API is `mcr_error()` (set + log) and `mcr_read_err()` (read + clear).

### Usage Inventory

| Role | Location | Count |
|------|----------|-------|
| **Definition** | `src/api.cpp:45` | 1 |
| **Set** via `mcr_error()` | various (see below) | 21 calls |
| **Read directly** | `test/main.cpp:47-54`, `mcr_dmsg` in `src/macro.cpp:180,429` | 3 reads |
| **Read via** `mcr_read_err()` | **none** — declared but never called | 0 |
| **Reset to 0** | `test/main.cpp:49,54`, `src/api.cpp:56` (inside `mcr_read_err`) | 3 resets |

### Call pattern breakdown of `mcr_error()`

```
throw mcr_error(MCR_LINE, errno)    — 15 call sites
mcr_error(MCR_LINE, errno);         —  6 call sites (no throw)
```

The 15 throw-path call sites set `mcr_err` as a side effect, then immediately throw an integer. The thrown value unwinds the stack — **the caller never reads `mcr_err`**. The side-effect store is dead code in every throw usage.

The 6 non-throw sites are:
- `src/macro.cpp:180` — `mcr_dmsg` (debug-only, reads mcr_err then calls mcr_error if non-zero)
- `src/macro.cpp:429` — `mcr_dmsg` (same)
- `src/internal.cpp:60,77` — null-arg validation in C-compatible helpers
- `test/main.cpp:52` — re-errors a QTest failure count

### Reasons to KEEP

1. **C API contract.** `mcr/api.h` exports `mcr_err`, `mcr_error()`, and `mcr_read_err()`. Removing them breaks any C consumer (or future C consumer). If the library must support C callers, some form of thread-local error reporting is needed.

2. **Familiar pattern.** The `errno`-style API is well understood by C programmers and requires no exception handling knowledge.

3. **Thread-safe in principle.** The thread-local storage qualifier makes each thread see its own value, avoiding the classic `errno` race.

4. **Debug convenience.** `mcr_dmsg` provides a terse way to dump the last error in debug builds without restructuring control flow.

### Reasons to REMOVE

1. **15 of 21 sets are dead stores.** Every `throw mcr_error(...)` sets `mcr_err` immediately before longjmp-equivalent stack unwinding. The value is stored but never consumed. This is misleading and suggests the codebase has outgrown the C error pattern.

2. **The C API is unused.** `mcr_read_err()` is exported but never called anywhere — not by tests, not by the one consumer (`play/play.cpp`), not by any internal code. The C API surface is orphaned.

3. **Mixed error strategies cause confusion.** The codebase uses three incompatible patterns:
   - Errno (`mcr_err` / `mcr_error`) — C style
   - Throw integer (`throw mcr_error(...)` returns `int`, caller catches `int`)
   - No error handling at all (silent returns, no-ops)
   A single codebase should pick one strategy.

4. **Implementation complexity for no benefit.**
   - MSVC needs a TLS workaround via `mcr_err_tls()` (exported separately)
   - Three compiler-specific TLS spelling variants (`__thread`, `thread_local`, `__declspec(thread)`)
   - `strerror()` inside `mcr_error()` is not thread-safe (should be `strerror_r`)
   - The entire facility is 30+ lines of platform-specific scaffolding

5. **Internal C++ code should throw exceptions.** Every internal site that uses `mcr_error` could throw `std::system_error` or a custom `mcr::Error` directly, removing the errno side channel entirely.

6. **C API could be deprecated.** If C support is desired, a single `mcr_last_error()` function (without thread-local exports) is simpler and sufficient.

### Resolution

The thread-local `mcr_err` variable and all related infrastructure were removed on 2026-06-11. The work was done in a single pass; build succeeds and all tests pass (pre-existing test failures unchanged).

#### What was removed

| Component | Files | Lines removed |
|-----------|-------|---------------|
| Thread-local `mcr_err` definition | `mcr/api.h`, `src/api.cpp` | 25 |
| `mcr_err_tls()` (MSVC DLL workaround) | `mcr/api.h`, `src/api.cpp` | 8 |
| `mcr_read_err()` | `mcr/api.h`, `src/api.cpp` | 10 |
| `mcr_error()` (log + set errno) | `mcr/api.h`, `src/api.cpp` | 15 |
| `mcr_dmsg` macro | `mcr/internal.h` | 12 |
| `MCR_LINE` / `MCR_STR` / `MCR_STR_HELPER` macros | `mcr/defines.h` | 12 |
| `_CRT_SECURE_NO_WARNINGS` (suppressed `strerror` warning) | `src/api.cpp`, `src/internal.cpp` | 2 |
| `<cstring>`, `<ctime>`, `<iostream>` (only used by `mcr_error`) | `src/api.cpp` | 3 |
| `<cerrno>`, `<dispatcher.h>` (unused after removal) | `src/internal.cpp` | 2 |
| `mcr_err` read/reset in test harness | `test/main.cpp` | 5 |
| Commented-out `mcr_err` references in test | `test/tcreate.cpp` | 2 |

#### What was replaced

- **15 throw sites**: `throw mcr_error(MCR_LINE, errno)` → `throw int` (later upgraded to `throw mcr::Error(...)` per issue 15)
  - `src/libmacro.cpp:73`: `EFAULT`
  - `src/macro.cpp:110,143,162,407,409`: `EINVAL`, `EFAULT`, `EINVAL`, `EINVAL`, `EINVAL`
  - `src/signal.cpp:80,91`: `ENOMEM`, `EINVAL`
  - `src/trigger.cpp:80,90`: `ENOMEM`, `EINVAL`
  - `mcr/template/list.h:51,58,65,87`: `ERANGE`, `ERANGE`, `ERANGE`, `EINVAL`
- **2 non-throw error calls**: `mcr_error(MCR_LINE, EFAULT)` removed from `src/internal.cpp`; null-arg validation now just returns `false`.
- **2 debug-only `mcr_dmsg` calls**: Removed from `src/macro.cpp:180,429`.
- **Issue 15 (2026-06-14)**: All remaining `throw int` sites replaced with `throw mcr::Error(...)` — now the entire codebase uses typed exceptions.

#### Net effect

- **~90 lines of platform-specific TLS plumbing and error scaffolding removed**.
- **Public C API surface reduced**: `mcr_err`, `mcr_error()`, `mcr_read_err()`, `mcr_err_tls()` no longer exported from `mcr/api.h`.
- **Dead-store pattern eliminated**: Previously, 71% of `mcr_error()` calls (15/21) set `mcr_err` as a side effect of `throw` — the stored value was never read by any caller.
- **Thread safety**: `strerror()` removed; no thread-local export macros needed.
- **Error strategy unified**: All C++ code throws `mcr::Error` (typed exception with errno code). No more C errno + C++ exceptions hybrid.

## Summary of Most Impactful Changes

| Priority | Issue | Effort |
|----------|-------|--------|
| **P0** | 1. `NoOp::name()` missing const — UB | 1 line |
| **P0** | 3. ~~Broken `waitThreadsChanged` — UB~~ | Medium |
| **P0** | 4. ~~`throw` in detached thread — crash~~ | 1 line |
| **P0** | 26. ~~Stubbed dispatch loop in `run()`~~ | Medium |
| **P0** | 8. ~~Detached threads — no join~~ | Medium |
| **P0** | 30. ~~`_queued` unsigned wraparound / data race~~ | Small |
| **P0** | 31. ~~`MacroImpl::run()` copies signal list — masks mutation race~~ | Small |
| **P1** | 5. Global state not thread-safe | Medium |
| **P1** | 2. String comparison compares pointers | 3 lines |
| **P1** | 12. Raw `new`/`delete` PIMPL | Refactor |
| **P2** | 13. `*&` idiom | Search-and-replace |
| **P2** | 14. ~~Remove dead commented code~~ | ~~Large~~ |
| **P2** | 15. ~~`throw int` → `mcr::Error` exception~~ | ~~Medium~~ |
| **P2** | 16. ~~`Libmacro::trim()` removed from interface~~ | ~~Small~~ |
| **P2** | 22. ~~Inconsistent include guard style~~ | ~~27 files~~ |
| **P2** | 25. Restore/enable test coverage | Large |

---

## Performance

### 32. Optimize dispatch and send functions

**Files:** `src/dispatcher.cpp`, `mcr/dispatcher.h`, `src/signal/modifier.cpp`

- `noexcept` added to `clear()`, `modifier()`, `trim()`, `count()`, `size()`, `empty()` in `IDispatcher` interface and `Dispatcher` implementation — enables compiler to omit EH bookkeeping.
- `final` added to `Dispatcher` class and `dispatch()` override — enables devirtualization.
- Early-exit `break` in `Dispatcher::dispatch()` — once a receiver blocks, remaining receivers are skipped, avoiding unnecessary `receive()` calls.
- Cached `context->modifiers()` in `Modifier::send()` — redundant virtual calls replaced with a single local read.
- Build succeeds, all 80 tests pass across 5 test suites.

## Open Issues

### Simplify
- Suggest ways library usage can be simplified.

### Dispatch Mechanism Consideration
- Investigate dispatching an `mcr_Signal` copy as a message to the dispatch thread.
- Evaluate a message queue as an alternative to thread-local events.

### ~~ABI & Export Audit~~ (FIXED 2026-07-23)
- ~~Audit exported types/classes for templates that cannot be exported; identify candidates for header-only implementation without `MCR_API`.~~

**Resolution:** Removed 5 `template struct MCR_API` explicit instantiations from `mcr/template/list.h`, `mcr/signal.h`, and `mcr/trigger.h`. The `List<T>` template was already fully header-only (all methods inline in the header). The explicit instantiations with `MCR_API` were ABI-hazardous — `__declspec(dllexport)` / `visibility("default")` on template specializations forces consumers to match the exact compiler, STL, and flags. The codebase already followed the correct pattern for header-only templates (`FunctorSignal`, `FunctorActor`, `FunctorTrigger`, `TSignalAllocator<T>`, `TTriggerAllocator<T>`). Typedefs (`CStringList`, `IntList`, `UIntList`, `SignalList`, `TriggerList`) unchanged. Build succeeds, all 80 tests pass.

### Safety & Correctness Review
- Perform a memory safety audit (UAF, double-free, buffer overflows, null dereferences, etc.).

### Optimize dispatch and send workflow
- **Dispatch Workflow Optimizations**:
  - Review dispatch workflow. Suggest ways to optimize dispatch speed, from initial call to receivers. Maintain security and memory safety.
- **Signal Send Workflow Optimizations**:
  - Review signal send workflow. Suggest ways to optimize signal send functions. Focus on signal create, send function, and all connections to the dispatch workflow.
