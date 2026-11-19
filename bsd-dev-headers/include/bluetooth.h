#ifndef BSD_BLUETOOTH_H
#define BSD_BLUETOOTH_H

/**
 * @file bluetooth.h
 * @brief FreeBSD Bluetooth userland library (<bluetooth.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AF_BLUETOOTH
#define AF_BLUETOOTH 36
#endif
#ifndef PF_BLUETOOTH
#define PF_BLUETOOTH AF_BLUETOOTH
#endif

#define BLUETOOTH_PROTO_HCI 134
#define BLUETOOTH_PROTO_L2CAP 135
#define BLUETOOTH_PROTO_RFCOMM 136
#define BLUETOOTH_PROTO_SCO 137

#if defined(_MSC_VER) || defined(__WATCOMC__)
#pragma pack(push, 1)
#endif
struct fbsd_bdaddr {
  uint8_t b[6];
};
#if defined(_MSC_VER) || defined(__WATCOMC__)
#pragma pack(pop)
#endif
typedef struct fbsd_bdaddr bdaddr_t;
typedef bdaddr_t *bdaddr_p;

#define NG_HCI_BDADDR_ANY ((const bdaddr_t *)&fbsd_bt_addr_any)
extern const bdaddr_t fbsd_bt_addr_any;

struct sockaddr_rfcomm {
  uint8_t rfcomm_len;
  uint8_t rfcomm_family;
  bdaddr_t rfcomm_bdaddr;
  uint8_t rfcomm_channel;
};

/** @brief Parse colon-separated Bluetooth address string into bdaddr_t. */
int bt_aton(const char *str, bdaddr_t *ba);
/** @brief Format bdaddr_t into colon-separated Bluetooth address string. */
const char *bt_ntoa(const bdaddr_t *ba, char *str);
/** @brief Look up Bluetooth host entry by name. */
struct hostent *bt_gethostbyname(const char *name);
/** @brief Look up Bluetooth host entry by address. */
struct hostent *bt_gethostbyaddr(const char *addr, int len, int type);
/** @brief Look up Bluetooth protocol entry by name. */
struct protoent *bt_getprotobyname(const char *name);
/** @brief Look up Bluetooth protocol entry by protocol number. */
struct protoent *bt_getprotobynumber(int proto);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_BLUETOOTH_H */
