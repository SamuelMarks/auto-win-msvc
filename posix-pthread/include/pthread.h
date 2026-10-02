#if defined(__GNUC__)
#endif
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#pragma GCC system_header
#include_next <pthread.h>
#else
#include <pthread.h>
#endif
#else
#include "posix-pthread.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* API Contract symbols: pthread_atfork, pthread_attr_destroy,
 * pthread_attr_getdetachstate, pthread_attr_init, pthread_attr_setdetachstate,
 * pthread_attr_setguardsize, pthread_attr_setinheritsched,
 * pthread_attr_setschedpolicy, pthread_attr_setscope, pthread_attr_setstack,
 * pthread_attr_setstacksize, pthread_cancel, pthread_cond_broadcast,
 * pthread_cond_destroy, pthread_cond_signal, pthread_condattr_destroy,
 * pthread_condattr_init, pthread_condattr_setclock,
 * pthread_condattr_setpshared, pthread_detach, pthread_equal,
 * pthread_getconcurrency, pthread_getcpuclockid, pthread_getspecific,
 * pthread_join, pthread_key_create, pthread_key_delete,
 * pthread_mutex_consistent, pthread_mutex_destroy, pthread_mutex_lock,
 * pthread_mutex_trylock, pthread_mutex_unlock, pthread_mutexattr_destroy,
 * pthread_mutexattr_init, pthread_mutexattr_setprioceiling,
 * pthread_mutexattr_setprotocol, pthread_mutexattr_setpshared,
 * pthread_mutexattr_setrobust, pthread_mutexattr_settype, pthread_once,
 * pthread_rwlock_destroy, pthread_rwlock_rdlock, pthread_rwlock_tryrdlock,
 * pthread_rwlock_trywrlock, pthread_rwlock_unlock, pthread_rwlock_wrlock,
 * pthread_rwlockattr_destroy, pthread_rwlockattr_init,
 * pthread_rwlockattr_setpshared, pthread_setcancelstate, pthread_setcanceltype,
 * pthread_setconcurrency, pthread_setschedprio, pthread_setspecific,
 * pthread_testcancel, __dependencies__, __include_next__, pthread_exit,
 * pthread_self */
