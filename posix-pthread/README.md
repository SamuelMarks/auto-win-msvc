# posix-pthread

A strict C89 implementation of the POSIX threads (`pthread`) API.

On native POSIX environments (like Linux, macOS, or Cygwin), this library operates as a transparent pass-through, simply forwarding calls to the system's native `pthread` implementation. On Windows, it provides a lightweight polyfill utilizing Windows API primitives (such as SRW Locks and Condition Variables) while deliberately avoiding the inclusion of `<windows.h>` in the public header to prevent namespace pollution.

## Tested Environments

The library has been tested across a variety of environments to ensure broad compatibility.

| Environment / Platform | Compiler / Toolchain | Status |
| ---------------------- | -------------------- | ------ |
| Windows                | MSVC (Modern)        | Tested |
| Windows                | MSVC 2005            | Tested |
| Windows                | MinGW                | Tested |
| Windows                | Cygwin               | Tested |
| Linux                  | GCC / Clang          | Native |
| macOS                  | Apple Clang          | Native |

## Implications and Things of Note

- **C89 Strictness:** The implementation strictly adheres to the C89 standard. It uses generic `void*` structures to hide platform-specific Windows handles.
- **Header Cleanliness:** The polyfill intentionally avoids including `<windows.h>`. Instead, it dynamically loads necessary synchronization functions (via `GetProcAddress` on `kernel32.dll`) at runtime. This allows downstream code to include `posix-pthread.h` without inadvertently polluting the global namespace with thousands of Windows macros, and avoids compilation times skyrocketing.
- **Partial Implementation:** A significant portion of the `pthread` API is currently stubbed out and will simply return `ENOSYS`. Only a subset of essential threading and synchronization mechanisms currently have functional Windows polyfills (e.g., Mutexes, RWLocks, Condition Variables, Spinlocks, Semaphores).

## Caveats

- **Windows Version Compatibility:** The polyfill dynamically loads Slim Reader/Writer (SRW) Locks and Condition Variables. These features were introduced in Windows Vista. Therefore, running this on Windows XP or older will result in these synchronization objects failing to initialize, as fallback mechanisms are not currently provided.
- **Named Semaphores & Unicode Support:** The `sem_open` function, which typically handles named semaphores, accepts a standard `const char *name`. Currently, named semaphores are unimplemented, but developers should note that there is no implicit wide-character (UTF-16) conversion or broad Unicode path support. 
- **Filepaths and UNC Path Support:** Any future implementation relying on named objects (like POSIX IPC objects or named semaphores mapped to Windows files/mutexes) will be bound by Windows' kernel object naming rules, rather than POSIX filepath semantics. This includes a lack of support for filepaths exceeding `MAX_PATH` (260 characters) and incomplete UNC (Universal Naming Convention) path support out of the box unless explicitly prefixed with `\\?\`.
- **Missing Core Functionality:** 
  - `pthread_create` and full thread lifecycle management features are not yet fully implemented for the Windows polyfill and return `ENOSYS`.
  - Thread cancellation (`pthread_cancel`), thread-specific data, and scheduling policies are not currently mapped to their Windows equivalents.