## 🤖 AI Agent To-Do List (Derived from TODO.txt)

## 🚀 Core Implementation Tasks

- [x] **optimization**: inline dispatch functions (DONE 2026-07-13)
- [x] **compatibility**: Initialize C-maps and arrays with const static arrays (arrlen for length) (DONE 2026-07-13)
- [x] **Signal/Trigger Function Wrapping**: Implement wrapping instances for `Signal` and `Trigger` functions. (DONE 2026-07-13 — `mcr/signal/functor.h`, `mcr/actor/functor.h`, `mcr/trigger/functor.h`)
- [x] **Macro Class Flag**: Implement and manage the `applyDispatch` flag in the `Macro` class, allowing enablement without requiring actual dispatch reception. (DONE 2026-07-13)

## 💡 Architectural/Design Improvements

- [x] **Context Update Tracking**: Add a `bool updateFlag` to all mappings that set values within `mcr_context`. (OBSOLETE 2026-07-13 — `mcr_context` and `Context` class were removed per SPEC.md issue 17)
- [x] **Initialization Strategy**: Initialize C-maps and arrays using const static arrays (e.g., use `arrlen` for length determination). (DONE 2026-07-13)
- [x] **Dispatch Functions**: Consider the feasibility and implementation of inline dispatch functions. (DONE 2026-07-13 — `noexcept` on safe interface methods, `final` on `Dispatcher::dispatch()`, early-exit `break` in dispatch loop, cached `modifiers()` in `Modifier::send()`)

## 🧩 Plugins System Enhancement

(Moved to SPEC.md)

## 📝 Code Example Cleanup & Review

- [x] **Review C-only Example Dependencies**: Not applicable - no example code found in repository. (DONE 2026-07-13)

## 🧵 Thread Management Improvements (MacroImpl — src/macro.cpp)

- [x] **bug**: `_queued` is `unsigned int` but `decQueued()` checks `<= 0` followed by `< 0` — both always false for unsigned, causing wraparound to `UINT_MAX`. Changed to `std::atomic<int>` with correct guard checks. (FIXED 2026-06-21)
- [x] **bug**: `setInterruptor()` spawns a thread as side effect (`_threads.emplace_back(&MacroImpl::resetThread, this)`) — surprising and unsafe under mutex. (FIXED 2026-07-04)
- [x] **stub**: Signal sending in `run()` loop at line 362 is `return ENOTSUP` — the dispatch loop was never finished. (FIXED per SPEC.md #26)
- [x] **perf**: `clearThreads()` uses `wait_for(2s)` instead of `wait()` — always blocks 2s per join. Replace with condition-based immediate wake. (FIXED — uses `wait()` with predicate)
- [x] **perf**: `pauseLoop()` polls on `sleep_for(1s)` up to 5 iterations. Use condition variable to wake on interruptor change. (FIXED 2026-07-04)
- [x] **scalability**: Per-macro thread pool (`vector<thread>` per `MacroImpl`) doesn't scale — 100 macros × 16 threads = 1600 threads. Consider a shared executor. (FIXED 2026-07-13)
- [x] **safety**: No RAII scope guard for thread handles — leaks threads on exception between `emplace_back` and `join`. (FIXED — `run()` catches exceptions, destructor joins all threads)
- [x] **safety**: `~MacroImpl()` calls `disableEverything()` which joins threads — UB if invoked from a thread also in `_threads`. (FIXED 2026-07-13)
- [x] **cleanup**: Dead code in `run()` — `static_cast<void>(signals)` before `for` loop with empty body. (FIXED — dispatch loop is fully implemented)

## 🔧 Refactoring — Reduce Nesting

- [x] **Extract signal dispatch for-loop from `run()`** — extracted to `dispatchSignals()` (DONE 2026-07-13)
- [x] **Extract activator-trigger pairing from `_addDispatch()`** — extracted to `pairActivatorsAndTriggers()` (DONE 2026-07-13)
- [x] **Extract `INTERRUPT_ALL` handling from `setInterruptor()`** — extracted to `handleInterruptAll()` (DONE 2026-07-13)
- [x] **Extract sticky-interrupt branch from `clearThreads()`** — extracted to `clearThreadsSticky()` (DONE 2026-07-13)
- [x] **Extract `CONTINUE`/`PAUSE` thread spawn from `start()`** — extracted to `trySpawnThread()` (DONE 2026-07-13)
- [x] **Extract per-dispatcher trigger removal from `_removeDispatch()`** — extracted to `removeDispatcherTriggers()` (DONE 2026-07-13)
- [x] **Extract trigger actor fixup loop from `copy()`** — extracted to `fixupTriggerActors()` (DONE 2026-07-13)
- [x] **Extract pre-loop preamble from `run()`** — extracted to `runPreamble()` (DONE 2026-07-13)

## 🧹 Cleanup & Deprecation

- [x] **Unused Code**: Address or remove unused components like `internal.cpp`. (DONE 2026-07-13 — removed `src/internal.cpp` and `mcr/internal.h`)
- [x] **Property Usage**: Verify if the `properties.h` header file is genuinely no longer used, as indicated in comments. (DONE 2026-07-13 — file does not exist in the project)

