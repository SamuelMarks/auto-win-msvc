auto-win-msvc
=============
![Doc Coverage](https://img.shields.io/badge/doc__coverage-100%25-brightgreen)
![Test Coverage](https://img.shields.io/badge/test__coverage-100%25-brightgreen)
[![License](https://img.shields.io/badge/license-CC0%20OR%20Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![MSVC POSIX Monorepo CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml)
[![Multi-Environment CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml)
[![Downstream Integration](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/downstream.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/downstream.yml)

**A Massive, Modular POSIX, BSD, and Linux Compatibility Layer for MSVC, MinGW, Cygwin, and OpenWatcom (for MS-DOS).**

`auto-win-msvc` is a comprehensive suite of C89-compliant libraries designed to provide POSIX, Linux, BSD (FreeBSD/NetBSD/OpenBSD), macOS, and Solaris user-space and system inspection APIs natively on Windows and MS-DOS. Validated against Microsoft Visual C++ (MSVC 2005, 2022, 2026), MinGW-w64, Cygwin, GCC, Clang, and OpenWatcom v2, this monorepo spans **91 distinct modules** to cover standard and BSD/Linux user-space APIs without modifying upstream sources.

## 🏭 Real-World Downstream Integration

`auto-win-msvc` powers native Windows and MS-DOS ports of major open-source POSIX, Linux, and FreeBSD projects, with automated downstream CI verification in [`.github/workflows/downstream.yml`](.github/workflows/downstream.yml) and [`scripts/test_downstream.py`](scripts/test_downstream.py):

| Project | Upstream / Port | Build System | Highlights |
|---|---|---|---|
| **Valkey** | [`SamuelMarks/valkey-windows`](https://github.com/SamuelMarks/valkey-windows) | MSVC / NMake / Release scripts | CI-verified; high-performance distributed key-value store, NSIS packaging |
| **Redis** | [`SamuelMarks/redis-windows`](https://github.com/SamuelMarks/redis-windows) | MSVC / CTest | CI-verified; validated against Redis 8+ internal test suites and NSIS packaging |
| **Rsync** | [`SamuelMarks/rsync`](https://github.com/SamuelMarks/rsync) | CMake + FetchContent | CI-verified; native POSIX file synchronizer built on Windows with vcpkg |
| **Memcached** | [`SamuelMarks/memcached`](https://github.com/SamuelMarks/memcached) | CMake + FetchContent | CI-verified; automated C99 VLA AST transformation, executes `memcached.exe -h` and `sizes.exe` |
| **DASH** | [`SamuelMarks/dash-windows`](https://github.com/SamuelMarks/dash-windows) | CMake + FetchContent + CTest | CI-verified; Debian Almquist Shell running natively on Windows with `vfork`/`setjmp` emulation |
| **BusyBox** | [`SamuelMarks/busybox-windows`](https://github.com/SamuelMarks/busybox-windows) | CMake + FetchContent + CTest | CI-verified; multi-call POSIX swiss-army knife compiled natively with MSVC and OpenWatcom DOS |
| **jq** | [`cmake/jq-port`](cmake/jq-port) / [`SamuelMarks/jq-windows`](https://github.com/SamuelMarks/jq-windows) | CMake + FetchContent + CTest | CI-verified (`cmake/jq-port`); JSON processor transformed to C89 for MSVC and OpenWatcom 32-bit DOS (`JQ.EXE`) |
| **s2n-tls** | [`SamuelMarks/s2n-tls`](https://github.com/SamuelMarks/s2n-tls) | CMake + FetchContent + CTest | AWS C99 TLS/SSL protocol implementation transformed and compiled under MSVC |
| **FreeBSD `usr.bin`** | [`SamuelMarks/freebsd-usr-bin-windows`](https://github.com/SamuelMarks/freebsd-usr-bin-windows) | CMake + FetchContent + CTest | Unmodified FreeBSD userland utilities (Capsicum/Casper, KVM/procstat, `libutil`, SunRPC/TI-RPC, device ioctls) on MSVC & DOS |
| **Toybox** | [`SamuelMarks/toybox-windows`](https://github.com/SamuelMarks/toybox-windows) | CMake + FetchContent + CTest | Linux command-line utilities transformed from C99/GNU C to strict C89 for MSVC & OpenWatcom DOS |

---

## 🏆 Current Feature Set & Implementation Details

- **Massive Scope (91 Modules):** Modular polyfills spanning POSIX.1-2008 (45), Linux (16), BSD/FreeBSD (26), macOS Mach (1), and Solaris (3) headers.
- **Strict ISO C90 (C89) Compliance:** Every module compiles under `-std=c90` / `/Za` / strict MSVC C89 and OpenWatcom v2 for legacy and enterprise integration.
- **Automated C99/C11/GNU-to-C89 Source Transformation Engine:** Includes [`cmake/AutoWinMsvcTransform.cmake`](cmake/AutoWinMsvcTransform.cmake) and [`scripts/transform_sources.py`](scripts/transform_sources.py) to rewrite unsupported C99, C11, and GNU C constructs at configure/build time—including Variable Length Arrays (VLAs), compound literals, local designated struct/array initializers, C99 array parameter qualifiers (`static`, `restrict`, `__min_size`), `_Noreturn` / C11 keywords, GNU Elvis `?:` and `case` ranges, scalar `const` integer folding, 1-arg BSD `setmode`/`getmode` disambiguation, `__FBSDID` / linker-set normalization, cycle-safe recursive local `#include` transformation, and `--rename-main` multicall entry-point renaming.
- **Universal Preprocessor & Error Shims:** Includes root-level headers (`<auto-win-msvc.h>`, `<auto-win-msvc-configure.h>`, `<auto-win-msvc-error.h>`, and `namespace_win_shim.h`) providing drop-in compiler attribute shims, automatic `<bsd/stdlib.h>` and `<bsd/string.h>` exposure, central `auto_win_msvc_error_t` error handling, and `vfork`/`setjmp` emulation.
- **Mega Library & Modular Architecture:** Consume the entire suite as a single `auto-win-msvc` mega-library (shared `.dll` or static `.lib`, with unified header staging in `${CMAKE_BINARY_DIR}/posix_includes`), OR fetch individual modules piece-by-piece to minimize binary footprint.
- **Implementation Caveats & Safety:**
  - **Zero `<windows.h>` Namespace Pollution:** Public headers avoid pulling in the Windows API namespace unless explicitly required.
  - **Safe CRT Utilization:** Employs MSVC safe CRT extensions (`sprintf_s`, `_TRUNCATE`, etc.) with proper `#if defined` fallbacks to ensure buffer safety without breaking non-MSVC toolchains.
  - **Customizable Compilation Matrix:** Build configurations support `UNICODE` vs `ANSI`, Multi-threaded vs Single-threaded (`/MT` vs `/MTd`, `/MD` vs `/MDd`), Link-Time Optimization (LTO/IPO), AddressSanitizer (ASAN), Code Coverage, and granular MSVC Runtime Checks (`/RTC1`, `/RTCs`, `/RTCu`).
  - **Strict Helper Exit Codes:** All internal non-void, non-math helper functions return strict `int` or `auto_win_msvc_error_t` exit codes for reliable execution auditing.
  - **Winsock Responsibility:** For `posix-sockets` and network-related modules, consumers remain responsible for calling `WSAStartup()` and `WSACleanup()` (or calling `auto_win_msvc_ensure_wsastartup()`).

## 📦 Module Catalog (91 Modules)

The monorepo is organized into 5 platform and standard categories:

### POSIX (45 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `posix-alloca` | `<alloca.h>` | Stack memory allocation wrapper |
| `posix-arpa` | `<arpa/inet.h>`, `<arpa/telnet.h>`, `<posix-arpa-inet.h>` | Internet operations, telnet constants, and byte-order conversions |
| `posix-core` | `<unistd.h>`, `<paths.h>`, `<sysexits.h>`, `<posix-core.h>` | Core POSIX file I/O, process APIs, `_PATH_*` defaults, and system constants |
| `posix-dirent` | `<dirent.h>`, `<posix-dirent.h>` | Directory stream traversal (`opendir`, `readdir`, `closedir`, `scandir`, `fdscandir`) |
| `posix-dlfcn` | `<dlfcn.h>`, `<posix-dlfcn.h>` | Dynamic library loading (`dlopen`, `dlsym`, `dlclose`, `dlerror`) |
| `posix-fcntl` | `<fcntl.h>` | File control options and descriptor manipulation (`fcntl`, `open`, `openat`) |
| `posix-glob` | `<glob.h>`, `<fnmatch.h>`, `<posix-glob.h>` | Pathname pattern and filename matching (`glob`, `fnmatch`, BSD/GNU flags) |
| `posix-inttypes` | `<inttypes.h>`, `<posix-inttypes.h>` | Exact-width integer formatting and conversions |
| `posix-ipc` | `<posix-ipc.h>` (`<sys/ipc.h>`, `<sys/shm.h>`, `<sys/msg.h>`, `<sys/sem.h>`) | System V IPC primitives |
| `posix-langinfo` | `<langinfo.h>`, `<posix-langinfo.h>` | Locale and language information constants (`nl_langinfo`) |
| `posix-libgen` | `<libgen.h>`, `<posix-libgen.h>` | Path parsing utilities (`basename`, `dirname`) |
| `posix-libproc` | `<posix-libproc.h>` | Process status and path inspection |
| `posix-mman` | `<posix-mman.h>` (`<sys/mman.h>`) | Memory management (`mmap`, `munmap`, `mprotect`, `msync`, `madvise`, `memfd_create`) |
| `posix-mqueue` | `<mqueue.h>` | POSIX message queues (`mq_open`, `mq_send`, `mq_receive`, `mq_unlink`) |
| `posix-netdb` | `<netdb.h>`, `<resolv.h>`, `<posix-netdb.h>` | Network database and DNS resolver operations (`getaddrinfo`, `gethostbyname`) |
| `posix-netinet` | `<netinet/in.h>`, `<netinet/in_systm.h>`, `<netinet/in_pcb.h>`, `<netinet/ip.h>`, `<netinet/ip6.h>`, `<netinet/ip_icmp.h>`, `<netinet/icmp6.h>`, `<netinet/tcp.h>`, `<netinet/tcp_fsm.h>`, `<netinet/tcp_seq.h>`, `<netinet/tcp_var.h>`, `<netinet/udp.h>`, `<netinet/udp_var.h>`, `<netinet/sctp.h>`, `<posix-netinet-in.h>`, `<posix-netinet-tcp.h>` | Internet protocol family, IPv4/IPv6, ICMP, TCP/UDP/SCTP headers, and socket options |
| `posix-nl-types` | `<nl_types.h>` | POSIX NLS message catalog operations (`catopen`, `catgets`, `catclose`) |
| `posix-poll` | `<poll.h>`, `<posix-poll.h>` | Polling file descriptor multiplexing (`poll`, `ppoll`) |
| `posix-pthread` | `<pthread.h>`, `<posix-pthread.h>` | POSIX threads, mutexes, condition variables, rwlocks, barriers, spinlocks, and TLS |
| `posix-pwdgrp` | `<pwd.h>`, `<grp.h>`, `<posix-pwdgrp.h>` | User/group database access and BSD caches (`getpwnam`, `getgrgid`, `user_from_uid`, `group_from_gid`, `pwcache_*`) |
| `posix-regex` | `<regex.h>`, `<posix-regex.h>` | POSIX regular expressions (backed by PCRE2 or native DOS/C89 fallback) |
| `posix-sched` | `<posix-sched.h>` (`<sched.h>`) | Thread and process scheduling and CPU affinity |
| `posix-semaphore` | `<semaphore.h>` | POSIX named and unnamed semaphores (`sem_init`, `sem_open`, `sem_wait`, `sem_post`) |
| `posix-signal` | `<signal.h>`, `<posix-signal.h>`, `<posix-signal-ext.h>` | POSIX signal emulation (`sigaction`, `kill`, `sigprocmask`, `sigsuspend`) |
| `posix-sockets` | `<sys/socket.h>`, `<sys/socketvar.h>`, `<sys/domain.h>`, `<sys/protosw.h>`, `<sys/un.h>`, `<sys/unpcb.h>`, `<net/if.h>`, `<net/if_arp.h>`, `<net/if_dl.h>`, `<net/if_types.h>`, `<net/ethernet.h>`, `<net/bpf.h>`, `<net/route.h>`, `<netlink/*.h>`, `<ifaddrs.h>`, `<posix-sockets.h>`, `<posix-sys-un.h>` | BSD socket layer with transparent CRT fd translation, routing/BPF/Ethernet headers, FreeBSD Netlink SNL shims, and interface inspection |
| `posix-spawn` | `<spawn.h>`, `<posix-spawn.h>` | Process spawning (`posix_spawn`, `posix_spawnp`) |
| `posix-stat` | `<sys/stat.h>`, `<sys/sysmacros.h>`, `<posix-stat.h>` | File attributes, permission modes, `mkfifo`, BSD file flags (`chflags`, `UF_*`, `SF_*`), and device macros |
| `posix-stdatomic` | `<posix-stdatomic.h>` (`<stdatomic.h>`) | C11 atomic types and operations polyfill |
| `posix-stdbool` | `<stdbool.h>`, `<posix-stdbool.h>` | Boolean type and constants |
| `posix-stdint` | `<stdint.h>`, `<stdckdint.h>`, `<posix-stdint.h>` | Standard integer types, limits, and C23 checked integer arithmetic (`ckd_add`, `ckd_sub`, `ckd_mul`) |
| `posix-strings` | `<strings.h>`, `<posix-strings.h>` | Case-insensitive comparisons and legacy memory ops (`bzero`, `bcopy`, `ffs`) |
| `posix-stropts` | `<stropts.h>` | Stream I/O definitions (`ioctl` command mappings) |
| `posix-sys-ioctl` | `<sys/ioctl.h>`, `<posix-sys-ioctl.h>` | Device and socket I/O control |
| `posix-sys-mman` | `<sys/mman.h>` | Subsystem redirection header for memory mapping |
| `posix-sys-resource` | `<sys/resource.h>`, `<posix-sys-resource.h>` | Resource usage and limits (`getrlimit`, `setrlimit`, `getrusage`) |
| `posix-sys-select` | `<sys/select.h>`, `<posix-sys-select.h>` | Synchronous I/O multiplexing (`select`, `pselect`, `fd_set`) |
| `posix-sys-uio` | `<sys/uio.h>`, `<posix-sys-uio.h>` | Vectored I/O operations (`readv`, `writev`) |
| `posix-syslog` | `<syslog.h>`, `<sys/syslog.h>`, `<posix-syslog.h>` | System logging facility (`openlog`, `syslog`, `vsyslog`, `closelog`, `SYSLOG_NAMES`) |
| `posix-termios` | `<termios.h>`, `<posix-termios.h>` | Terminal control and baud rates |
| `posix-time` | `<sys/time.h>`, `<utime.h>`, `<posix-time.h>` | High-resolution timers, `clock_gettime`, `gettimeofday`, `nanosleep`, `utime`, and `timespec` macros |
| `posix-times` | `<sys/times.h>`, `<posix-times.h>` | Process and child execution times (`times`, `struct tms`) |
| `posix-types` | `<sys/types.h>`, `<posix-types.h>` | Core POSIX and BSD kernel/shorthand data types (`pid_t`, `uid_t`, `ssize_t`, `u_int`, `quad_t`, `caddr_t`, `register_t`) |
| `posix-ucontext` | `<posix-ucontext.h>` (`<ucontext.h>`) | User-space context switching (`getcontext`, `setcontext`, `makecontext`) |
| `posix-utsname` | `<sys/utsname.h>`, `<posix-utsname.h>` | System identification (`uname`, `struct utsname`) |
| `posix-wait` | `<sys/wait.h>`, `<posix-wait.h>` | Process termination status macros and waiting (`wait`, `waitpid`, `wait4`) |

### Linux (16 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `linux-endian` | `<endian.h>`, `<byteswap.h>`, `<linux-endian.h>` | Byte swapping macros (`htobe16`, `be32toh`, `bswap_32`, etc.) |
| `linux-epoll` | `<sys/epoll.h>`, `<linux-epoll.h>` | Scalable I/O event notification (powered by `wepoll`) |
| `linux-err` | `<err.h>` | BSD/Linux formatted error and warning messages (`err`, `errc`, `errx`, `warn`, `warnc`, `warnx`, `err_set_*`) |
| `linux-execinfo` | `<linux-execinfo.h>` (`<execinfo.h>`) | GNU backtrace functionality (`backtrace`, `backtrace_symbols`) |
| `linux-features` | `<features.h>`, `<linux-features.h>` | GNU libc compatibility and feature test flags |
| `linux-getopt` | `<getopt.h>`, `<linux-getopt.h>` | Command-line option parsing (`getopt`, `getopt_long`, `getopt_long_only`) |
| `linux-magic` | `<linux/magic.h>` | Filesystem magic constants |
| `linux-sys-bitops` | `<linux-sys-bitops.h>` (`<sys/bitops.h>`) | Bit manipulation operations (`ffs`, `fls`, popcount) |
| `linux-sys-eventfd` | `<sys/eventfd.h>` | Event wait/notify descriptor mechanism (`eventfd`) |
| `linux-sys-prctl` | `<linux-sys-prctl.h>` (`<sys/prctl.h>`) | Process operations and thread name configuration |
| `linux-sys-procfs` | `<sys/sysinfo.h>`, `<linux-sys-procfs.h>` | System statistics (`sysinfo`) and `/proc` data structures |
| `linux-sys-sendfile` | `<sys/sendfile.h>` | Zero-copy data transfer between file descriptors (`sendfile`) |
| `linux-sys-statfs` | `<sys/statfs.h>`, `<sys/statvfs.h>`, `<mntent.h>`, `<linux-sys-statfs.h>` | Filesystem capacity, mount table parsing (`getmntent`), and status inspection |
| `linux-sys-syscall` | `<linux-sys-syscall.h>` (`<sys/syscall.h>`) | Linux syscall number constants and `syscall` interface |
| `linux-sys-timerfd` | `<sys/timerfd.h>` | Timer expiration notification descriptors (`timerfd_create`, `timerfd_settime`) |
| `linux-sys-user` | `<linux-sys-user.h>` (`<sys/user.h>`) | Linux user structure and hardware register state definitions |

### BSD (26 Modules)
| Module | Primary Headers Provided | Description |
|---|---|---|
| `bsd-bitstring` | `<bitstring.h>` | BSD bit-string allocation and manipulation macros (`bit_alloc`, `bit_set`, `bit_clear`, `bit_ffs`, `bit_ffc`) |
| `bsd-capsicum` | `<sys/capsicum.h>`, `<sys/caprights.h>`, `<capsicum_helpers.h>`, `<libcasper.h>`, `<casper/cap_*.h>`, `<sys/procdesc.h>` | FreeBSD Capsicum capability mode, Casper service shims (`fileargs`, `cap_pwd`, `cap_grp`, `cap_net`, `cap_sysctl`, `cap_syslog`), and process descriptors (`pdfork`, `pdwait4`) |
| `bsd-db` | `<db.h>`, `<ndbm.h>` | BSD 1.85 database (`dbopen`, `DB_BTREE`, `DB_HASH`, `DB_RECNO`) and `ndbm` key-value API |
| `bsd-dev-headers` | `<ar.h>`, `<bluetooth.h>`, `<sdp.h>`, `<elf.h>`, `<hesiod.h>`, `<sysdecode.h>`, `<usbhid.h>`, `<xlocale.h>`, `<cam/*.h>`, `<dev/filemon/filemon.h>`, `<dev/speaker/speaker.h>`, `<dev/usb/usbhid.h>`, `<machine/cpu.h>`, `<machine/elf.h>`, `<netipsec/ipsec.h>`, `<protocols/talkd.h>`, `<sys/*.h>` | FreeBSD device, CAM/SCSI, ELF, USB HID, OSS audio/speaker, tape (`mtio`), tty/console ioctl, backlight, Bluetooth/SDP, Hesiod, IPsec, MAC, KLD, and `sysdecode` headers/shims |
| `bsd-fts` | `<fts.h>` | BSD file hierarchy stream traversal (`fts_open`, `fts_read`, `fts_children`, `fts_set`, `fts_close`) |
| `bsd-kvm-procstat` | `<kvm.h>`, `<libprocstat.h>`, `<devstat.h>`, `<memstat.h>`, `<nlist.h>`, `<fstab.h>`, `<jail.h>`, `<sys/jail.h>`, `<sys/mount.h>`, `<sys/user.h>`, `<sys/proc.h>`, `<sys/procctl.h>`, `<sys/ptrace.h>`, `<sys/vmmeter.h>`, `<sys/acl.h>`, `<sys/extattr.h>`, `<sys/rctl.h>`, `<ufs/ufs/quota.h>`, `<vm/vm.h>` | FreeBSD kernel virtual memory (`kvm_*`), `libprocstat`, `devstat`/`memstat`, BSD `statfs`/`getmntinfo`, jail, quota, ACL, `extattr`, `rctl`, and `procctl` APIs |
| `bsd-libutil` | `<libutil.h>`, `<login_cap.h>` | FreeBSD `libutil` utilities (`humanize_number`, `expand_number`, `hexdump`, `pidfile_*`, `flopen`, `kinfo_get*`, `login_cap`, `pw_*`, `quota_*`, `uu_lock`) |
| `bsd-machine-endian` | `<machine/endian.h>`, `<bsd-machine-endian.h>` | Architecture-specific endianness macros |
| `bsd-malloc-np` | `<malloc_np.h>`, `<bsd-malloc-np.h>` | Non-portable memory allocation (`malloc_size`, `malloc_usable_size`) |
| `bsd-pthread-np` | `<pthread_np.h>`, `<bsd-pthread-np.h>` | Non-portable pthread routines (`pthread_set_name_np`, affinity) |
| `bsd-rpc` | `<netconfig.h>`, `<rpc/rpc.h>`, `<rpc/xdr.h>`, `<rpc/clnt.h>`, `<rpc/auth.h>`, `<rpc/types.h>`, `<rpc/nettype.h>`, `<rpc/rpc_com.h>`, `<rpc/pmap_clnt.h>`, `<rpc/pmap_prot.h>`, `<rpc/rpcb_prot.h>`, `<rpc/rpcent.h>`, `<rpcsvc/*.h>` | SunRPC, TI-RPC transport configuration, XDR serialization, `rpcbind`, and NIS/YP/NFS `rpcsvc` stubs |
| `bsd-sbuf` | `<sys/sbuf.h>` | FreeBSD safe string composition buffers (`sbuf_new`, `sbuf_printf`, `sbuf_cat`, `sbuf_finish`) |
| `bsd-stdlib` | `<bsd/stdlib.h>` | BSD standard library extensions (`reallocarray`, `recallocarray`, `freezero`, `arc4random*`, `strtonum`, `getprogname`/`setprogname`, `fgetln`/`fparseln`, `mergesort`/`heapsort`/`radixsort`, `cget*`, `daemon`, `closefrom`, `bsd_setmode`/`getmode`) |
| `bsd-string` | `<bsd/string.h>` | Safe string and mode operations (`strlcpy`, `strlcat`, `strnstr`, `strcasestr`, `strsep`, `strverscmp`, `strmode`, `explicit_bzero`, `timingsafe_bcmp`, `fflagstostr`) |
| `bsd-stringlist` | `<stringlist.h>` | Dynamic string list containers (`sl_init`, `sl_add`, `sl_find`, `sl_delete`, `sl_free`) |
| `bsd-sys-cdefs` | `<sys/cdefs.h>` | BSD compiler, attribute, alignment, bitfield, `__DECONST`/`__containerof`, and `__FBSDID` macro definitions |
| `bsd-sys-cpuset` | `<sys/cpuset.h>`, `<bsd-sys-cpuset.h>` | FreeBSD CPU affinity sets (`cpuset_getaffinity`, `cpuset_setaffinity`, `cpuset_getid`) |
| `bsd-sys-endian` | `<sys/endian.h>`, `<bsd-sys-endian.h>` | BSD byte ordering and stream encoding/decoding (`be16enc`..`le64dec`, `htobe*`, `bswap*`) |
| `bsd-sys-event` | `<sys/event.h>`, `<bsd-sys-event.h>` | `kqueue` and `kevent` notification polyfills |
| `bsd-sys-file` | `<sys/file.h>`, `<bsd-sys-file.h>` | Advisory file locking (`flock`, `LOCK_SH`, `LOCK_EX`) |
| `bsd-sys-param` | `<sys/param.h>`, `<bsd-sys-param.h>` | System parameters, FreeBSD version/limit constants, bitmaps, and `roundup`/`nitems`/`MIN`/`MAX` macros |
| `bsd-sys-queue` | `<sys/queue.h>` | BSD `SLIST`, `LIST`, `STAILQ`, and `TAILQ` intrusive data structure macros |
| `bsd-sys-sysctl` | `<sys/sysctl.h>`, `<bsd-sys-sysctl.h>` | System management and hardware/process inspection via `sysctl`, `sysctlbyname`, and `sysctlnametomib` |
| `bsd-sys-tree` | `<sys/tree.h>` | BSD `SPLAY` and `RB` (red-black) tree data structure macros |
| `bsd-utmpx` | `<utmpx.h>`, `<timeconv.h>`, `<ttyent.h>` | User accounting database (`getutxent`, `pututxline`), terminal table (`getttyent`), and time conversion helpers |
| `bsd-vis` | `<vis.h>` | Visual character encoding and decoding (`vis`, `strvis`, `stravis`, `strnvisx`, `strsenvisx`, `unvis`, `strunvis`) |

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
If you want all 91 modules compiled into a single static or shared library:

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

#### Transforming Upstream C99/C11/GNU Sources at Build Time
To compile unmodified C99/C11/GNU C upstream sources under strict MSVC C89 or OpenWatcom v2:

```cmake
include("${auto-win-msvc_SOURCE_DIR}/cmake/AutoWinMsvcTransform.cmake")

auto_win_msvc_transform_sources(
    OUTPUT_VAR   TRANSFORMED_SRCS
    SOURCES      ${UPSTREAM_C_SOURCES}
    INCLUDE_DIRS "${UPSTREAM_INCLUDE_DIR}"
    # RENAME_MAIN applet_main   # Optional: rename main() for multicall binaries
)

add_executable(your_target ${TRANSFORMED_SRCS})
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

54 standalone modules include per-module `vcpkg.json` manifests for integration with a custom vcpkg registry or overlay ports (for the full 91-module mega-library or newer BSD/POSIX modules without standalone manifests, prefer CMake `FetchContent` above):

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
