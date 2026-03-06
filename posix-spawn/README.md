# posix-spawn

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [ ] `posix_spawn` (TODO: Polyfill with CreateProcess)
- [ ] `posix_spawn_file_actions_addclose` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawn_file_actions_adddup2` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawn_file_actions_addopen` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawn_file_actions_destroy` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawn_file_actions_init` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_destroy` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_getflags` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_getpgroup` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_getschedparam` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_getschedpolicy` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_getsigdefault` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_getsigmask` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_init` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_setflags` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_setpgroup` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_setschedparam` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_setschedpolicy` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_setsigdefault` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnattr_setsigmask` (TODO: Polyfill with Win32 API)
- [ ] `posix_spawnp` (TODO: Polyfill with CreateProcess)


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
    "posix-spawn"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-spawn
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-spawn
)
FetchContent_MakeAvailable(posix-spawn)
target_link_libraries(your_target PRIVATE posix-spawn)
```
