# CodeLineCalculator

CodeLineCalculator is a C11 command-line tool for counting source lines in a
project directory. It is intended for cases such as software copyright
registration forms where a total source-code line count is required.

## Command Line Usage

Build the project, then run:

```sh
codelinecalculator <absolute-project-path> [--preset <name>]
codelinecalculator <absolute-project-path> [suffix...]
codelinecalculator --interactive
codelinecalculator --list-presets
codelinecalculator --help
```

Examples:

```powershell
.\build\ninja-debug\bin\codelinecalculator.exe C:\work\my-project
```

Use a preset for a project type:

```powershell
.\build\ninja-debug\bin\codelinecalculator.exe C:\work\my-project --preset web
```

Run interactively:

```powershell
.\build\ninja-debug\bin\codelinecalculator.exe --interactive
```

Show help:

```powershell
.\build\ninja-debug\bin\codelinecalculator.exe help
```

```powershell
.\build\ninja-debug\bin\codelinecalculator.exe C:\work\my-project "*.c" "*.h" "*.js" "*.ts" "*.cs" "*.razor" "*.css"
```

```sh
./build/ninja-debug/bin/codelinecalculator /home/me/my-project c h cpp js ts cs razor css
```

List available presets:

```powershell
.\build\ninja-debug\bin\codelinecalculator.exe --list-presets
```

Built-in presets:

| Preset | Use case |
| --- | --- |
| `common` | Default. Common code suffixes across mainstream project types. |
| `c` | C source and header files. |
| `cpp` | C and C++ source/header files. |
| `web` | JavaScript, TypeScript, CSS, HTML, Vue, and Svelte files. |
| `dotnet` | C#, Razor, CSHTML, and XAML files. |

Suffixes may be written as `*.js`, `.js`, or `js`. If no suffixes or preset are
provided, the tool uses the `common` preset.

Interactive mode asks for the project absolute path, then lets the user choose a
preset by number/name or enter a custom suffix list separated by spaces, commas,
or semicolons.

The tool counts physical lines, including comments and blank lines, and counts
a final line even when the file does not end with a newline. Common generated
or build directories such as `.git`, `build`, `out`, `dist`, `install`,
`node_modules`, `bin`, `obj`, `target`, and `cmake-build-*` are skipped.

## Publish

Create a release build and copy the command-line executable to an output
directory:

**Windows PowerShell**

```powershell
.\scripts\publish.ps1 C:\tools\CodeLineCalculator
```

**Linux/macOS or MSYS2/Git Bash**

```sh
./scripts/publish.sh /opt/codelinecalculator
```

If no output directory is provided, the executable is copied to the repository
root. The scripts use the `ninja-release` preset by default and accept
`--preset <name>` when another preset is needed. On non-Windows platforms, the
published executable does not use the `.exe` suffix.

## Development

- CMake modules split by responsibility.
- C11 as the target-level language baseline.
- Cross-platform compiler support for MSVC, GCC, Clang, and AppleClang.
- CMake presets for debug, release, sanitizer, and coverage builds.
- Static and shared library builds through `BUILD_SHARED_LIBS`.
- CTest based unit tests.
- Installable CMake package config with downstream smoke tests.
- `add_subdirectory()` smoke tests for subproject consumers.
- GitHub Actions for build, test, and static analysis.
- Short contributor, testing, release, security, and AI-agent guidance.
- Focused public C APIs for source-line counting and source suffix presets.

## Layout

```text
include/        Public headers
src/            Implementation files
tests/          CTest test programs
cmake/          CMake modules
scripts/        Local bootstrap and check entry points
doc/            Short project documentation
doc/guides/     Topic guides
.github/        CI workflows and repository metadata
```

## Quick Start

Required tools:

- CMake 3.21 or newer.
- A C11 compiler from MSVC, GCC, Clang, or AppleClang.
- Ninja for the default cross-platform presets, or another CMake-supported
  generator through local presets.

On Windows, run the Ninja presets from a Developer PowerShell, Developer
Command Prompt, or VS Code CMake kit that initializes MSVC.

For step-by-step installation instructions per platform, see
[doc/guides/environment.md](doc/guides/environment.md).

Recommended editor tools:

- VS Code with the CMake Tools and clangd extensions.
- Ninja, because it generates `compile_commands.json` for clangd.
- Microsoft C/C++ remains usable as a local fallback; see
  `.vscode/c_cpp_properties.example.json`.

The repository includes portable VS Code recommendations, CMake tasks, clangd
defaults, an optional C/C++ fallback configuration, and Ninja presets. Compiler
installation paths and per-user kits stay in local user configuration.

First-time setup:

**Windows PowerShell**

```powershell
./scripts/bootstrap.ps1
```

**Linux/macOS**

```sh
./scripts/bootstrap.sh
```

The bootstrap script checks required tools and runs configure so editors can
load `build/ninja-debug/compile_commands.json`.

For a local Visual Studio, Xcode, or Make workflow, copy
`CMakeUserPresets.example.json` to `CMakeUserPresets.json` and edit it for your
machine. The real user preset file is ignored by Git.

Daily check:

**Windows PowerShell**

```powershell
./scripts/check.ps1
```

**Linux/macOS**

```sh
./scripts/check.sh
```

Optional clang-tidy checks:

**Windows PowerShell**

```powershell
./scripts/check.ps1 -EnableTidy
```

**Linux/macOS**

```sh
./scripts/check.sh --enable-tidy
```

For a fuller explanation of configure, build, test, install, presets, and common
errors, see [doc/guides/cmake.md](doc/guides/cmake.md).

## Documentation Map

- [doc/README.md](doc/README.md) maps each topic to its source of truth.
- [doc/guides/cmake.md](doc/guides/cmake.md) explains day-to-day CMake usage.
- [doc/guides/testing.md](doc/guides/testing.md) describes test and static-analysis
  expectations.
- [CONTRIBUTING.md](CONTRIBUTING.md) describes branch, commit, and PR workflow.
- [SECURITY.md](SECURITY.md) describes vulnerability and dependency handling.
- [ENGINEERING_STANDARD.md](ENGINEERING_STANDARD.md) defines the project engineering baseline.

## Installed Package Use

The install rules generate a CMake package config:

```cmake
find_package(CodeLineCalculator CONFIG REQUIRED)
target_link_libraries(app PRIVATE CodeLineCalculator::codelinecalculator_core)
```

The `tests/package_smoke/` project validates installed C and C++ consumers.

Direct source-tree consumption through `add_subdirectory()` is validated by
`tests/subproject_smoke/`.

See [ENGINEERING_STANDARD.md](ENGINEERING_STANDARD.md) for the project engineering baseline.
