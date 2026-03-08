# solaris-sys-byteorder

A modular, strict C89-compliant POSIX/Linux/BSD compatibility layer for Windows.

## Features & Implementation Caveats

- **Broad Toolchain Support:** Validated against MSVC (2005, 2022, 2026), MinGW-w64, and Cygwin.
- **Strict C89 Compliance:** Written in pure C89 for maximum legacy compiler support. No `//` comments, variables declared at the top of scopes.
- **Zero Namespace Pollution:** Completely avoids `#include <windows.h>` in headers. Internal `.c` implementations utilize minimal declarations.
- **Flexible Linkage:** Fully supports both Static (`/MT`, `/MTd`) and Shared (`/MD`, `/MDd`) CRT, as well as Static or Shared (`.dll`) library output via `BUILD_SHARED_LIBS`.
- **Advanced Compilation Settings:** Supports CMake configuration toggles for `UNICODE`/ANSI, Link-Time Optimization (LTO), Single/Multi-threading, and MSVC Runtime Checks (`/RTC1`, `/RTCs`, `/RTCu`).
- **Safe CRT:** Automatically utilizes MSVC secure string extensions (e.g., `strcpy_s`, `sprintf_s`, `_TRUNCATE`) where available, falling back to standard C89 string manipulation on older toolchains or MinGW.
- **POSIX API Preservation:** Retains strict standard function signatures and structs. Error codes map natively to standard `errno`. Non-void helper functions return strict `int` exit codes.
- **Empty Translation Units:** Guarded against strict ISO C warnings via dummy typedefs and functions.

*(Note for Sockets/Network modules: Winsock initialization via `WSAStartup()` and `WSACleanup()` remains the responsibility of the consuming application.)*

## Usage

### Option 1: Native CMake FetchContent

You can consume this module individually using CMake's `FetchContent`.

```cmake
include(FetchContent)
FetchContent_Declare(
    solaris-sys-byteorder
    GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
    GIT_TAG        master
    SOURCE_SUBDIR  solaris-sys-byteorder
)
FetchContent_MakeAvailable(solaris-sys-byteorder)

# Link against the individual module
target_link_libraries(your_target PRIVATE solaris-sys-byteorder)
```

### Option 2: vcpkg

Add this specific module to your `vcpkg.json`:

```json
{
  "dependencies": [
    "solaris-sys-byteorder"
  ]
}
```
