#ifndef LINUX_SYS_SYSCALL_H
#define LINUX_SYS_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define SYS_gettid 178
/** \brief syscall function. */
long syscall(long number, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif
