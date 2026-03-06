# posix-core

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `open` (Mapped to `_open`)
- [x] `close` (Mapped to `_close`)
- [x] `read` (Mapped to `_read`)
- [x] `write` (Mapped to `_write`)
- [x] `lseek` (Mapped to `_lseek`)
- [x] `dup` (Mapped to `_dup`)
- [x] `dup2` (Mapped to `_dup2`)
- [x] `fsync` (Mapped to `_commit`)
- [x] `ftruncate` (Mapped to `_chsize`)
- [x] `access` (Mapped to `_access`)
- [x] `rmdir` (Mapped to `_rmdir`)
- [x] `chdir` (Mapped to `_chdir`)
- [x] `getcwd` (Mapped to `_getcwd`)
- [x] `unlink` (Mapped to `_unlink`)
- [x] `execve` (Mapped to `_execve`)
- [x] `execv` (Mapped to `_execv`)
- [x] `execvp` (Mapped to `_execvp`)
- [x] `execl` (Mapped to `_execl`)
- [x] `execlp` (Mapped to `_execlp`)
- [x] `getpid` (Mapped to `_getpid`)
- [x] `sleep` (Mapped to `Sleep`)
- [x] `usleep` (Mapped to `Sleep`)
- [x] `isatty` (Mapped to `_isatty`)
- [x] `swab` (Mapped to `_swab`)
- [x] `creat` (Mapped to `_creat`)
- [ ] `fcntl` (TODO: Polyfill with ioctlsocket / SetHandleInformation)
- [ ] `openat` (TODO: Polyfill with CreateFile (relative))
- [ ] `posix_fadvise` (TODO: Polyfill with Win32 API)
- [ ] `posix_fallocate` (TODO: Polyfill with SetEndOfFile)
- [ ] `alarm` (TODO: Polyfill with CreateWaitableTimer)
- [ ] `chown` (TODO: Polyfill with SetNamedSecurityInfo)
- [ ] `confstr` (TODO: Polyfill with Win32 API)
- [ ] `crypt` (TODO: Polyfill with Win32 API)
- [ ] `encrypt` (TODO: Polyfill with Win32 API)
- [ ] `faccessat` (TODO: Polyfill with Win32 API)
- [ ] `fchown` (TODO: Polyfill with SetSecurityInfo)
- [ ] `fchownat` (TODO: Polyfill with Win32 API)
- [ ] `fdatasync` (TODO: Polyfill with FlushFileBuffers)
- [ ] `fexecve` (TODO: Polyfill with Win32 API)
- [ ] `fork` (TODO: Polyfill with CreateProcess)
- [ ] `fpathconf` (TODO: Polyfill with Win32 API)
- [ ] `getegid` (TODO: Polyfill with Win32 API)
- [ ] `geteuid` (TODO: Polyfill with Win32 API)
- [ ] `getgid` (TODO: Polyfill with Win32 API)
- [ ] `getgroups` (TODO: Polyfill with GetTokenInformation)
- [ ] `gethostid` (TODO: Polyfill with Win32 API)
- [ ] `gethostname` (TODO: Polyfill with GetComputerName)
- [ ] `getlogin` (TODO: Polyfill with GetUserName)
- [ ] `getlogin_r` (TODO: Polyfill with Win32 API)
- [ ] `getopt` (TODO: Polyfill with Custom loop / Native wingetopt)
- [ ] `getpgid` (TODO: Polyfill with Win32 API)
- [ ] `getpgrp` (TODO: Polyfill with Win32 API)
- [ ] `getppid` (TODO: Polyfill with NtQueryInformationProcess)
- [ ] `getsid` (TODO: Polyfill with Win32 API)
- [ ] `getuid` (TODO: Polyfill with GetUserName)
- [ ] `lchown` (TODO: Polyfill with Win32 API)
- [ ] `link` (TODO: Polyfill with CreateHardLink)
- [ ] `linkat` (TODO: Polyfill with Win32 API)
- [ ] `lockf` (TODO: Polyfill with _locking)
- [ ] `pathconf` (TODO: Polyfill with Win32 API)
- [ ] `pause` (TODO: Polyfill with Sleep(INFINITE))
- [ ] `pipe` (TODO: Polyfill with _pipe / CreatePipe)
- [ ] `pread` (TODO: Polyfill with ReadFile with OVERLAPPED)
- [ ] `pwrite` (TODO: Polyfill with WriteFile with OVERLAPPED)
- [ ] `readlink` (TODO: Polyfill with GetFinalPathNameByHandle)
- [ ] `readlinkat` (TODO: Polyfill with Win32 API)
- [ ] `setegid` (TODO: Polyfill with Win32 API)
- [ ] `seteuid` (TODO: Polyfill with Win32 API)
- [ ] `setgid` (TODO: Polyfill with Win32 API)
- [ ] `setpgid` (TODO: Polyfill with Win32 API)
- [ ] `setpgrp` (TODO: Polyfill with Win32 API)
- [ ] `setregid` (TODO: Polyfill with Win32 API)
- [ ] `setreuid` (TODO: Polyfill with Win32 API)
- [ ] `setsid` (TODO: Polyfill with Win32 API)
- [ ] `setuid` (TODO: Polyfill with Win32 API)
- [ ] `symlink` (TODO: Polyfill with CreateSymbolicLink)
- [ ] `symlinkat` (TODO: Polyfill with Win32 API)
- [ ] `sync` (TODO: Polyfill with FlushFileBuffers)
- [ ] `sysconf` (TODO: Polyfill with GetSystemInfo)
- [ ] `tcgetpgrp` (TODO: Polyfill with Win32 API)
- [ ] `tcsetpgrp` (TODO: Polyfill with Win32 API)
- [ ] `truncate` (TODO: Polyfill with SetEndOfFile)
- [ ] `ttyname` (TODO: Polyfill with Win32 API)
- [ ] `ttyname_r` (TODO: Polyfill with Win32 API)
- [ ] `ualarm` (TODO: Polyfill with Win32 API)
- [ ] `vfork` (TODO: Polyfill with CreateProcess)


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
    "posix-core"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-core
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-core
)
FetchContent_MakeAvailable(posix-core)
target_link_libraries(your_target PRIVATE posix-core)
```
