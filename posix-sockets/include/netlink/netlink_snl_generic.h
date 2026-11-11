#ifndef NETLINK_NETLINK_SNL_GENERIC_H
#define NETLINK_NETLINK_SNL_GENERIC_H

/**
 * @file netlink_snl_generic.h
 * @brief FreeBSD Simple Netlink Generic (<netlink/netlink_snl_generic.h>)
 * polyfill.
 */

/* clang-format off */
#include <netlink/netlink_snl.h>
#include <netlink/netlink_generic.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

static __inline uint16_t snl_get_genl_family(struct snl_state *ss,
                                             const char *family_name) {
  (void)ss;
  (void)family_name;
  return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETLINK_NETLINK_SNL_GENERIC_H */
