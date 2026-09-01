
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LINUX_SYS_PRCTL_H
#define LINUX_SYS_PRCTL_H

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

#if defined(_MSC_VER) && !defined(__clang__)

/** \brief Option to set the name of the calling thread. */
#define PR_SET_NAME 15

/** \brief Option to receive a signal when the parent process dies. */
#define PR_SET_PDEATHSIG 1

/** \brief Option to get the current parent death signal. */
#define PR_GET_PDEATHSIG 2

/** \brief prctl function.
 *
 * Implements process control operations.
 * Currently supports PR_SET_NAME to set the calling thread's name.
 *
 * \param option The operation to perform.
 * \param ... Variable arguments based on the option.
 * \return ERR_NONE on success, or an error code.
 */
error_type_t prctl(int option, ...);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
