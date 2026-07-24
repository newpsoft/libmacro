# Libmacro

Libmacro is a multi-platform, extensible macro and hotkey C/C++ library.
It provides signal dispatch, trigger matching, modifier tracking, and a
serialization layer for mapping names to enum values -- all with a plugin
architecture for custom signal and trigger types.

## License

Libmacro uses the LGPL v2.1 license. Please refer to the [LICENSE](./LICENSE)
or [lgpl-2.1.txt](./lgpl-2.1.txt) files for more information.

## Requirements

- **CMake** 3.10+
- **C17 / C++17** compiler:
  - GCC 7+
  - Clang 5+
  - MSVC 2017+ (Visual Studio)
- **Threads** (pthreads on Linux, Win32 threads on Windows)
- **Qt6Core** and **Qt6Test** (testing only)

See [docs/requirements.md](docs/requirements.md) for full details.

## Building

```sh
cmake -B build -DBUILD_TESTING=ON .
cmake --build build
```

Key CMake options:

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTING` | ON | Build test executables |
| `BUILD_DOC` | ON (Release) | Build Doxygen documentation |
| `BUILD_SHARED_LIBS` | ON | Build shared library (OFF for static) |
| `BUILD_PACKAGE` | ON (Release) | Build CPack packaging materials |
| `TEST_TERMINAL` | ON | Tests have terminal access |
| `CMAKE_BUILD_TYPE` | -- | `Debug` or `Release` |

See [docs/cmake_modules.md](docs/cmake_modules.md) for all options.

The library target is `mcr::Libmacro` (alias). On Linux the output is
`libmacro.so` (`macro`), on Windows `libmacro.dll` (`libmacro`).

## Quick start

```cpp
#include "mcr/libmacro.h"

int main()
{
    mcr::Libmacro ctx;
    ctx.setEnabled(true);

    // Register a signal type
    auto &sigReg = ctx.signalRegistry();
    sigReg.map<mcr::Modifier>();

    // Register a trigger type
    auto &trigReg = ctx.triggerRegistry();
    trigReg.map<mcr::Action>();

    // Create and configure a macro
    mcr::Macro macro(&ctx);

    // Clean up -- disable before destruction
    ctx.setEnabled(false);
    return 0;
}
```

See [docs/getting-started.md](docs/getting-started.md) for a full tutorial.

## Documentation

- [Getting started](docs/getting-started.md) -- 5-minute tutorial
- [Architecture](docs/architecture.md) -- Design and subsystems
- [Usage](docs/usage.md) -- Extending name-value mappings
- [Platform guide](docs/platform.md) -- Platform-specific symbols
- [CMake options](docs/cmake_modules.md) -- Build configuration
- [Requirements](docs/requirements.md) -- Dependencies and toolchains
- [Testing](docs/testing.md) -- Test suites and coverage
- API reference: build with `-DBUILD_DOC=ON` and open `build/doxygen/html/index.html`

## Supported platforms

- Linux
- Windows

See [docs/platform.md](docs/platform.md) for platform-specific details.

## Contributing

Please follow style guides when it makes sense. clang-format will be used for all
style formatting and manual formatting will not be kept. The following are
known style guidelines used in this project.

- C and C++ style is the Linux Kernel Normal Form (KNF), which is similar to K&R. The
  clang-format config file is [.clang-format](.clang-format).
- C and C++ `#include` statements will be sorted alphabetically (Use vim `:sort` or `:sort u`).
  They may also follow the Google style guide found at
  [https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
- More of the Google style guide may be adopted in the future.

### Style source

clang-format is required for correct formatting. [scripts/style](scripts/style) runs clang-format for
all source files, with KNF options. Run from any directory to format any C
and C++ header and source files in that directory. Run with `scripts/style --help`
for more information.

### Platform Style Guides

- Refer to [docs/platform.md](docs/platform.md)
- Platform files use "p\_" as a naming prefix.

### Internal API

Symbols in the `mcr::internal` namespace are library-internal and not part of the
public API. Consumers must not depend on them.
