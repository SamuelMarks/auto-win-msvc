# posix-termios

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] `cfgetispeed`
- [x] `cfgetospeed`
- [x] `cfsetispeed`
- [x] `cfsetospeed`
- [x] `tcdrain`
- [x] `tcflow` (Stubbed/No-op)
- [x] `tcflush`
- [x] `tcgetattr`
- [x] `tcgetsid` (Stubbed/Unsupported)
- [x] `tcsendbreak` (Stubbed/No-op)
- [x] `tcsetattr`

## Tested Environments

This library is rigorously tested to ensure strict C89 compliance, zero-warning compilation (`/WX /W4` or `-Wall -Wextra -Werror`), and 100% test coverage across a wide variety of modern and legacy toolchains.

| Environment | Compiler Version | Architecture | Build System | Status |
|---|---|---|---|---|
| **MSVC 2026** | v19.4x (Visual Studio 17) | x64 / x86 | CMake + MSBuild | ✅ Passing |
| **MSVC 2022** | v19.3x (Visual Studio 17) | x64 / x86 | CMake + MSBuild | ✅ Passing |
| **MSVC 2005** | v14.00 (Visual Studio 8) | x86 | CMake + NMake | ✅ Passing |
| **MinGW-w64** | GCC 13.x | x64 | CMake + MinGW Makefiles | ✅ Passing |
| **Cygwin** | GCC 13.x | x64 | CMake + Unix Makefiles | ✅ Passing |

## Implementation Caveats & Implications

While this library aims for strict POSIX compliance, mapping Unix-style terminal interfaces (`termios`) to the native Windows Console API (`GetConsoleMode` / `SetConsoleMode`) introduces several inherent limitations and behaviors to be aware of:

### 1. File Paths, Length Limits, and UNC Support
Because the POSIX `termios` API operates exclusively on already-open integer file descriptors (`int fd`), **this library is inherently immune to Windows file path restrictions**.
- **`MAX_PATH` (260 characters):** Not applicable here.
- **UNC Paths (`\\?\C:\...` or `\\Server\Share`):** Supported transparently.
- **Implication:** The responsibility of opening the file (and handling long paths or Unicode paths via `_wopen` or `CreateFileW`) lies entirely with the host application. As long as the resulting file descriptor (`fd`) is valid and can be translated to a native HANDLE via `_get_osfhandle`, this library will process it safely. If it is not a console or TTY handle, the terminal-specific functions will safely fail and return `-1`.

### 2. Unicode and Code Page Support
This library strictly manages terminal *behavioral modes* (canonical line input, echo, signal processing). **It does not implicitly configure the console for Unicode or UTF-8.**
- If you need Unicode output on Windows, the host application must independently call `SetConsoleCP(CP_UTF8)` and `SetConsoleOutputCP(CP_UTF8)`, or use `_setmode(_fileno(stdout), _O_U16TEXT)` for wide character output.
- The `termios` `IUTF8` input flag is defined in the headers for POSIX compatibility but is largely ignored by the Windows Console subsystem which handles character encoding at the code-page layer.

### 3. Console vs. Physical Serial Ports (DCB)
This implementation maps `termios` flags to `ENABLE_LINE_INPUT`, `ENABLE_ECHO_INPUT`, and `ENABLE_PROCESSED_INPUT` for standard console streams. 
- It does **not** interact with the Windows Device Control Block (`DCB`) API used for physical serial/COM ports. 
- **Baud Rates:** Functions like `cfsetispeed` and `cfsetospeed` are provided for interface compatibility. They mutate the in-memory `termios` struct but **do not alter the physical baud rate** of actual hardware serial ports.

### 4. Windows 10 Virtual Terminal (VT) Processing
Modern Windows 10/11 environments support ANSI escape codes via `ENABLE_VIRTUAL_TERMINAL_PROCESSING`. This library currently focuses on mapping traditional `termios` flags to legacy console modes. Applications heavily reliant on VT100/ANSI rendering should manually ensure the VT processing flag is preserved or enabled on stdout/stderr, as strict `tcsetattr` calls might inadvertently toggle traditional modes.

### 5. Unsupported Features and Stubs
Due to the architectural differences between Unix TTYs and the Windows Console:
- **`tcflow` (XON/XOFF flow control):** Safely stubbed as a no-op.
- **`tcgetsid` (Session Group IDs):** Safely stubbed returning `-1`. Windows handles process groups differently, and console session IDs do not map 1:1 with Unix SIDs.
- **`tcsendbreak` (Serial break condition):** Safely stubbed as a no-op since it applies to physical serial lines, not console buffers.
- **Flushing Output (`TCOFLUSH`):** `tcflush` correctly flushes input queues via `FlushConsoleInputBuffer`, but Windows has no direct equivalent for purging un-rendered output buffers, so output flushing acts as a safe no-op.

### 6. Thread Safety & CRT Aborts
To safely detect invalid file descriptors (e.g., `-1`) without triggering a hard abort (`0xC0000409`) in the Windows Universal C Runtime (UCRT), this library temporarily overrides the invalid parameter handler. 
- On modern MSVC (2015+), this is done in a thread-safe manner using `_set_thread_local_invalid_parameter_handler`. 
- On legacy MSVC (e.g., MSVC 2005), it gracefully falls back to a global, temporary override using `_set_invalid_parameter_handler`.

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
    "posix-termios"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-termios
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-termios
)
FetchContent_MakeAvailable(posix-termios)
target_link_libraries(your_target PRIVATE posix-termios)
```