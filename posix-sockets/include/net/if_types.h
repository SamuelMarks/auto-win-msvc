#ifndef NET_IF_TYPES_H
#define NET_IF_TYPES_H

/**
 * @file if_types.h
 * @brief BSD network interface type constants (<net/if_types.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

#define IFT_OTHER 0x1
#define IFT_ETHER 0x6
#define IFT_ISO88025 0x9
#define IFT_FDDI 0xf
#define IFT_PPP 0x17
#define IFT_LOOP 0x18
#define IFT_SLIP 0x1c
#define IFT_PARA 0x22
#define IFT_PROPVIRTUAL 0x35
#define IFT_IEEE1394 0x90
#define IFT_L2VLAN 0x87
#define IFT_BRIDGE 0xd1

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NET_IF_TYPES_H */
