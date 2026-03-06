# auto-win-msvc

**A Massive, Modular POSIX Compatibility Layer for MSVC**

`auto-win-msvc` is a comprehensive, modular suite of C89-compliant libraries designed to provide POSIX, BSD, and Linux user-space APIs natively on Windows via the Microsoft Visual C++ (MSVC) compiler. Inspired by projects like `libunistd`, this monorepo vastly expands the scope to cover almost 100% of the POSIX.1-2017 standard.

## 🚀 Extended Scope & Architecture

The project has been split into **18 distinct CMake projects** to minimize interdependencies and allow consumers to pull in *only* the POSIX headers they actually need (e.g., if you only need `<sys/mman.h>`, you don't need to link threading libraries).

### Core Principles
1. **Strict C89 Compliance:** Backwards compatible to the earliest versions of MSVC, while fully functional on modern Visual Studio.
2. **Zero-Overhead Native Mapping:** Where possible, functions are mapped directly to MSVC's underscore-prefixed equivalents (e.g., `read` -> `_read`).
3. **Robust Windows Polyfills:** Complex POSIX features (like `mmap`, `pthreads`, `dirent`) are polyfilled using native Win32 APIs (e.g., `CreateFileMapping`, `InitializeSRWLock`, `FindFirstFile`).
4. **100% Coverage Driven:** Every mapped symbol is backed by a unit test stub.
5. **LLM-Ready Iteration:** All unimplemented functions are currently scaffolded with `/* TODO: Map to X */` stubs and documented in JSON mapping files, designed specifically for AI agents to continuously implement and traverse.

## 📦 The 18 Libraries

| Project | Headers Covered | Description |
|---|---|---|
| **posix-types** | `<sys/types.h>`, `<stdint.h>` | Core POSIX types (`pid_t`, `ssize_t`, `mode_t`) |
| **posix-core** | `<unistd.h>`, `<fcntl.h>` | Standard I/O, file control, and basic process APIs |
| **posix-stat** | `<sys/stat.h>` | File attributes, permissions, and `mkdir` |
| **posix-dirent** | `<dirent.h>` | Directory traversal (`opendir`, `readdir`) |
| **posix-dlfcn** | `<dlfcn.h>` | Dynamic linking (`dlopen`, `dlsym`) |
| **posix-mman** | `<sys/mman.h>` | Memory mapping (`mmap`, `munmap`) |
| **posix-pthread** | `<pthread.h>`, `<sched.h>`, `<semaphore.h>` | POSIX threading, scheduling, and semaphores |
| **posix-sockets** | `<sys/socket.h>`, `<netdb.h>`, `<arpa/inet.h>`, `<poll.h>`, `<sys/select.h>` | Networking and asynchronous I/O |
| **posix-syslog** | `<syslog.h>` | System logging mapped to Windows Event Log |
| **posix-termios** | `<termios.h>` | Terminal I/O interfaces |
| **posix-glob** | `<glob.h>`, `<fnmatch.h>`, `<wordexp.h>` | Pattern matching and word expansion |
| **posix-ipc** | `<sys/ipc.h>`, `<sys/msg.h>`, `<sys/shm.h>`, `<sys/sem.h>` | XSI Interprocess Communication |
| **posix-pwdgrp** | `<pwd.h>`, `<grp.h>` | User and group database access |
| **posix-utsname**| `<sys/utsname.h>` | System information (`uname`) |
| **posix-libgen** | `<libgen.h>` | Pathname manipulation (`basename`, `dirname`) |
| **posix-spawn**  | `<spawn.h>` | Process spawning APIs |
| **posix-wait**   | `<sys/wait.h>` | Process waiting macros and functions |
| **posix-time**   | `<sys/time.h>`, `<time.h>`, `<utime.h>` | Time management and resolution |

## 🛠 Usage & Installation

You can consume these libraries individually or as a complete suite. 

### Option A: CMake FetchContent (Recommended)
Add the specific modules you need directly to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  posix-core
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-core # Targets just the core module
)
FetchContent_MakeAvailable(posix-core)

target_link_libraries(your_application PRIVATE posix-core)
```

### Option B: Vcpkg
We maintain custom vcpkg ports for every library. Add the desired library to your `vcpkg.json`:

```json
{
  "dependencies": [
    "posix-mman",
    "posix-pthread"
  ]
}
```
*(Note: Ensure you add our `vcpkg-ports/` directory to your vcpkg overlay ports path).*

## 🧪 Testing & CI

The project enforces strict C89 compilation natively. The root `CMakeLists.txt` strings together all 18 sub-projects. 

To build and test locally:
```bash
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Every push is validated via **GitHub Actions** across Windows MSVC to ensure 0 compiler warnings (`/W4 /WX`) and complete passing test stubs.

## 🤖 Contributing (LLM Guidance)
If you are an AI assistant traversing this codebase:
1. Check the `mappings.json` inside a project folder to see what Win32 APIs are required.
2. Replace the `/* TODO */` implementations in `src/*.c`.
3. Add meaningful assertions to `tests/test.c`.
4. Run `ctest` to verify.

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

