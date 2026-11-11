#ifndef NET_IF_DL_H
#define NET_IF_DL_H

/**
 * @file if_dl.h
 * @brief BSD link-level sockaddr (<net/if_dl.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Link-level socket address structure. */
struct sockaddr_dl {
  unsigned char sdl_len;
  unsigned char sdl_family;
  unsigned short sdl_index;
  unsigned char sdl_type;
  unsigned char sdl_nlen;
  unsigned char sdl_alen;
  unsigned char sdl_slen;
  char sdl_data[46];
};

#define LLADDR(s) ((caddr_t)((s)->sdl_data + (s)->sdl_nlen))

void link_addr(const char *addr, struct sockaddr_dl *sdl);
char *link_ntoa(const struct sockaddr_dl *sdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NET_IF_DL_H */
