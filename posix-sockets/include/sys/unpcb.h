#ifndef SYS_UNPCB_H
#define SYS_UNPCB_H

/**
 * @file unpcb.h
 * @brief FreeBSD UNIX domain protocol control block (<sys/unpcb.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socketvar.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct xunpcb {
  size_t xu_len;
  kvaddr_t xu_unpp;
  kvaddr_t xu_next;
  kvaddr_t xu_socket;
  kvaddr_t xu_vnode;
  kvaddr_t xu_conn;
  struct xsocket xu_socket_data;
  struct sockaddr_un xu_addr;
  struct sockaddr_un xu_caddr;
};

struct xunpgen {
  size_t xug_len;
  unsigned int xug_count;
  uint64_t xug_gen;
  uint64_t xug_sogen;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_UNPCB_H */
