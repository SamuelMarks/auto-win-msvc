build: fix CI workflows, C89 compliance, MSVC 2005 compatibility, MinGW warnings, and downstream tests

- Refactor FetchContent and enforce strict C89 compliance across POSIX polyfills (MSVC 2005 compatibility).
- Resolve MinGW compilation errors and warnings (posix-wait, posix-sockets).
- Fix test-downstream CI issues by ensuring auto-win-msvc updates correctly and missing CMake variables/typedefs are passed down.
- Add robust header guards for internal system headers (e.g., <io.h>) against local shadowing.
- Fix Python testing hooks to prevent cyclic copytree recursion.
- Fix formatting and ensure .pre-commit-config.yaml executes smoothly.
