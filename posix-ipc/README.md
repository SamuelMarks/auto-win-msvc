# posix-ipc

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [ ] `ftok` (TODO: Polyfill with Hash of GetFileInformationByHandle)
- [ ] `msgctl` (TODO: Polyfill with Win32 API)
- [ ] `msgget` (TODO: Polyfill with Win32 API)
- [ ] `msgrcv` (TODO: Polyfill with Win32 API)
- [ ] `msgsnd` (TODO: Polyfill with Win32 API)
- [ ] `semctl` (TODO: Polyfill with Win32 API)
- [ ] `semget` (TODO: Polyfill with Win32 API)
- [ ] `semop` (TODO: Polyfill with Win32 API)
- [ ] `shmat` (TODO: Polyfill with Win32 API)
- [ ] `shmctl` (TODO: Polyfill with Win32 API)
- [ ] `shmdt` (TODO: Polyfill with Win32 API)
- [ ] `shmget` (TODO: Polyfill with Win32 API)


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
    "posix-ipc"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-ipc
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-ipc
)
FetchContent_MakeAvailable(posix-ipc)
target_link_libraries(your_target PRIVATE posix-ipc)
```
