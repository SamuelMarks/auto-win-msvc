# Implementation Phases

The project is broken down into distinct phases, each enabled as CLI options. 
Each transformation supports an `audit` (dry-run/report) and a `fix` (apply) flag.

### Phase 1: C/POSIX Standard to Safe CRT Migration
*Audit identifies unsafe/POSIX usages; Fix injects the conditional MSVC alternatives using the standard guard.*

| Audit | Fix | Task | Description |
| :---: | :---: | :--- | :--- |
| [ ] | [ ] | **String Operations** | Wrap `strcpy` -> `strcpy_s`, `sprintf` -> `sprintf_s`, `strncat` -> `strncat_s`, etc., using the standard `_MSC_VER`/`__STDC_LIB_EXT1__` guard. |
| [ ] | [ ] | **POSIX Function Aliases** | Wrap deprecated POSIX names to MSVC equivalents (e.g., `strdup` -> `_strdup`, `fileno` -> `_fileno`, `isatty` -> `_isatty`). |
| [ ] | [ ] | **File I/O** | Wrap unsafe I/O `fopen` -> `fopen_s`, `freopen` -> `freopen_s`. |
| [ ] | [ ] | **Environment Variables** | Wrap `getenv` -> `_dupenv_s` or `getenv_s`. |

### Phase 2: POSIX/Linux/BSD Headers & Guards
| Audit | Fix | Task | Description |
| :---: | :---: | :--- | :--- |
| [ ] | [ ] | **Identify POSIX Headers** | Detect `<unistd.h>`, `<sys/types.h>`, `<sys/socket.h>`, `<pthread.h>`, etc. |
| [ ] | [ ] | **Guard Existing Headers** | Protect identified non-Windows headers under `#ifndef _MSC_VER` macro guards. |
| [ ] | [ ] | **Inject MSVC Headers** | Add POSIX/Linux/BSD header alternatives (e.g., `<windows.h>`, `<io.h>`, `<winsock2.h>`) to the `#else` block of the above, or flip conditions to check MSVC first. |
| [ ] | [ ] | **WinSock2 Precedence** | Ensure `<winsock2.h>` is injected *before* `<windows.h>` to prevent macro clashes. |

### Phase 3: Types, Macros & Format Specifiers
| Audit | Fix | Task | Description |
| :---: | :---: | :--- | :--- |
| [ ] | [ ] | **Typedef Injection** | Add basic typedefs and macros for common things like `SIZE_T`, `ssize_t`, `pid_t` based on their usage within the source file. |
| [ ] | [ ] | **printf Formatting** | Use macro `printf` modifiers (e.g., `<inttypes.h>` macros like `PRId64`, `PRIu64`) to support MSVC non-standard options without duplicating the format string. |
| [ ] | [ ] | **Macro Overrides** | Provide `#define` polyfills for missing macros (e.g., `S_ISDIR`, `__func__` vs `__FUNCTION__`). |

### Phase 4: Build System Adaptation
| Audit | Fix | Task | Description |
| :---: | :---: | :--- | :--- |
| [ ] | [ ] | **CMake Modifications** | Modify existing CMake configurations for MSVC compatibility (compiler flags, linking Windows libraries). |
| [ ] | [ ] | **Initial CMake Generation** | Generate an initial CMake project derived from existing non-CMake build systems (Make, Autotools) to support both the original targets and MSVC. |

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

