# posix-mman

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [ ] `mlock` (TODO: Polyfill with VirtualLock)
- [ ] `mlockall` (TODO: Polyfill with Win32 API)
- [ ] `mmap` (TODO: Polyfill with CreateFileMapping / MapViewOfFile)
- [ ] `mprotect` (TODO: Polyfill with VirtualProtect)
- [ ] `msync` (TODO: Polyfill with FlushViewOfFile)
- [ ] `munlock` (TODO: Polyfill with VirtualUnlock)
- [ ] `munlockall` (TODO: Polyfill with Win32 API)
- [ ] `munmap` (TODO: Polyfill with UnmapViewOfFile)
- [ ] `shm_open` (TODO: Polyfill with CreateFileMapping)
- [ ] `shm_unlink` (TODO: Polyfill with Win32 API)


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
    "posix-mman"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-mman
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-mman
)
FetchContent_MakeAvailable(posix-mman)
target_link_libraries(your_target PRIVATE posix-mman)
```
