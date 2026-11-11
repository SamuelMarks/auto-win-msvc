#ifndef NETLINK_NETLINK_GENERIC_H
#define NETLINK_NETLINK_GENERIC_H

/**
 * @file netlink_generic.h
 * @brief FreeBSD Generic Netlink (<netlink/netlink_generic.h>) definitions.
 */

/* clang-format off */
#include <netlink/netlink.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct genlmsghdr {
  uint8_t cmd;
  uint8_t version;
  uint16_t reserved;
};

#define GENL_ID_CTRL 0x10

enum {
  CTRL_CMD_UNSPEC,
  CTRL_CMD_NEWFAMILY,
  CTRL_CMD_DELFAMILY,
  CTRL_CMD_GETFAMILY,
  CTRL_CMD_NEWOPS,
  CTRL_CMD_DELOPS,
  CTRL_CMD_GETOPS,
  CTRL_CMD_NEWMCAST_GRP,
  CTRL_CMD_DELMCAST_GRP,
  CTRL_CMD_GETMCAST_GRP
};

enum {
  CTRL_ATTR_UNSPEC,
  CTRL_ATTR_FAMILY_ID,
  CTRL_ATTR_FAMILY_NAME,
  CTRL_ATTR_VERSION,
  CTRL_ATTR_HDRSIZE,
  CTRL_ATTR_MAXATTR,
  CTRL_ATTR_OPS,
  CTRL_ATTR_MCAST_GROUPS
};

enum { CTRL_ATTR_OP_UNSPEC, CTRL_ATTR_OP_ID, CTRL_ATTR_OP_FLAGS };

enum {
  CTRL_ATTR_MCAST_GRP_UNSPEC,
  CTRL_ATTR_MCAST_GRP_NAME,
  CTRL_ATTR_MCAST_GRP_ID
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETLINK_NETLINK_GENERIC_H */
