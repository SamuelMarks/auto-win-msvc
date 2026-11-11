#ifndef NETLINK_NETLINK_SYSEVENT_H
#define NETLINK_NETLINK_SYSEVENT_H

/**
 * @file netlink_sysevent.h
 * @brief FreeBSD Netlink Sysevent (<netlink/netlink_sysevent.h>) definitions.
 */

/* clang-format off */
#include <netlink/netlink.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum {
  NLSE_ATTR_UNSPEC,
  NLSE_ATTR_SYSTEM,
  NLSE_ATTR_SUBSYSTEM,
  NLSE_ATTR_TYPE,
  NLSE_ATTR_DATA
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETLINK_NETLINK_SYSEVENT_H */
