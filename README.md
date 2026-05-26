auto-win-msvc
=============

![Doc Coverage](https://img.shields.io/badge/doc__coverage-98%25-brightgreen)
![Test Coverage](https://img.shields.io/badge/test__coverage-100%25-brightgreen)






















































[![License](https://img.shields.io/badge/license-CC0%20OR%20Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![MSVC POSIX Monorepo CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/ci.yml)
[![Multi-Environment CI](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml/badge.svg)](https://github.com/SamuelMarks/auto-win-msvc/actions/workflows/multi-env-ci.yml)

**A Massive, Modular POSIX Compatibility Layer for MSVC, MinGW, and Cygwin**

`auto-win-msvc` is a comprehensive suite of C89-compliant libraries designed to provide POSIX, BSD, Solaris, and Linux user-space APIs natively on Windows. Validated against Microsoft Visual C++ (MSVC 2005, 2022, 2026), MinGW-w64, and Cygwin, this monorepo expands across 59 distinct modules to cover nearly 100% of the standard user-space APIs.

## 🏆 Current Feature Set & Implementation Details

- **Massive Scope:** 59 separate modules for `posix-*`, `linux-*`, `bsd-*`, and `solaris-*` headers.
- **Strict C89 Compliance:** Guaranteed pure ISO C90 (C89) compatibility for legacy enterprise integration.
- **Mega Library & Modular Architecture:** Consume the entire suite as a single `auto-win-msvc` mega-library (shared `.dll` or static `.lib`), OR fetch individual modules piece-by-piece to minimize binary bloat.
- **Implementation Caveats & Safety:**
  - **Zero `<windows.h>` Namespace Pollution:** Public headers completely avoid pulling in the massive `<windows.h>` payload.
  - **Safe CRT Utilization:** Internally litters MSVC safe C runtime (CRT) extensions (`sprintf_s`, `_TRUNCATE`, etc.) with proper `#if defined` legacy fallbacks to ensure buffer safety without breaking non-MSVC toolchains.
  - **Customizable Compilation Matrix:** Build configurations seamlessly support `UNICODE` vs `ANSI`, Multi-threaded vs Single-threaded (`/MT` vs `/MTd`, `/MD` vs `/MDd`), Link-Time Optimization (LTO), and granular MSVC Runtime Checks (`/RTC1`, `/RTCs`, `/RTCu`).
  - **Strict Helper Exit Codes:** All internal non-void, non-math helper functions return strict `int` exit codes for reliable execution auditing.
  - **Winsock Responsibility:** For `posix-sockets` and network-related modules, consumers remain responsible for calling `WSAStartup()` and `WSACleanup()`.

## 🚀 Usage Guide

### Option 1: Native CMake FetchContent

#### Fetching the Entire "Mega Library"
If you want all 59 modules compiled into a single static or shared library:

```cmake
include(FetchContent)
FetchContent_Declare(
    auto-win-msvc
    GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(auto-win-msvc)

# Build configurations (can be toggled before MakeAvailable or via command line):

# set(BUILD_SHARED_LIBS ON)
# set(AUTO_WIN_MSVC_UNICODE ON)
# set(AUTO_WIN_MSVC_CRT "Static") # or "Shared"

target_link_libraries(your_target PRIVATE auto-win-msvc)
```

#### Fetching an Individual Module
If you only need a specific symbol (e.g., `<sys/epoll.h>` via `linux-epoll`):

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
*Or, for a specific module:*
```json
{
  "dependencies": [
    "linux-epoll",
    "posix-sockets"
  ]
}
```

## 🛠 Supported Environments Summary

| Toolchain / Environment | Build Status | Highlights |
|---|---|---|
| **MSVC 2005** | ✅ Passing | Strict legacy C89 compatibility. |
| **MSVC 2022** | ✅ Passing | Native modern Windows development with strict `/W4 /WX`. |
| **MSVC 2026** | ✅ Passing | Future-proofed against upcoming MSVC shifts. |
| **MinGW** | ✅ Passing | Seamless integration for GNU toolchains targeting Windows natively. |
| **Cygwin** | ✅ Passing | Validated against the gold-standard POSIX-on-Windows environment. |
