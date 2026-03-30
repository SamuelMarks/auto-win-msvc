#ifndef LINUX_SYS_PRCTL_H
#define LINUX_SYS_PRCTL_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define PR_SET_NAME 15
/** \brief prctl function. */
int prctl(int option, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif
