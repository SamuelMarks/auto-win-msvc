#ifndef SYS_PROCCTL_H
#define SYS_PROCCTL_H

/**
 * @file procctl.h
 * @brief FreeBSD process control (<sys/procctl.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
#include <sys/wait.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__)
#ifndef P_PID
#define P_PID 1
#endif
#ifndef P_PGID
#define P_PGID 2
#endif
#endif

#define PROC_SPROTECT 1
#define PROC_REAP_ACQUIRE 2
#define PROC_REAP_RELEASE 3
#define PROC_REAP_STATUS 4
#define PROC_REAP_GETPIDS 5
#define PROC_REAP_KILL 6
#define PROC_TRACE_CTL 7
#define PROC_TRACE_STATUS 8
#define PROC_TRAPCAP_CTL 9
#define PROC_TRAPCAP_STATUS 10
#define PROC_PDEATHSIG_CTL 11
#define PROC_PDEATHSIG_STATUS 12
#define PROC_ASLR_CTL 13
#define PROC_ASLR_STATUS 14
#define PROC_PROTMAX_CTL 15
#define PROC_PROTMAX_STATUS 16
#define PROC_STACKGAP_CTL 17
#define PROC_STACKGAP_STATUS 18
#define PROC_NO_NEW_PRIVS_CTL 19
#define PROC_NO_NEW_PRIVS_STATUS 20
#define PROC_WXMAP_CTL 21
#define PROC_WXMAP_STATUS 22

#define PPROT_SET 1
#define PPROT_CLEAR 2
#define PPROT_DESCEND 0x10
#define PPROT_INHERIT 0x20

#define PROC_TRACE_CTL_ENABLE 1
#define PROC_TRACE_CTL_DISABLE 2
#define PROC_TRACE_CTL_DISABLE_EXEC 3

#define PROC_TRAPCAP_CTL_ENABLE 1
#define PROC_TRAPCAP_CTL_DISABLE 2

#define PROC_ASLR_FORCE_ENABLE 1
#define PROC_ASLR_FORCE_DISABLE 2
#define PROC_ASLR_NOFORCE 3
#define PROC_ASLR_ACTIVE 0x80000000

#define PROC_PROTMAX_FORCE_ENABLE 1
#define PROC_PROTMAX_FORCE_DISABLE 2
#define PROC_PROTMAX_NOFORCE 3
#define PROC_PROTMAX_ACTIVE 0x80000000

#define PROC_STACKGAP_ENABLE 0x0001
#define PROC_STACKGAP_DISABLE 0x0002
#define PROC_STACKGAP_ENABLE_EXEC 0x0004
#define PROC_STACKGAP_DISABLE_EXEC 0x0008

#define PROC_NO_NEW_PRIVS_ENABLE 1
#define PROC_NO_NEW_PRIVS_DISABLE 2

#define PROC_WX_MAPPINGS_PERMIT 0x0001
#define PROC_WX_MAPPINGS_DISALLOW_EXEC 0x0002
#define PROC_WXORX_ENFORCE 0x80000000

/** @brief Control process attributes (no-op returning 0). */
int procctl(idtype_t idtype, id_t id, int com, void *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PROCCTL_H */
