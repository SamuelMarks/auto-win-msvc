# posix-termios

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [ ] `cfgetispeed` (TODO: Polyfill with Win32 API)
- [ ] `cfgetospeed` (TODO: Polyfill with Win32 API)
- [ ] `cfsetispeed` (TODO: Polyfill with Win32 API)
- [ ] `cfsetospeed` (TODO: Polyfill with Win32 API)
- [ ] `tcdrain` (TODO: Polyfill with FlushConsoleInputBuffer)
- [ ] `tcflow` (TODO: Polyfill with Win32 API)
- [ ] `tcflush` (TODO: Polyfill with Win32 API)
- [ ] `tcgetattr` (TODO: Polyfill with GetConsoleMode)
- [ ] `tcgetsid` (TODO: Polyfill with Win32 API)
- [ ] `tcsendbreak` (TODO: Polyfill with Win32 API)
- [ ] `tcsetattr` (TODO: Polyfill with SetConsoleMode)


## Current Status & Future Plans

**Current Status:**
- The `auto-win-msvc` monorepo has been successfully scaffolded into 18 distinct, modular CMake projects.
- All standard POSIX headers and types are generated and strictly C89 compliant.
- Simple functions with direct MSVC equivalents (e.g., `open` -> `_open`) are fully mapped via macros.
- Complex POSIX APIs requiring polyfills (e.g., `mmap`, `pthreads`, `dirent`) are currently scaffolded as `ENOSYS` stubs, with their target Win32 APIs documented in `mappings.json` files.
- Test files and build systems (CMake and vcpkg) are in place.

**Future Plans:**
- **AI-Driven Iteration:** Iteratively implement all stubbed polyfills using native Win32 APIs across the 18 modules, maintaining 0 compiler warnings (`/W4 /WX`) and strict C89 compliance.
- **cdd-c Integration:** Expand `cdd-c` into a Concrete Syntax Tree (CST) weaver (as outlined in `cdd-c-expansion.md`). This will allow automated, byte-for-byte precise injection of `auto-win-msvc` polyfills and standard `#ifdef _MSC_VER` guards directly into legacy C codebases.

## Installation

### Vcpkg
Add to your `vcpkg.json`:
```json
{
  "dependencies": [
    "posix-termios"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-termios
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-termios
)
FetchContent_MakeAvailable(posix-termios)
target_link_libraries(your_target PRIVATE posix-termios)
```
