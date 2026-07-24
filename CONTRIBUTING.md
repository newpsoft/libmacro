# Contributing to Libmacro

Thank you for considering contributing to Libmacro.

## Getting started

1. Fork and clone the repository
2. Install requirements: CMake 3.10+, GCC 7+ or Clang 5+, Qt6 (for testing)
3. Build and run tests:
   ```sh
   cmake -B build -DBUILD_TESTING=ON .
   cmake --build build
   build/tst_libmacro
   ```

## Code style

### Formatting

All C and C++ code must be formatted with clang-format using the project's
[.clang-format](.clang-format) configuration. Run from any directory:

```sh
scripts/style
```

Manual formatting will not be preserved in code review.

### Conventions

- **Language standard**: C17 / C++17
- **Naming**: Linux Kernel Normal Form (KNF), similar to K&R
- **Includes**: Sorted alphabetically within each group (Google style)
  - Use `:sort` in vim or follow the
    [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
- **Platform files**: Use `p_` prefix for platform-specific source files
- **Internal API**: Symbols in `mcr::internal` namespace are library-internal.
  Consumers must not depend on them.

### Documentation

- Public API headers use Doxygen `@brief`, `@param`, `@return` annotations
- New public methods must include Doxygen documentation
- Keep `docs/` files up to date when changing build options or APIs

## Pull request workflow

1. Create a feature branch from `master`
2. Make changes, following the style guide
3. Run formatting: `scripts/style`
4. Build and test:
   ```sh
   cmake -B build -DBUILD_TESTING=ON -DTEST_TERMINAL=OFF .
   cmake --build build
   build/tst_libmacro
   ```
5. Open a PR against `master`

### Commit messages

- Use imperative mood ("Add feature" not "Added feature")
- Keep subject line under 72 characters
- Reference issues where applicable

## Reporting issues

Open an issue on GitHub with:
- Platform and compiler information
- CMake configuration used
- Steps to reproduce
- Expected vs actual behavior

## License

By contributing, you agree that your contributions will be licensed under the
LGPL v2.1.
