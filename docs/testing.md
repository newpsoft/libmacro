# Testing

## Overview

Libmacro uses Qt Test (`QTestLib`) for its test suite. Tests verify signal
dispatch, trigger matching, macro execution, serialization, and library
creation.

## Building tests

```sh
cmake -B build -DBUILD_TESTING=ON .
cmake --build build --target tst_libmacro
```

## Running tests

### Combined test suite

```sh
build/tst_libmacro
```

### Individual test suites

Each test can be built and run independently:

| Target | Tests |
|--------|-------|
| `taction` | Action trigger behavior |
| `tcreate` | Context and library instance creation |
| `tdispatcher` | Signal dispatch mechanics |
| `tmacro` | Macro lifecycle and threading |
| `tserial` | Serialization name-value mappings |

```sh
cmake --build build --target tcreate
build/tcreate
```

### Running all tests via CTest

```sh
cmake --build build
cd build && ctest
```

## Terminal access

Some tests require an interactive terminal (e.g., input device interaction).
Set `TEST_TERMINAL` to enable these:

```sh
cmake -B build -DBUILD_TESTING=ON -DTEST_TERMINAL=ON .
```

In CI, `TEST_TERMINAL=OFF` is the default. Tests that need a terminal are
skipped when this flag is off.

## Coverage

GCC and Clang builds automatically enable coverage flags (`--coverage`). To
generate an HTML coverage report:

```sh
scripts/coverage.sh
```

Coverage output is written to `build/coverage/`.

## Known failures

See `test/known_failures.txt` for tests that are expected to fail. Check this
file before assuming a test regression.

## Test structure

```
test/
  main.cpp          # Combined test runner entry point
  main.h            # Shared test setup
  taction.cpp/.h    # Action trigger tests
  tcreate.cpp/.h    # Creation tests
  tdispatcher.cpp/.h # Dispatcher tests
  tmacro.cpp/.h     # Macro tests
  tserial.cpp/.h    # Serialization tests
  expect_actor.h    # Test helper for expected actors
  expect_receiver.h # Test helper for expected receivers
  known_failures.txt
  assets/           # Test data files
```

Individual test files define `TEST_INDIVIDUAL` for standalone builds and
use `QTEST_GUILESS_MAIN` as the entry point.
