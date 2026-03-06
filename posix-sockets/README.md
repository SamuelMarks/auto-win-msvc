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
