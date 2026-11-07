auto-win-msvc
=============
![Doc Coverage](https://img.shields.io/badge/doc__coverage-100%25-brightgreen)
![Test Coverage](https://img.shields.io/badge/test__coverage-100%25-brightgreen)
[![License](https://img.shields.io/badge/license-CC0%20OR%20Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![MSVC POSIX Monorepo CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml)
[![Multi-Environment CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml)
[![Downstream Integration](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/downstream.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/downstream.yml)

**A Massive, Modular POSIX Compatibility Layer for MSVC, MinGW, Cygwin, and OpenWatcom (for MS-DOS).**

`auto-win-msvc` is a comprehensive suite of C89-compliant libraries designed to provide POSIX, Linux, BSD, macOS, and Solaris user-space APIs natively on Windows. Validated against Microsoft Visual C++ (MSVC 2005, 2022, 2026), MinGW-w64, Cygwin, GCC, and Clang, this monorepo expands across **77 distinct modules** to cover nearly 100% of standard user-space APIs.

## 🏭 Real-World Downstream Integration

`auto-win-msvc` is continuously tested and verified in downstream CI builds of major open-source projects ported to native Windows:

| Project | Upstream / Port | Build System | Highlights |
|---|---|---|---|
| **Valkey** | [`SamuelMarks/valkey-windows`](https://github.com/SamuelMarks/valkey-windows) | MSVC / NMake / Release scripts | High-performance distributed key-value store, NSIS packaging |
| **Redis** | [`SamuelMarks/redis-windows`](https://github.com/SamuelMarks/redis-windows) | MSVC / CTest | Validated against Redis 8+ test suites and NSIS packaging |
| **Rsync** | [`SamuelMarks/rsync`](https://github.com/SamuelMarks/rsync) | CMake + FetchContent | Native POSIX file synchronizer built on Windows with vcpkg |
| **Memcached** | [`SamuelMarks/memcached`](https://github.com/SamuelMarks/memcached) | CMake + FetchContent | Automated C99 VLA AST transformation, executes `memcached.exe -h` and `sizes.exe` |
| **DASH** | [`SamuelMarks/dash-windows`](https://github.com/SamuelMarks/dash-windows) | CMake + FetchContent + CTest | Debian Almquist Shell running natively on Windows with `vfork`/`setjmp` emulation |
| **BusyBox** | [`SamuelMarks/busybox-windows`](https://github.com/SamuelMarks/busybox-windows) | CMake + FetchContent + CTest | Multi-call POSIX swiss-army knife compiled natively with MSVC |
| **s2n-tls** | [`SamuelMarks/s2n-tls`](https://github.com/SamuelMarks/s2n-tls) | CMake + FetchContent + CTest | AWS C99 TLS/SSL protocol implementation transformed and compiled under MSVC |
| **jq** | [`SamuelMarks/jq-windows`](https://github.com/SamuelMarks/jq-windows) | CMake + FetchContent + CTest | Command-line JSON processor transformed to strict C89 for native MSVC and OpenWatcom 32-bit DOS (`JQ.EXE`) |

---

## 🏆 Current Feature Set & Implementation Details

- **Massive Scope (78 Modules):** Modular polyfills spanning POSIX.1-2008, Linux, BSD (FreeBSD/NetBSD/OpenBSD), macOS (Mach), and Solaris headers.
- **Strict ISO C90 (C89) Compliance:** Every module compiles under `-std=c90` / `/Za` / strict MSVC C89 for legacy and enterprise integration.
- **Automated C99-to-C89 Source Transformation Engine:** Includes `AutoWinMsvcTransform.cmake` and `scripts/transform_sources.py` to rewrite unsupported C99 idioms (such as Variable Length Arrays) at configure time for seamless downstream compilation under MSVC.
- **Universal Preprocessor & Error Shims:** Includes root-level headers (`<auto-win-msvc.h>`, `<auto-win-msvc-configure.h>`, `<auto-win-msvc-error.h>`, and `namespace_win_shim.h`) providing drop-in compiler attribute shims, central `auto_win_msvc_error_t` error handling, and `vfork`/`setjmp` emulation.
- **Mega Library & Modular Architecture:** Consume the entire suite as a single `auto-win-msvc` mega-library (shared `.dll` or static `.lib`), OR fetch individual modules piece-by-piece to minimize binary footprint.
- **Implementation Caveats & Safety:**
  - **Zero `<windows.h>` Namespace Pollution:** Public headers avoid pulling in the Windows API namespace unless explicitly required.
  - **Safe CRT Utilization:** Employs MSVC safe CRT extensions (`sprintf_s`, `_TRUNCATE`, etc.) with proper `#if defined` fallbacks to ensure buffer safety without breaking non-MSVC toolchains.
  - **Customizable Compilation Matrix:** Build configurations support `UNICODE` vs `ANSI`, Multi-threaded vs Single-threaded (`/MT` vs `/MTd`, `/MD` vs `/MDd`), Link-Time Optimization (LTO/IPO), AddressSanitizer (ASAN), Code Coverage, and granular MSVC Runtime Checks (`/RTC1`, `/RTCs`, `/RTCu`).
  - **Strict Helper Exit Codes:** All internal non-void, non-math helper functions return strict `int` or `auto_win_msvc_error_t` exit codes for reliable execution auditing.
  - **Winsock Responsibility:** For `posix-sockets` and network-related modules, consumers remain responsible for calling `WSAStartup()` and `WSACleanup()` (or calling `auto_win_msvc_ensure_wsastartup()`).

## 📦 Module Catalog (77 Modules)

The monorepo is organized into 5 platform and standard categories:

### POSIX (44 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `posix-alloca` | `<alloca.h>` | Stack memory allocation wrapper |
| `posix-arpa` | `<arpa/inet.h>`, `<arpa/telnet.h>`, `<posix-arpa-inet.h>` | Internet operations, telnet constants, and byte-order conversions |
| `posix-core` | `<unistd.h>`, `<paths.h>`, `<sysexits.h>`, `<posix-core.h>` | Core POSIX file I/O, process APIs, and system constants |
| `posix-dirent` | `<dirent.h>`, `<posix-dirent.h>` | Directory stream traversal (`opendir`, `readdir`, `closedir`) |
| `posix-dlfcn` | `<dlfcn.h>`, `<posix-dlfcn.h>` | Dynamic library loading (`dlopen`, `dlsym`, `dlclose`, `dlerror`) |
| `posix-fcntl` | `<fcntl.h>` | File control options and descriptor manipulation (`fcntl`, `open`, `openat`) |
| `posix-glob` | `<glob.h>`, `<fnmatch.h>`, `<posix-glob.h>` | Pathname pattern and filename matching |
| `posix-inttypes` | `<inttypes.h>`, `<posix-inttypes.h>` | Exact-width integer formatting and conversions |
| `posix-ipc` | `<posix-ipc.h>` (`<sys/ipc.h>`, `<sys/shm.h>`, `<sys/msg.h>`, `<sys/sem.h>`) | System V IPC primitives |
| `posix-langinfo` | `<langinfo.h>`, `<posix-langinfo.h>` | Locale and language information constants (`nl_langinfo`) |
| `posix-libgen` | `<libgen.h>`, `<posix-libgen.h>` | Path parsing utilities (`basename`, `dirname`) |
| `posix-libproc` | `<posix-libproc.h>` | Process status and path inspection |
| `posix-mman` | `<posix-mman.h>` (`<sys/mman.h>`) | Memory management (`mmap`, `munmap`, `mprotect`, `msync`, `madvise`) |
| `posix-mqueue` | `<mqueue.h>` | POSIX message queues (`mq_open`, `mq_send`, `mq_receive`, `mq_unlink`) |
| `posix-netdb` | `<netdb.h>`, `<resolv.h>`, `<posix-netdb.h>` | Network database and DNS resolver operations (`getaddrinfo`, `gethostbyname`) |
| `posix-netinet` | `<netinet/in.h>`, `<netinet/tcp.h>`, `<netinet/ip.h>`, `<netinet/ip_icmp.h>`, `<netinet/icmp6.h>` | Internet protocol family, ICMP, and TCP socket options |
| `posix-poll` | `<poll.h>`, `<posix-poll.h>` | Polling file descriptor multiplexing (`poll`, `ppoll`) |
| `posix-pthread` | `<pthread.h>`, `<posix-pthread.h>` | POSIX threads, mutexes, condition variables, rwlocks, barriers, and TLS |
| `posix-pwdgrp` | `<pwd.h>`, `<grp.h>`, `<posix-pwdgrp.h>` | User and group database access (`getpwnam`, `getgrgid`) |
| `posix-regex` | `<regex.h>`, `<posix-regex.h>` | POSIX regular expressions (backed by PCRE2) |
| `posix-sched` | `<posix-sched.h>` (`<sched.h>`) | Thread and process scheduling and CPU affinity |
| `posix-semaphore` | `<semaphore.h>` | POSIX named and unnamed semaphores (`sem_init`, `sem_wait`, `sem_post`) |
| `posix-signal` | `<signal.h>`, `<posix-signal.h>`, `<posix-signal-ext.h>` | POSIX signal emulation (`sigaction`, `kill`, `sigprocmask`, `sigsuspend`) |
| `posix-sockets` | `<sys/socket.h>`, `<sys/un.h>`, `<net/if.h>`, `<ifaddrs.h>`, `<posix-sockets.h>` | BSD socket layer with transparent CRT fd translation and interface inspection |
| `posix-spawn` | `<spawn.h>`, `<posix-spawn.h>` | Process spawning (`posix_spawn`, `posix_spawnp`) |
| `posix-stat` | `<sys/stat.h>`, `<sys/sysmacros.h>`, `<posix-stat.h>` | File attributes, permission modes, and device number macros |
| `posix-stdatomic` | `<posix-stdatomic.h>` (`<stdatomic.h>`) | C11 atomic types and operations polyfill |
| `posix-stdbool` | `<stdbool.h>`, `<posix-stdbool.h>` | Boolean type and constants |
| `posix-stdint` | `<stdint.h>`, `<posix-stdint.h>` | Standard integer types and limits for C89 |
| `posix-strings` | `<strings.h>`, `<posix-strings.h>` | Case-insensitive comparisons and legacy memory ops (`bzero`, `bcopy`, `ffs`) |
| `posix-stropts` | `<stropts.h>` | Stream I/O definitions (`ioctl` command mappings) |
| `posix-sys-ioctl` | `<sys/ioctl.h>`, `<posix-sys-ioctl.h>` | Device and socket I/O control |
| `posix-sys-mman` | `<sys/mman.h>` | Subsystem redirection header for memory mapping |
| `posix-sys-resource` | `<sys/resource.h>`, `<posix-sys-resource.h>` | Resource usage and limits (`getrlimit`, `setrlimit`, `getrusage`) |
| `posix-sys-select` | `<sys/select.h>`, `<posix-sys-select.h>` | Synchronous I/O multiplexing (`select`, `pselect`, `fd_set`) |
| `posix-sys-uio` | `<sys/uio.h>`, `<posix-sys-uio.h>` | Vectored I/O operations (`readv`, `writev`) |
| `posix-syslog` | `<syslog.h>`, `<posix-syslog.h>` | System logging facility (`openlog`, `syslog`, `closelog`) |
| `posix-termios` | `<termios.h>`, `<posix-termios.h>` | Terminal control and baud rates |
| `posix-time` | `<sys/time.h>`, `<posix-time.h>` | High-resolution timers, `clock_gettime`, `gettimeofday`, `nanosleep` |
| `posix-times` | `<sys/times.h>`, `<posix-times.h>` | Process and child execution times (`times`, `struct tms`) |
| `posix-types` | `<sys/types.h>`, `<posix-types.h>` | Core POSIX data types (`pid_t`, `uid_t`, `gid_t`, `ssize_t`, etc.) |
| `posix-ucontext` | `<posix-ucontext.h>` (`<ucontext.h>`) | User-space context switching (`getcontext`, `setcontext`, `makecontext`) |
| `posix-utsname` | `<sys/utsname.h>`, `<posix-utsname.h>` | System identification (`uname`, `struct utsname`) |
| `posix-wait` | `<sys/wait.h>`, `<posix-wait.h>` | Process termination status macros and waiting (`wait`, `waitpid`) |

### Linux (16 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `linux-endian` | `<endian.h>`, `<byteswap.h>`, `<linux-endian.h>` | Byte swapping macros (`htobe16`, `be32toh`, `bswap_32`, etc.) |
| `linux-epoll` | `<sys/epoll.h>`, `<linux-epoll.h>` | Scalable I/O event notification (powered by `wepoll`) |
| `linux-err` | `<err.h>` | BSD/Linux formatted error and warning messages (`err`, `errx`, `warn`, `warnx`) |
| `linux-execinfo` | `<linux-execinfo.h>` (`<execinfo.h>`) | GNU backtrace functionality (`backtrace`, `backtrace_symbols`) |
| `linux-features` | `<features.h>`, `<linux-features.h>` | GNU libc compatibility and feature test flags |
| `linux-getopt` | `<linux-getopt.h>` (`<getopt.h>`) | Command-line option parsing (`getopt`, `getopt_long`, `getopt_long_only`) |
| `linux-magic` | `<linux/magic.h>` | Filesystem magic constants |
| `linux-sys-bitops` | `<linux-sys-bitops.h>` (`<sys/bitops.h>`) | Bit manipulation operations (`ffs`, `fls`, popcount) |
| `linux-sys-eventfd` | `<sys/eventfd.h>` | Event wait/notify descriptor mechanism (`eventfd`) |
| `linux-sys-prctl` | `<linux-sys-prctl.h>` (`<sys/prctl.h>`) | Process operations and thread name configuration |
| `linux-sys-procfs` | `<sys/sysinfo.h>`, `<linux-sys-procfs.h>` | System statistics (`sysinfo`) and `/proc` data structures |
| `linux-sys-sendfile` | `<sys/sendfile.h>` | Zero-copy data transfer between file descriptors (`sendfile`) |
| `linux-sys-statfs` | `<sys/statfs.h>`, `<sys/statvfs.h>`, `<mntent.h>`, `<linux-sys-statfs.h>` | Filesystem capacity, mount table parsing (`getmntent`), and status inspection |
| `linux-sys-syscall` | `<linux-sys-syscall.h>` (`<sys/syscall.h>`) | Linux syscall number constants and `syscall` interface |
| `linux-sys-timerfd` | `<sys/timerfd.h>` | Timer expiration notification descriptors (`timerfd_create`, `timerfd_settime`) |
| `linux-sys-user` | `<linux-sys-user.h>` (`<sys/user.h>`) | User structure and hardware register state definitions |

### BSD (14 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `bsd-machine-endian` | `<machine/endian.h>`, `<bsd-machine-endian.h>` | Architecture-specific endianness macros |
| `bsd-malloc-np` | `<bsd-malloc-np.h>` (`<malloc_np.h>`) | Non-portable memory allocation (`malloc_size`, `malloc_usable_size`) |
| `bsd-pthread-np` | `<bsd-pthread-np.h>` (`<pthread_np.h>`) | Non-portable pthread routines (`pthread_set_name_np`, affinity) |
| `bsd-stdlib` | `<bsd/stdlib.h>` | BSD standard library extensions (`reallocarray`) |
| `bsd-string` | `<bsd/string.h>` | Safe string operations (`strlcpy`, `strlcat`, `strcasestr`, `strsep`, `strverscmp`) |
| `bsd-sys-cdefs` | `<sys/cdefs.h>` | BSD C compiler and preprocessor macro definitions |
| `bsd-sys-cpuset` | `<bsd-sys-cpuset.h>` (`<sys/cpuset.h>`) | FreeBSD CPU affinity sets (`cpuset_getaffinity`, `cpuset_setaffinity`) |
| `bsd-sys-endian` | `<bsd-sys-endian.h>` (`<sys/endian.h>`) | BSD byte ordering and encoding functions |
| `bsd-sys-event` | `<bsd-sys-event.h>` (`<sys/event.h>`) | `kqueue` and `kevent` notification polyfills |
| `bsd-sys-file` | `<bsd-sys-file.h>` (`<sys/file.h>`) | Advisory file locking (`flock`, `LOCK_SH`, `LOCK_EX`) |
| `bsd-sys-param` | `<sys/param.h>`, `<bsd-sys-param.h>` | System parameters, limits, and min/max macros |
| `bsd-sys-queue` | `<sys/queue.h>` | BSD queue, tail-queue, and linked list data structure macros |
| `bsd-sys-sysctl` | `<bsd-sys-sysctl.h>` (`<sys/sysctl.h>`) | System management and hardware inspection via `sysctl` / `sysctlbyname` |
| `bsd-sys-tree` | `<sys/tree.h>` | BSD splay and red-black tree data structure macros |

### macOS (1 Module)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `macos-mach` | `<mach/mach.h>`, `<mach/mach_init.h>`, `<mach/task.h>` | Mach microkernel clocks, timebase info, tasks, and ports |

### Solaris (3 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `solaris-port` | `<solaris-port.h>` (`<port.h>`) | Solaris Event Ports API (`port_create`, `port_associate`, `port_get`) |
| `solaris-sys-byteorder` | `<solaris-sys-byteorder.h>` (`<sys/byteorder.h>`) | Solaris byte order conversion macros |
| `solaris-sys-feature-tests` | `<solaris-sys-feature-tests.h>` (`<sys/feature_tests.h>`) | Solaris compiler and standards feature test macros |

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
# set(AUTO_WIN_MSVC_BUILD_TESTING OFF)          # Toggle building the test suite
# set(AUTO_WIN_MSVC_COVERAGE ON)                # Enable code coverage reporting
# set(AUTO_WIN_MSVC_ASAN ON)                    # Enable AddressSanitizer (ASAN) on modern MSVC

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
| **OpenWatcom v2 (MS-DOS 1.25, 2.0, 4.0)** | ✅ Passing | Protected-Mode (`DOS/32A` / `DOS/4GW`, `wcl386`) & Real-Mode (`wcl`) DOS with single-threaded TLS and POSIX shims, verified via `vagrant/dos/Vagrantfile`. |

---

## 💾 MS-DOS 1.25, 2.0, and 4.0 Vagrant CI (`vagrant/dos/Vagrantfile`)

`auto-win-msvc` includes a multi-machine Vagrant environment in [`vagrant/dos/Vagrantfile`](vagrant/dos/Vagrantfile) that boots **Microsoft MS-DOS 1.25, 2.0, and 4.0** (`bento/msdos-1.25`, `bento/msdos-2.0`, `bento/msdos-4.0`), provisions **OpenWatcom v2**, cross-compiles `auto-win-msvc` with `Watcom WMake`, uploads the DOS `.EXE` test binaries over the COM1 serial/telnet bridge (`SERGET.COM`), and executes the CTest suite inside the live MS-DOS virtual machines:

```sh
cd vagrant/dos

# Boot and run the OpenWatcom + CMake + CTest suite inside MS-DOS 1.25, 2.0, and 4.0
vagrant up msdos-1.25 --provider=qemu
vagrant up msdos-2.0 --provider=qemu
vagrant up msdos-4.0 --provider=qemu

# Re-run the CMake + CTest verification on a running MS-DOS VM
vagrant provision msdos-4.0
```

---

## 📜 License

Licensed under any of:

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or
  <https://apache.org/licenses/LICENSE-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)
- CC0 1.0 Universal ([LICENSE-CC0](LICENSE-CC0) or
  <https://creativecommons.org/publicdomain/zero/1.0/>)

at your option.
