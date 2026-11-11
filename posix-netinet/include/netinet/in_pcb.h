#ifndef NETINET_IN_PCB_H
#define NETINET_IN_PCB_H

/**
 * @file in_pcb.h
 * @brief FreeBSD Internet Protocol Control Block (<netinet/in_pcb.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct in_endpoints {
  uint16_t ie_fport;
  uint16_t ie_lport;
  union {
    struct in_addr ie46_foreign;
    struct in6_addr ie6_foreign;
  } ie_dependfaddr;
  union {
    struct in_addr ie46_local;
    struct in6_addr ie6_local;
  } ie_dependladdr;
};

struct in_conninfo {
  uint8_t inc_flags;
  uint8_t inc_len;
  uint16_t inc_fibnum;
  struct in_endpoints inc_ie;
};

#define inc_fport inc_ie.ie_fport
#define inc_lport inc_ie.ie_lport
#define inc_faddr inc_ie.ie_dependfaddr.ie46_foreign
#define inc_laddr inc_ie.ie_dependladdr.ie46_local
#define inc6_faddr inc_ie.ie_dependfaddr.ie6_foreign
#define inc6_laddr inc_ie.ie_dependladdr.ie6_local

#define INC_ISIPV6 0x01

struct xinpcb {
  size_t xi_len;
  kvaddr_t inp_ppcb;
  struct in_conninfo inp_inc;
  int inp_flags;
  int inp_flags2;
  uint64_t inp_gencnt;
  struct xsocket xi_socket;
};

struct xinpgen {
  size_t xig_len;
  unsigned int xig_count;
  uint64_t xig_gen;
  uint64_t xig_sogen;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_IN_PCB_H */
