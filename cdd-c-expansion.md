# Expansion Plan for `cdd-c`

To achieve PR-ready, pre-processor-intertwined code, `cdd-c` needs to evolve from a standard parser/generator into a **Concrete Syntax Tree (CST) weaver**. Standard AST parsers discard whitespace, comments, and macro expansions. To generate patches acceptable by upstream maintainers, `cdd-c` must preserve the exact byte-for-byte layout of unmodified code.

Here is the roadmap for extending `cdd-c` to support the `auto-win-msvc` project.

## 1. Concrete Syntax Tree (CST) & Token Preservation
* **[ ] Token Stream Attachment:** Modify the parser to keep a continuous stream of original tokens (including whitespace, newlines, and comments) attached to the AST nodes.
* **[ ] Non-Destructive Rewriting:** Implement an emitter that writes the file back out by traversing the original token stream, applying insertions/replacements only where the AST has been explicitly flagged as mutated.
* **[ ] Macro Location Tracking:** Ensure `#include` and `#define` directives are parsed as distinct manipulable nodes in the tree, retaining their original line positions.

## 2. AST Pattern Matching & Querying
* **[ ] Function Call Matcher:** Implement a robust way to search the AST for specific function calls (e.g., `FindCall("strcpy")`), identifying the arguments as sub-trees.
* **[ ] Include Matcher:** Ability to find specific header inclusions (e.g., `FindInclude("<unistd.h>")`).
* **[ ] Type & Variable Usage Query:** Basic scope awareness to detect if common POSIX types like `ssize_t` or `pid_t` are actually used in a given `.c` or `.h` file, ensuring we only inject polyfills when necessary.

## 3. The "Weaver" Engine (Macro Injection)
* **[ ] The `#ifdef` Wrapper API:** Create a high-level API to wrap an existing AST node (or contiguous set of nodes) in conditional compilation blocks.
  * *Input:* Target Node, Condition (`_MSC_VER`), True-Block AST, False-Block AST.
  * *Output:* Woven token stream.
* **[ ] Guard Injection Strategy:** Support injecting the standardized Annex K/MSVC guard around unsafe CRT functions:
  ```c
  #if defined(_MSC_VER) && !defined(__INTEL_COMPILER) || \
      defined(__STDC_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__
  ```
* **[ ] Header Weaving:** API to safely append MSVC headers inside an `#ifdef` block *adjacent* to existing POSIX headers, handling things like `WIN32_LEAN_AND_MEAN` precedence.

## 4. AST Transformation Primitives
* **[ ] Argument Reordering/Injection:** When mapping `sprintf(str, ...)` to `sprintf_s(str, size, ...)`, `cdd-c` needs to safely duplicate the AST node for `str`, infer or inject the `size` argument, and reconstruct the call.
* **[ ] Format String Parser:** A utility to parse `printf`/`scanf` literal strings to identify format specifiers (e.g., `%ld`, `%zu`) and replace them with standard `<inttypes.h>` macros (e.g., `PRId64`) via string concatenation.
* **[ ] Type Replacement:** Swap standard C types for Windows types in function signatures or variable declarations (e.g., `pthread_t` -> `HANDLE`), wrapping the declaration in an `#ifdef` if required.

## 5. Build System Emitters
* **[ ] CMake Parser/Generator:** Since `cdd-c` focuses on C, it needs an auxiliary module to parse basic `CMakeLists.txt` structures to append MSVC-specific `target_compile_options` or link `ws2_32.lib`.
* **[ ] Makefile Scraper:** (Optional/Later) Basic regex/rule extraction from standard Makefiles to bootstrap initial `CMakeLists.txt` files for older projects.

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

## Immediate Action Items for cdd-c:
1. Validate the current parsing engine's ability to retain whitespace/comments (or switch the backend parser to a CST-friendly one if needed).
2. Implement the `Weaver Engine` primitive to successfully wrap a single `printf("hello")` into an `#ifdef _MSC_VER` block and write it back to disk perfectly formatted.
