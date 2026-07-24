# Requirements

## Build tools

| Tool | Minimum version | Notes |
|------|----------------|-------|
| CMake | 3.10 | Required |
| Git | any | Required for version extraction |
| Doxygen | 1.8+ | Optional, for documentation |

## Compilers

| Compiler | Minimum version | Standard |
|----------|----------------|----------|
| GCC | 7+ | C17 / C++17 |
| Clang | 5+ | C17 / C++17 |
| MSVC | 2017 (19.14+) | C17 / C++17 |

The project requires C17 and C++17 language standards. Older compilers that
do not support these standards will fail to build.

## Libraries

| Library | Required | Purpose |
|---------|----------|---------|
| Threads (pthreads / Win32 threads) | Yes | Macro threading model |
| Qt6Core | Testing only | Test framework foundation |
| Qt6Test | Testing only | QTestLib test framework |

## Platform-specific requirements

### Linux

- Standard POSIX environment
- `/dev/uinput` and `/dev/input` for input device interaction (runtime)
- GCC or Clang recommended

### Windows

- Visual Studio 2017+ or MinGW with GCC 7+
- NSIS (optional, for installer packaging)

## Qt setup

If Qt6 is not in a standard location, set one of:

```sh
# Option 1: Set prefix path
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt6 .

# Option 2: Set individual variables
cmake -B build -DQt6_DIR=/path/to/qt6/lib/cmake/Qt6 .
```

## CI environment

The GitHub Actions workflow uses:

- `ubuntu-latest`
- `jurplel/install-qt-action@v2` with Qt 6.11.1
- `TEST_TERMINAL=OFF` (headless)
