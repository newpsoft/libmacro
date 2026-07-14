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

## Project state

- `SPEC.MD` file holds issues to be fixed.
- `TODO.md` file holds TODOs and other decisions to be made about issues or architecture.

