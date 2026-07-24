# CMake Modules

Libmacro uses several CMake configuration files under `config/` to set up
platform detection, versioning, documentation, testing, coverage, and
packaging.

## Module load order

From the root `CMakeLists.txt`:

1. `config/libmacro.cmake` -- version and platform detection (before `project()`)
2. `config/git.cmake` -- Git revision extraction (before `project()`)
3. `config/doxygen.cmake` -- documentation target
4. `config/coverage.cmake` -- test targets and coverage flags
5. `config/cpack.cmake` -- packaging configuration

## CMake options

### General

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | ON | Build shared library. OFF for static. |
| `BUILD_DOC` | ON (Release) | Build Doxygen API documentation. |
| `BUILD_PACKAGE` | ON (Release) | Build CPack packaging materials. |
| `BUILD_TESTING` | ON | Build test executables. |

### Testing

| Option | Default | Description |
|--------|---------|-------------|
| `TEST_TERMINAL` | ON | Tests have access to a terminal. Disable for headless/CI environments. |

### Build configuration

| Variable | Description |
|----------|-------------|
| `CMAKE_BUILD_TYPE` | `Debug` or `Release`. Controls defaults for `BUILD_DOC` and `BUILD_PACKAGE`. |
| `CMAKE_CXX_COMPILER` | C++ compiler executable (GCC, Clang, MSVC). |
| `CMAKE_C_COMPILER` | C compiler executable. |
| `CMAKE_INSTALL_PREFIX` | Installation prefix path. |
| `CMAKE_PREFIX_PATH` | Custom paths for headers, libraries, and Qt. |
| `Qt6_DIR` | Path to Qt6 cmake directory. |
| `Qt6Core_DIR` | Path to Qt6Core cmake directory. |
| `Qt6Test_DIR` | Path to Qt6Test cmake directory. |

## Platform detection (`config/libmacro.cmake`)

Sets the following variables:

| Variable | Values |
|----------|--------|
| `MCR_PLATFORM` | `linux`, `windows`, `apple`, or `none` |
| `MCR_PLATFORM_UPPER` | Uppercase of `MCR_PLATFORM` (e.g. `LINUX`) |
| `MCR_VER` | Version string (`MAJOR.MINOR`) |
| `MCR_VER_MAJ` | Major version number |
| `MCR_VER_MIN` | Minor version number |
| `MCR_DEBUG` | Set in Debug builds |

Adds compile definitions: `-DMCR_PLATFORM=...`, `-DMCR_PLATFORM_LINUX=1` (etc.),
`-DMCR_DEBUG=1` (debug only).

## Git revision (`config/git.cmake`)

Extracts from the local repository:

| Variable | Description |
|----------|-------------|
| `GIT_REVISION` | Commit count on `master` |
| `GIT_BRANCH` | Current branch name |
| `GIT_EXECUTABLE` | Path to git binary |

## Documentation (`config/doxygen.cmake`)

When `BUILD_DOC=ON` and Doxygen is found:

- Processes `Doxyfile.in` into `build/Doxyfile`
- Creates a `doxygen` custom target that generates HTML in `build/doxygen/html/`
- The target is added to `ALL` so it builds with the default target

## Testing and coverage (`config/coverage.cmake`)

### Test targets

| Target | Description |
|--------|-------------|
| `tst_libmacro` | Combined test executable (all test sources) |
| `taction` | Individual action trigger tests |
| `tcreate` | Individual creation tests |
| `tdispatcher` | Individual dispatcher tests |
| `tmacro` | Individual macro tests |
| `tserial` | Individual serialization tests |

All test targets link against `mcr::Libmacro`, `Qt6::Core`, and `Qt6::Test`.

### Coverage

On GCC/Clang, coverage flags (`-fprofile-arcs -ftest-coverage -O0 -g`) are
added to both the library and test targets. Use `scripts/coverage.sh` to
generate HTML reports.

### Qt MOC

`CMAKE_AUTOUIC`, `CMAKE_AUTOMOC`, and `CMAKE_AUTORCC` are enabled for test
builds.

## Packaging (`config/cpack.cmake`)

When `BUILD_PACKAGE=ON`:

- Sets CPack metadata (name, version, vendor, license)
- On Windows: configures NSIS installer settings
- Library installs to `lib/` (Linux) or `/` (Windows)
- Headers install to `include/mcr/`

## Installing

```sh
cmake --build build
cmake --install build --prefix /usr/local
```

This installs:
- Library to `<prefix>/lib/`
- Headers to `<prefix>/include/mcr/`
