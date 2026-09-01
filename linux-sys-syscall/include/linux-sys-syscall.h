
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LINUX_SYS_SYSCALL_H
#define LINUX_SYS_SYSCALL_H

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define SYS_gettid 178
/** \brief syscall function. */
error_type_t syscall(long number, long *out_result, ...);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
