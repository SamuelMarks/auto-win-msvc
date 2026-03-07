# posix-dlfcn

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [x] dlopen (Polyfill with LoadLibraryA)
- [x] dlsym (Polyfill with GetProcAddress)
- [x] dlclose (Polyfill with FreeLibrary)
- [x] dlerror (Polyfill with FormatMessageA)
- [x] dladdr (Polyfill with VirtualQuery and GetModuleFileNameA)

## Supported Environments

This library is rigorously tested and verified to compile cleanly with strict C89 compliance (/W4 /WX or -Wall -Wextra -Werror -pedantic) across a wide range of toolchains:

| Environment | Architecture | Compiler / Version | Status | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MSVC 2005** | x86 | cl.exe (v14.00) | ? Passing | Handles legacy <windef.h> & MSVC internal warnings |
| **MSVC 2022** | x64 | cl.exe (v19.xx) | ? Passing | Lean Windows headers <minwindef.h> used |
| **MSVC 2026** | x64 | cl.exe (v19.44+) | ? Passing | Strict /WX compliance with latest toolset |
| **MinGW-w64** | x86_64 | gcc (v13.x+) | ? Passing | Native API mappings, strict C89 pedantic |
| **Cygwin** | x86_64 | gcc (v13.x+) | ? Passing | Fallbacks cleanly to stubbed implementations |

## Implementation Notes & Caveats

To maintain strict C89 compliance and minimal footprint, this implementation has the following caveats:

- **ASCII Only / No Unicode Support:** This layer strictly uses the ASCII versions of the Windows API (e.g., LoadLibraryA, GetModuleFileNameA, FormatMessageA). Paths containing non-ASCII characters or requiring wide strings (wchar_t) are not supported.
- **Path Length Limitations:** The implementation uses MAX_PATH (260 characters) for dladdr module name resolution. Extremely long filepaths or certain deep UNC (\\?\...) paths exceeding this limit will be truncated or fail.
- **RTLD Flags Ignored:** While RTLD_LAZY, RTLD_NOW, RTLD_GLOBAL, and RTLD_LOCAL are defined for API compatibility, they are essentially ignored as they have no direct, lightweight equivalent in standard LoadLibrary behavior on Windows without complex custom loaders.
- **RTLD_NEXT Unsupported:** RTLD_NEXT is notoriously difficult to implement on Windows without relying on heavy debugging APIs (like DbgHelp) or manually parsing the PE export tables. Passing RTLD_NEXT to dlsym will return NULL and set an error message.
- **Thread Local Storage (TLS):** dlerror is implemented using compiler-specific thread-local storage macros (__declspec(thread) for MSVC/Intel, __thread for GCC/Clang) to ensure thread safety without pulling in the entire pthreads library or complex Windows TlsAlloc boilerplate.
- **dladdr Symbol Information:** The dladdr implementation successfully resolves the file base address (dli_fbase) and the module file name (dli_fname). However, resolving the nearest symbol name (dli_sname) and exact symbol address (dli_saddr) requires parsing PE exports or using dbghelp.dll, which inflates binary size and complexity. Therefore, dli_sname and dli_saddr always return NULL.

## Current Status & Future Plans

**Current Status:**
- The uto-win-msvc monorepo has been successfully scaffolded into 18 distinct, modular CMake projects. 
- All standard POSIX headers and types are generated and strictly C89 compliant.
- Simple functions with direct MSVC equivalents (e.g., open -> _open) are fully mapped via macros.    
- Complex POSIX APIs requiring polyfills (e.g., mmap, pthreads, dirent) are currently scaffolded as ENOSYS stubs, with their target Win32 APIs documented in mappings.json files.
- Test files and build systems (CMake and vcpkg) are in place.

**Future Plans:**
- **AI-Driven Iteration:** Iteratively implement all stubbed polyfills using native Win32 APIs across the 18 modules, maintaining 0 compiler warnings (/W4 /WX) and strict C89 compliance.
- **cdd-c Integration:** Expand cdd-c into a Concrete Syntax Tree (CST) weaver (as outlined in cdd-c-expansion.md). This will allow automated, byte-for-byte precise injection of uto-win-msvc polyfills and standard #ifdef _MSC_VER guards directly into legacy C codebases.

## Installation

### Vcpkg
Add to your cpkg.json:
`json
{
  "dependencies": [
    "posix-dlfcn"
  ]
}
`

### FetchContent (CMake)
`cmake
include(FetchContent)
FetchContent_Declare(
  posix-dlfcn
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-dlfcn
)
FetchContent_MakeAvailable(posix-dlfcn)
target_link_libraries(your_target PRIVATE posix-dlfcn)
`