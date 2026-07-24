# Changelog

All notable changes to Libmacro will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/).

## [Unreleased]

### Fixed
- Race condition in `globalThreadLimitPreventsExcessiveThreads` test
- Missing `SignalList::send()` implementation
- `MCR_API` removed from template instantiations

## [0.3] - 2024

### Changed
- Major simplification: removed Qt dependency from library core
- Reorganized C++ API with factory pattern (`mcr::factory` namespace)
- Simplified dispatch and threading model

### Removed
- Qt runtime dependency (Qt6 still required for testing only)

## [0.2]

### Added
- Multi-platform support (Linux, Windows)
- Signal dispatch pipeline with `IDispatcher` interface
- Threaded macro execution with global thread limit
- Extensible signal and trigger registries
- Serialization layer with custom name-value mapping
- Doxygen documentation generation

### Fixed
- Multiple key dispatches
- Duplicate dispatch handling

## [0.1]

### Added
- Initial release
- Basic macro and hotkey functionality
- Linux platform support via uinput
