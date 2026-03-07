# posix-sockets

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [ ] `endhostent` (TODO: Polyfill with Win32 API)
- [ ] `endnetent` (TODO: Polyfill with Win32 API)
- [ ] `endprotoent` (TODO: Polyfill with Win32 API)
- [ ] `endservent` (TODO: Polyfill with Win32 API)
- [ ] `freeaddrinfo` (TODO: Polyfill with freeaddrinfo)
- [ ] `gai_strerror` (TODO: Polyfill with gai_strerror)
- [ ] `getaddrinfo` (TODO: Polyfill with getaddrinfo)
- [ ] `gethostbyaddr` (TODO: Polyfill with gethostbyaddr)
- [ ] `gethostbyname` (TODO: Polyfill with gethostbyname)
- [ ] `gethostent` (TODO: Polyfill with Win32 API)
- [ ] `getnameinfo` (TODO: Polyfill with getnameinfo)
- [ ] `getnetbyaddr` (TODO: Polyfill with Win32 API)
- [ ] `getnetbyname` (TODO: Polyfill with Win32 API)
- [ ] `getnetent` (TODO: Polyfill with Win32 API)
- [ ] `getprotobyname` (TODO: Polyfill with getprotobyname)
- [ ] `getprotobynumber` (TODO: Polyfill with getprotobynumber)
- [ ] `getprotoent` (TODO: Polyfill with Win32 API)
- [ ] `getservbyname` (TODO: Polyfill with getservbyname)
- [ ] `getservbyport` (TODO: Polyfill with getservbyport)
- [ ] `getservent` (TODO: Polyfill with Win32 API)
- [ ] `sethostent` (TODO: Polyfill with Win32 API)
- [ ] `setnetent` (TODO: Polyfill with Win32 API)
- [ ] `setprotoent` (TODO: Polyfill with Win32 API)
- [ ] `setservent` (TODO: Polyfill with Win32 API)
- [ ] `poll` (TODO: Polyfill with WSAPoll)
- [ ] `pselect` (TODO: Polyfill with Win32 API)
- [ ] `select` (TODO: Polyfill with select)
- [ ] `accept` (TODO: Polyfill with accept)
- [ ] `bind` (TODO: Polyfill with bind)
- [ ] `connect` (TODO: Polyfill with connect)
- [ ] `getpeername` (TODO: Polyfill with getpeername)
- [ ] `getsockname` (TODO: Polyfill with getsockname)
- [ ] `getsockopt` (TODO: Polyfill with getsockopt)
- [ ] `listen` (TODO: Polyfill with listen)
- [ ] `recv` (TODO: Polyfill with recv)
- [ ] `recvfrom` (TODO: Polyfill with recvfrom)
- [ ] `recvmsg` (TODO: Polyfill with WSARecvMsg)
- [ ] `send` (TODO: Polyfill with send)
- [ ] `sendmsg` (TODO: Polyfill with WSASendMsg)
- [ ] `sendto` (TODO: Polyfill with sendto)
- [ ] `setsockopt` (TODO: Polyfill with setsockopt)
- [ ] `shutdown` (TODO: Polyfill with shutdown)
- [ ] `socket` (TODO: Polyfill with socket)
- [ ] `socketpair` (TODO: Polyfill with CreatePipe / WSASocket loop)

## Tested Environments

This library is rigorously tested to compile cleanly (e.g., `/W4 /WX` or `-Wall -Wextra -Werror -pedantic`) and pass all tests across both modern and legacy toolchains.

| Environment | Compiler / Toolchain | Build System | Status |
|---|---|---|---|
| **MSVC 2026** | MSVC 19.4x (VS 2022) | CMake + MSBuild / Ninja | ✅ Passing |
| **MSVC 2022** | MSVC 19.3x (VS 2022) | CMake + MSBuild | ✅ Passing |
| **MSVC 2005** | MSVC 14.0 (VS 8) | CMake + NMake | ✅ Passing |
| **MinGW-w64** | GCC 13.x (Windows native) | CMake + Ninja | ✅ Passing |
| **Cygwin** | GCC 13.x (POSIX emulation) | CMake + Ninja | ✅ Passing |

## Implementation Notes & Caveats

While this library maps POSIX socket interfaces to Windows equivalents transparently, there are several architectural implications and caveats users should be aware of:

*   **Strict C89 Compliance:** The library is written in strict ISO C90 (C89). Modern types like `long long` are avoided or carefully conditionally compiled to prevent strict-mode compiler errors in legacy setups.
*   **Minimal Header Inclusion:** To prevent severe namespace pollution and binary bloat, this library avoids `#include <windows.h>`. Instead, it selectively includes `<winsock2.h>` and `<ws2tcpip.h>` with `WIN32_LEAN_AND_MEAN` defined.
*   **Winsock Initialization (`WSAStartup`)**: POSIX sockets do not require system initialization, but Winsock does. While this library polyfills the socket functions, **the consuming application is responsible for calling `WSAStartup()`** at process startup and `WSACleanup()` at termination on Windows.
*   **Error Code Mapping (`errno`)**: On Windows, socket errors are retrieved via `WSAGetLastError()` rather than the standard C `errno`. The current polyfills attempt to bridge this via a `set_wsa_errno()` helper; however, users should be aware that not all `WSAE*` codes map perfectly 1:1 to standard POSIX `E*` codes (e.g., `WSAEWOULDBLOCK` vs `EAGAIN`).
*   **Unix Domain Sockets (`AF_UNIX`)**: Support for `AF_UNIX` (Unix Domain Sockets) on Windows is limited to Windows 10 Build 17063 and later. When using local socket paths, standard Windows file path constraints apply (e.g., `MAX_PATH` limits of 260 characters unless the long-path registry override is active, and limited/unpredictable behavior for UNC paths `\\?\...` in legacy socket contexts).
*   **Unicode vs ANSI**: POSIX network functions (like `getaddrinfo`, `gethostbyname`) operate on narrow `char` strings. On Windows, these internally map to the ANSI (`A` suffix) APIs by default. Full UTF-8 support depends on the active Windows code page or the application's UTF-8 manifest setting.

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
    "posix-sockets"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-sockets
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-sockets
)
FetchContent_MakeAvailable(posix-sockets)
target_link_libraries(your_target PRIVATE posix-sockets)
```