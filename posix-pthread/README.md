# posix-pthread

C89 POSIX compatibility layer for MSVC.

## Implemented Symbols
- [ ] `pthread_atfork` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getdetachstate` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getguardsize` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getinheritsched` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getschedparam` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getschedpolicy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getscope` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getstack` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_getstacksize` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_init` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setdetachstate` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setguardsize` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setinheritsched` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setschedparam` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setschedpolicy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setscope` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setstack` (TODO: Polyfill with Win32 API)
- [ ] `pthread_attr_setstacksize` (TODO: Polyfill with Win32 API)
- [ ] `pthread_barrier_destroy` (TODO: Polyfill with DeleteSynchronizationBarrier)
- [ ] `pthread_barrier_init` (TODO: Polyfill with InitializeSynchronizationBarrier)
- [ ] `pthread_barrier_wait` (TODO: Polyfill with EnterSynchronizationBarrier)
- [ ] `pthread_barrierattr_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_barrierattr_getpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_barrierattr_init` (TODO: Polyfill with Win32 API)
- [ ] `pthread_barrierattr_setpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_cancel` (TODO: Polyfill with TerminateThread)
- [ ] `pthread_cleanup_pop` (TODO: Polyfill with Win32 API)
- [ ] `pthread_cleanup_push` (TODO: Polyfill with Win32 API)
- [ ] `pthread_cond_broadcast` (TODO: Polyfill with WakeAllConditionVariable)
- [ ] `pthread_cond_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_cond_init` (TODO: Polyfill with InitializeConditionVariable)
- [ ] `pthread_cond_signal` (TODO: Polyfill with WakeConditionVariable)
- [ ] `pthread_cond_timedwait` (TODO: Polyfill with SleepConditionVariableCS)
- [ ] `pthread_cond_wait` (TODO: Polyfill with SleepConditionVariableCS)
- [ ] `pthread_condattr_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_condattr_getclock` (TODO: Polyfill with Win32 API)
- [ ] `pthread_condattr_getpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_condattr_init` (TODO: Polyfill with Win32 API)
- [ ] `pthread_condattr_setclock` (TODO: Polyfill with Win32 API)
- [ ] `pthread_condattr_setpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_create` (TODO: Polyfill with _beginthreadex)
- [ ] `pthread_detach` (TODO: Polyfill with CloseHandle)
- [ ] `pthread_equal` (TODO: Polyfill with Win32 API)
- [ ] `pthread_exit` (TODO: Polyfill with _endthreadex)
- [ ] `pthread_getconcurrency` (TODO: Polyfill with Win32 API)
- [ ] `pthread_getcpuclockid` (TODO: Polyfill with Win32 API)
- [ ] `pthread_getschedparam` (TODO: Polyfill with Win32 API)
- [ ] `pthread_getspecific` (TODO: Polyfill with TlsGetValue)
- [ ] `pthread_join` (TODO: Polyfill with WaitForSingleObject)
- [ ] `pthread_key_create` (TODO: Polyfill with TlsAlloc)
- [ ] `pthread_key_delete` (TODO: Polyfill with TlsFree)
- [ ] `pthread_mutex_destroy` (TODO: Polyfill with DeleteCriticalSection)
- [ ] `pthread_mutex_init` (TODO: Polyfill with InitializeCriticalSection)
- [ ] `pthread_mutex_lock` (TODO: Polyfill with EnterCriticalSection)
- [ ] `pthread_mutex_timedlock` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutex_trylock` (TODO: Polyfill with TryEnterCriticalSection)
- [ ] `pthread_mutex_unlock` (TODO: Polyfill with LeaveCriticalSection)
- [ ] `pthread_mutexattr_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_getprioceiling` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_getprotocol` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_getpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_gettype` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_init` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_setprioceiling` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_setprotocol` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_setpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_mutexattr_settype` (TODO: Polyfill with Win32 API)
- [ ] `pthread_once` (TODO: Polyfill with InitOnceExecuteOnce)
- [ ] `pthread_rwlock_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_rwlock_init` (TODO: Polyfill with InitializeSRWLock)
- [ ] `pthread_rwlock_rdlock` (TODO: Polyfill with AcquireSRWLockShared)
- [ ] `pthread_rwlock_timedrdlock` (TODO: Polyfill with Win32 API)
- [ ] `pthread_rwlock_timedwrlock` (TODO: Polyfill with Win32 API)
- [ ] `pthread_rwlock_tryrdlock` (TODO: Polyfill with TryAcquireSRWLockShared)
- [ ] `pthread_rwlock_trywrlock` (TODO: Polyfill with TryAcquireSRWLockExclusive)
- [ ] `pthread_rwlock_unlock` (TODO: Polyfill with ReleaseSRWLockShared)
- [ ] `pthread_rwlock_wrlock` (TODO: Polyfill with AcquireSRWLockExclusive)
- [ ] `pthread_rwlockattr_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_rwlockattr_getpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_rwlockattr_init` (TODO: Polyfill with Win32 API)
- [ ] `pthread_rwlockattr_setpshared` (TODO: Polyfill with Win32 API)
- [ ] `pthread_self` (TODO: Polyfill with GetCurrentThreadId)
- [ ] `pthread_setcancelstate` (TODO: Polyfill with Win32 API)
- [ ] `pthread_setcanceltype` (TODO: Polyfill with Win32 API)
- [ ] `pthread_setconcurrency` (TODO: Polyfill with Win32 API)
- [ ] `pthread_setschedparam` (TODO: Polyfill with Win32 API)
- [ ] `pthread_setschedprio` (TODO: Polyfill with Win32 API)
- [ ] `pthread_setspecific` (TODO: Polyfill with TlsSetValue)
- [ ] `pthread_sigmask` (TODO: Polyfill with Win32 API)
- [ ] `pthread_spin_destroy` (TODO: Polyfill with Win32 API)
- [ ] `pthread_spin_init` (TODO: Polyfill with InitializeCriticalSectionAndSpinCount)
- [ ] `pthread_spin_lock` (TODO: Polyfill with EnterCriticalSection)
- [ ] `pthread_spin_trylock` (TODO: Polyfill with TryEnterCriticalSection)
- [ ] `pthread_spin_unlock` (TODO: Polyfill with LeaveCriticalSection)
- [ ] `pthread_testcancel` (TODO: Polyfill with Win32 API)
- [ ] `sched_get_priority_max` (TODO: Polyfill with Win32 API)
- [ ] `sched_get_priority_min` (TODO: Polyfill with Win32 API)
- [ ] `sched_getparam` (TODO: Polyfill with Win32 API)
- [ ] `sched_getscheduler` (TODO: Polyfill with Win32 API)
- [ ] `sched_rr_get_interval` (TODO: Polyfill with Win32 API)
- [ ] `sched_setparam` (TODO: Polyfill with Win32 API)
- [ ] `sched_setscheduler` (TODO: Polyfill with Win32 API)
- [ ] `sched_yield` (TODO: Polyfill with SwitchToThread / Sleep(0))
- [ ] `sem_close` (TODO: Polyfill with CloseHandle)
- [ ] `sem_destroy` (TODO: Polyfill with CloseHandle)
- [ ] `sem_getvalue` (TODO: Polyfill with Win32 API)
- [ ] `sem_init` (TODO: Polyfill with CreateSemaphore)
- [ ] `sem_open` (TODO: Polyfill with CreateSemaphore)
- [ ] `sem_post` (TODO: Polyfill with ReleaseSemaphore)
- [ ] `sem_timedwait` (TODO: Polyfill with WaitForSingleObject)
- [ ] `sem_trywait` (TODO: Polyfill with WaitForSingleObject)
- [ ] `sem_unlink` (TODO: Polyfill with Win32 API)
- [ ] `sem_wait` (TODO: Polyfill with WaitForSingleObject)


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
    "posix-pthread"
  ]
}
```

### FetchContent (CMake)
```cmake
include(FetchContent)
FetchContent_Declare(
  posix-pthread
  GIT_REPOSITORY https://github.com/SamuelMarks/auto-win-msvc.git
  GIT_TAG master
  SOURCE_SUBDIR posix-pthread
)
FetchContent_MakeAvailable(posix-pthread)
target_link_libraries(your_target PRIVATE posix-pthread)
```
