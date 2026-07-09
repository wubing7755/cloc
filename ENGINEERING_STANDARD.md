# Engineering Standard

This document records the engineering baseline for cloc.

## Principles

- Keep the command-line tool simple and predictable.
- Put stable public APIs in `include/`; keep private helpers in `src/`.
- Keep modules small enough that ownership and cleanup paths are visible.
- Prefer explicit behavior and tests over broad abstractions.
- Keep build, test, package, and publish workflows repeatable from a clean checkout.

## Project Shape

```text
include/cloc/   Public C APIs
src/cloc/       Core implementation
src/main.c                    CLI entry point
tests/                        Focused CTest programs
tests/package_smoke/          Installed-package consumer smoke test
tests/subproject_smoke/       add_subdirectory() consumer smoke test
cmake/                        CMake modules
scripts/                      Bootstrap, check, and publish wrappers
doc/                          Contributor documentation
doc/guides/                   Topic guides
.github/workflows/            CI workflows
```

## CMake

- Keep the top-level `CMakeLists.txt` small and delegate repeated logic to
  `cmake/` modules.
- Add implementation files to `CLOC_CORE_SOURCES` in
  `cmake/Sources.cmake`.
- Keep CLI sources separate from the reusable core library.
- Keep public target requirements narrow. Consumers should inherit only what the
  public headers require.
- Preserve `add_subdirectory()` behavior: CLI, tests, and install rules default
  to on for top-level builds and off for subproject builds.

## C API

- Preserve the public `CLOC_` macro prefix.
- Use `cloc_` for public functions.
- Keep public structs stable and documented before adding new fields.
- Keep implementation-only helpers out of public headers.
- Avoid global mutable state unless a design note explains why it is necessary.

## Testing

- Use CTest for automated tests.
- Test behavior through public APIs whenever practical.
- Add regression tests for bug fixes.
- Keep package and subproject smoke tests small; they should verify consumer
  integration, not duplicate unit-test coverage.
- Do not weaken warnings, tests, or CI to make a change pass.

## Release And Publishing

- Use `scripts/publish.ps1` or `scripts/publish.sh` to create a release build
  and copy the CLI executable to an output directory.
- Run release-oriented checks before publishing externally.
- Do not publish artifacts from AI-assisted changes without maintainer approval.

## Documentation

- Keep `README.md` focused on user-facing usage and quick start.
- Keep `doc/guides/cmake.md` as the source of truth for configure, build, test,
  install, package, subproject, and publish workflows.
- Update docs with code changes that affect commands, public APIs, packaging,
  release behavior, or supported platforms.
