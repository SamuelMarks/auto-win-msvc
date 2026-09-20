auto-win-msvc
=============
![Doc Coverage](https://img.shields.io/badge/doc__coverage-98%25-brightgreen)
![Test Coverage](https://img.shields.io/badge/test__coverage-100%25-brightgreen)
[![License](https://img.shields.io/badge/license-CC0%20OR%20Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![MSVC POSIX Monorepo CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml)
[![Multi-Environment CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml)
[![Downstream Integration](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/downstream.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/downstream.yml)

**A Massive, Modular POSIX Compatibility Layer for MSVC, MinGW, and Cygwin**

`auto-win-msvc` is a comprehensive suite of C89-compliant libraries designed to provide POSIX, Linux, BSD, macOS, Solaris, and Haiku user-space APIs natively on Windows. Validated against Microsoft Visual C++ (MSVC 2005, 2022, 2026), MinGW-w64, Cygwin, GCC, and Clang, this monorepo expands across **78 distinct modules** to cover nearly 100% of standard user-space APIs.

## 🏆 Current Feature Set & Implementation Details

- **Massive Scope (78 Modules):** Modular polyfills spanning POSIX.1-2008, Linux, BSD (FreeBSD/NetBSD/OpenBSD), macOS (Mach / CoreFoundation), Solaris, and Haiku OS headers.
- **Strict ISO C90 (C89) Compliance:** Every module compiles under `-std=c90` / `/Za` / strict MSVC C89 for legacy and enterprise integration.
- **Automated C99-to-C89 Source Transformation Engine:** Includes `AutoWinMsvcTransform.cmake` and `scripts/transform_sources.py` to rewrite unsupported C99 idioms (such as Variable Length Arrays) at configure time for seamless downstream compilation under MSVC.
- **Mega Library & Modular Architecture:** Consume the entire suite as a single `auto-win-msvc` mega-library (shared `.dll` or static `.lib`), OR fetch individual modules piece-by-piece to minimize binary footprint.
- **Implementation Caveats & Safety:**
  - **Zero `<windows.h>` Namespace Pollution:** Public headers avoid pulling in the Windows API namespace unless explicitly required.
  - **Safe CRT Utilization:** Employs MSVC safe CRT extensions (`sprintf_s`, `_TRUNCATE`, etc.) with proper `#if defined` fallbacks to ensure buffer safety without breaking non-MSVC toolchains.
  - **Customizable Compilation Matrix:** Build configurations support `UNICODE` vs `ANSI`, Multi-threaded vs Single-threaded (`/MT` vs `/MTd`, `/MD` vs `/MDd`), Link-Time Optimization (LTO/IPO), and granular MSVC Runtime Checks (`/RTC1`, `/RTCs`, `/RTCu`).
  - **Strict Helper Exit Codes:** All internal non-void, non-math helper functions return strict `int` exit codes for reliable execution auditing.
  - **Winsock Responsibility:** For `posix-sockets` and network-related modules, consumers remain responsible for calling `WSAStartup()` and `WSACleanup()`.

## 📦 Module Catalog (78 Modules)

The monorepo is organized into 6 platform and standard categories:

### POSIX (43 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `posix-alloca` | `<alloca.h>` | Stack memory allocation wrapper |
| `posix-arpa` | `<arpa/inet.h>` | Internet operations and byte-order conversions |
| `posix-core` | `<unistd.h>`, `<fcntl.h>`, `<sys/stat.h>`, `<sys/types.h>` | Core POSIX file I/O, process APIs, and system constants |
| `posix-dirent` | `<dirent.h>` | Directory stream traversal (`opendir`, `readdir`, `closedir`) |
| `posix-dlfcn` | `<dlfcn.h>` | Dynamic library loading (`dlopen`, `dlsym`, `dlclose`, `dlerror`) |
| `posix-glob` | `<glob.h>` | Pathname pattern matching |
| `posix-inttypes` | `<inttypes.h>` | Exact-width integer formatting and conversions |
| `posix-ipc` | `<sys/ipc.h>`, `<sys/shm.h>`, `<sys/msg.h>`, `<sys/sem.h>` | System V IPC primitives |
| `posix-langinfo` | `<langinfo.h>`, `<nl_types.h>` | Locale and language information constants |
| `posix-libgen` | `<libgen.h>` | Path parsing utilities (`basename`, `dirname`) |
| `posix-libproc` | `<libproc.h>` | Process status and path inspection |
| `posix-libunwind` | `<libunwind.h>` | Call-stack unwinding interface |
| `posix-mman` | `<sys/mman.h>` | Memory management (`mmap`, `munmap`, `mprotect`, `msync`, `madvise`) |
| `posix-netdb` | `<netdb.h>` | Network database operations (`getaddrinfo`, `gethostbyname`) |
| `posix-netinet` | `<netinet/in.h>`, `<netinet/tcp.h>`, `<netinet/ip.h>` | Internet protocol family and socket options |
| `posix-poll` | `<poll.h>`, `<sys/poll.h>` | Polling file descriptor multiplexing |
| `posix-pthread` | `<pthread.h>`, `<sched.h>` | POSIX threads, mutexes, condition variables, rwlocks, and TLS |
| `posix-pwdgrp` | `<pwd.h>`, `<grp.h>` | User and group database access |
| `posix-regex` | `<regex.h>` | POSIX regular expressions (backed by PCRE2) |
| `posix-sched` | `<sched.h>` | Thread and process scheduling and CPU affinity |
| `posix-signal` | `<signal.h>`, `<sys/signal.h>` | POSIX signal emulation (`sigaction`, `kill`, `raise`) |
| `posix-sockets` | `<sys/socket.h>`, `<sys/un.h>` | BSD socket layer with transparent CRT fd translation |
| `posix-spawn` | `<spawn.h>` | Process spawning (`posix_spawn`, `posix_spawnp`) |
| `posix-stat` | `<sys/stat.h>` | File attributes and permission modes |
| `posix-stdatomic` | `<stdatomic.h>` | C11 atomic types and operations polyfill |
| `posix-stdbool` | `<stdbool.h>` | Boolean type and constants |
| `posix-stdint` | `<stdint.h>` | Standard integer types and limits for C89 |
| `posix-strings` | `<strings.h>` | Case-insensitive comparisons and legacy memory ops (`bzero`, `bcopy`) |
| `posix-stropts` | `<stropts.h>` | Stream I/O definitions (`ioctl` command mappings) |
| `posix-sys-ioctl` | `<sys/ioctl.h>` | Device and socket I/O control |
| `posix-sys-mman` | `<sys/mman.h>` | Subsystem redirection header for memory mapping |
| `posix-sys-resource` | `<sys/resource.h>` | Resource usage and limits (`getrlimit`, `setrlimit`, `getrusage`) |
| `posix-sys-select` | `<sys/select.h>` | Synchronous I/O multiplexing (`select`, `pselect`, `fd_set`) |
| `posix-sys-uio` | `<sys/uio.h>` | Vectored I/O operations (`readv`, `writev`) |
| `posix-syslog` | `<syslog.h>` | System logging facility (`openlog`, `syslog`, `closelog`) |
| `posix-termios` | `<termios.h>` | Terminal control and baud rates |
| `posix-time` | `<time.h>`, `<sys/time.h>` | High-resolution timers, `clock_gettime`, `gettimeofday` |
| `posix-times` | `<sys/times.h>` | Process and child execution times (`times`, `struct tms`) |
| `posix-types` | `<sys/types.h>` | Core POSIX data types (`pid_t`, `uid_t`, `gid_t`, `ssize_t`, etc.) |
| `posix-ucontext` | `<ucontext.h>` | User-space context switching (`getcontext`, `setcontext`, `makecontext`) |
| `posix-unwind` | `<unwind.h>` | Stack unwinding types and procedures |
| `posix-utsname` | `<sys/utsname.h>` | System identification (`uname`, `struct utsname`) |
| `posix-wait` | `<sys/wait.h>` | Process termination status macros and waiting (`wait`, `waitpid`) |

### Linux (16 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `linux-backtrace` | `<execinfo.h>` | Stack tracing helper utilities |
| `linux-endian` | `<endian.h>` | Byte swapping macros (`htobe16`, `be32toh`, `le64toh`, etc.) |
| `linux-epoll` | `<sys/epoll.h>` | Scalable I/O event notification (powered by `wepoll`) |
| `linux-execinfo` | `<execinfo.h>` | GNU backtrace functionality (`backtrace`, `backtrace_symbols`) |
| `linux-features` | `<features.h>` | GNU libc compatibility and feature flags |
| `linux-getopt` | `<getopt.h>` | Command-line option parsing (`getopt`, `getopt_long`, `getopt_long_only`) |
| `linux-hv-hloop` | `<linux/hyperv.h>` | Hyper-V and hypercall interface constants |
| `linux-magic` | `<linux/magic.h>` | Filesystem magic constants |
| `linux-rdma` | `<rdma/rdma_cma.h>` | Remote Direct Memory Access (RDMA) type definitions |
| `linux-sys-bitops` | `<sys/bitops.h>` | Bit manipulation operations (`ffs`, `fls`, popcount) |
| `linux-sys-prctl` | `<sys/prctl.h>` | Process operations and name configuration |
| `linux-sys-procfs` | `<sys/procfs.h>` | `/proc` data structures and register definitions |
| `linux-sys-statfs` | `<sys/statfs.h>` | Filesystem capacity and status inspection |
| `linux-sys-syscall` | `<sys/syscall.h>` | Linux syscall number constants |
| `linux-sys-user` | `<sys/user.h>` | User structure and hardware register state |
| `linux-systemd` | `<systemd/sd-daemon.h>` | Systemd daemon notification protocol (`sd_notify`, `sd_booted`) |

### BSD (10 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `bsd-machine-endian` | `<machine/endian.h>` | Architecture-specific endianness macros |
| `bsd-malloc-np` | `<malloc_np.h>` | Non-portable memory allocation (`malloc_size`, `malloc_usable_size`) |
| `bsd-pthread-np` | `<pthread_np.h>` | Non-portable pthread routines (`pthread_set_name_np`, affinity) |
| `bsd-sys-cpuset` | `<sys/cpuset.h>` | FreeBSD CPU affinity sets |
| `bsd-sys-endian` | `<sys/endian.h>` | BSD byte ordering functions |
| `bsd-sys-event` | `<sys/event.h>` | `kqueue` and `kevent` notification polyfills |
| `bsd-sys-file` | `<sys/file.h>` | Advisory file locking (`flock`, `LOCK_SH`, `LOCK_EX`) |
| `bsd-sys-param` | `<sys/param.h>` | System parameters, limits, and min/max macros |
| `bsd-sys-sysctl` | `<sys/sysctl.h>` | System management and hardware inspection via `sysctl` |
| `bsd-vm-param` | `<vm/param.h>` | Virtual memory architecture constants |

### macOS (5 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `macos-availabilitymacros` | `<AvailabilityMacros.h>` | Apple SDK availability and version macros |
| `macos-corefoundation` | `<CoreFoundation/CoreFoundation.h>` | CoreFoundation type definitions and string wrappers |
| `macos-dyld` | `<mach-o/dyld.h>` | Dynamic linker inspection (`_NSGetExecutablePath`) |
| `macos-mach` | `<mach/mach.h>`, `<mach/clock.h>`, `<mach/mach_time.h>` | Mach microkernel clocks, timebase info, and ports |
| `macos-malloc` | `<malloc/malloc.h>` | macOS heap introspection and allocation queries |

### Solaris (3 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `solaris-port` | `<port.h>` | Solaris Event Ports API (`port_create`, `port_associate`) |
| `solaris-sys-byteorder` | `<sys/byteorder.h>` | Solaris byte order conversion macros |
| `solaris-sys-feature-tests` | `<sys/feature_tests.h>` | Solaris compiler and standards feature test macros |

### Haiku OS (1 Module)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `haiku-os` | `<OS.h>`, `<kernel/OS.h>` | Haiku OS kernel timing, thread IDs, and system info |

---

## 🚀 Usage Guide

### Option 1: Native CMake FetchContent

#### Fetching the Entire "Mega Library"
If you want all modules compiled into a single static or shared library:

```cmake
include(FetchContent)
FetchContent_Declare(
    auto-win-msvc
    GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(auto-win-msvc)

# Build configurations (can be toggled before MakeAvailable or via command line):
# set(BUILD_SHARED_LIBS ON)                     # Build .dll instead of .lib
# set(AUTO_WIN_MSVC_UNICODE ON)                 # Build with UNICODE and _UNICODE
# set(AUTO_WIN_MSVC_CRT "Static")               # "Static" (/MT) or "Shared" (/MD)
# set(AUTO_WIN_MSVC_LTO ON)                     # Enable Link-Time Optimization (IPO)
# set(AUTO_WIN_MSVC_RTC "RTC1")                 # Runtime Checks: "Default", "RTC1", "RTCs", "RTCu"
# set(AUTO_WIN_MSVC_SINGLE_THREADED ON)         # Single-threaded CRT / _SINGLE_THREAD

target_link_libraries(your_target PRIVATE auto-win-msvc)
```

#### Overriding Local Source Directory
If consuming `auto-win-msvc` from a local path or submodule in downstream builds:
```bash
cmake -B build -S . -DFETCHCONTENT_SOURCE_DIR_AUTO_WIN_MSVC=/path/to/auto-win-msvc
```

#### Fetching an Individual Module
If you only need a specific subsystem (e.g., `<sys/epoll.h>` via `linux-epoll`):

```cmake
include(FetchContent)
FetchContent_Declare(
    linux-epoll
    GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
    GIT_TAG        master
    SOURCE_SUBDIR  linux-epoll
)
FetchContent_MakeAvailable(linux-epoll)

target_link_libraries(your_target PRIVATE linux-epoll)
```

### Option 2: vcpkg (`vcpkg.json`)

If the ports are integrated into your vcpkg registry, you can pull either the mega package or granular dependencies:

```json
{
  "dependencies": [
    "auto-win-msvc"
  ]
}
```
*Or, for specific modules:*
```json
{
  "dependencies": [
    "linux-epoll",
    "posix-sockets",
    "posix-time"
  ]
}
```

---

## 🏭 Real-World Downstream Integration

`auto-win-msvc` is continuously tested and verified in downstream CI builds of major open-source projects ported to native Windows:

| Project | Upstream / Port | Build System | Highlights |
|---|---|---|---|
| **Valkey** | [`SamuelMarks/valkey-windows`](https://github.com/SamuelMarks/valkey-windows) | MSVC / NMake / Release scripts | High-performance distributed key-value store, NSIS packaging |
| **Redis** | [`SamuelMarks/redis-windows`](https://github.com/SamuelMarks/redis-windows) | MSVC / CTest | Validated against Redis 8+ test suites and NSIS packaging |
| **Rsync** | [`SamuelMarks/rsync`](https://github.com/SamuelMarks/rsync) | CMake + FetchContent | Native POSIX file synchronizer built on Windows with vcpkg |
| **Memcached** | [`SamuelMarks/memcached`](https://github.com/SamuelMarks/memcached) | CMake + FetchContent | Automated C99 VLA AST transformation, executes `memcached.exe -h` and `sizes.exe` |

---

## 🔌 Socket & Pipe Interoperability with Libevent

`auto-win-msvc` provides POSIX file descriptor semantics on top of Windows CRT and Winsock:

- **CRT File Descriptors vs Winsock Sockets:** Functions such as `posix_socket`, `posix_socketpair`, and `pipe` return integer CRT file descriptors wrapping native Winsock `SOCKET` handles (via `_open_osfhandle` with `_O_RDWR | _O_BINARY`).
- **Transparent Handle Translation:** All socket APIs (`posix_send`, `posix_recv`, `posix_sendmsg`, `posix_recvmsg`, `posix_select`, `posix_bind`, `posix_connect`, etc.) use `safe_get_osfhandle` to accept either integer CRT file descriptors or raw Winsock `SOCKET` handles interchangeably.
- **Pipe Implementation:** `pipe()` and `pipe2()` construct an internal bidirectional loopback `posix_socketpair(AF_INET, SOCK_STREAM, 0, fds)`. This ensures pipe file descriptors can be multiplexed via `select()`, `WSASend()`, `WSARecv()`, and external event notification loops like Libevent.
- **Passing Sockets to External Libraries (e.g. Libevent):** If an external library directly expects a native Winsock `SOCKET` (such as `evutil_socket_t`) rather than a CRT integer descriptor, extract the underlying handle using `(SOCKET)_get_osfhandle(fd)`.

---

## 🛠 Supported Environments Summary

| Toolchain / Environment | Build Status | Highlights |
|---|---|---|
| **MSVC 2005** | ✅ Passing | Strict legacy C89 compatibility. |
| **MSVC 2022** | ✅ Passing | Native modern Windows development with strict `/W4 /WX`. |
| **MSVC 2026** | ✅ Passing | Future-proofed against upcoming MSVC compiler releases. |
| **MinGW-w64** | ✅ Passing | Seamless integration for GNU toolchains targeting Windows natively. |
| **Cygwin** | ✅ Passing | Validated against the POSIX-on-Windows standard environment. |
| **AppleClang (macOS)** | ✅ Passing | Strict ISO C90 verification with `-Wall -Wextra -pedantic -Werror`. |
| **GCC & Clang (Linux)** | ✅ Passing | Tested under Linux container environments to ensure pure portable C89. |

---

## 📄 License

This project is multi-licensed under:
- **CC0 1.0 Universal** (`LICENSE-CC0`)
- **Apache License 2.0** (`LICENSE-APACHE`)
- **MIT License** (`LICENSE-MIT`)
