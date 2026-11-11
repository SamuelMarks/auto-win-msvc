#ifndef SYS_SOCKETVAR_H
#define SYS_SOCKETVAR_H

/**
 * @file socketvar.h
 * @brief FreeBSD kernel socket structures (<sys/socketvar.h>).
 */

/* clang-format off */
#include <sys/types.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _KVADDR_T_DEFINED
#define _KVADDR_T_DEFINED
typedef uint64_t kvaddr_t;
typedef uint64_t ksize_t;
#endif

struct xsockbuf {
  unsigned int sb_cc;
  unsigned int sb_hiwat;
  unsigned int sb_mbcnt;
  unsigned int sb_mcnt;
  unsigned int sb_ccnt;
  unsigned int sb_mbmax;
  int sb_lowat;
  int sb_timeo;
  short sb_flags;
};

struct xsocket {
  size_t xso_len;
  kvaddr_t xso_so;
  short so_type;
  short so_options;
  short so_linger;
  short so_state;
  kvaddr_t so_pcb;
  int xso_protocol;
  int xso_family;
  unsigned int so_qlen;
  unsigned int so_incqlen;
  unsigned int so_qlimit;
  short so_timeo;
  unsigned short so_error;
  pid_t so_pgid;
  unsigned long so_oobmark;
  struct xsockbuf so_rcv;
  struct xsockbuf so_snd;
  uid_t so_uid;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_SOCKETVAR_H */
