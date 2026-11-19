#ifndef BSD_SDP_H
#define BSD_SDP_H

/**
 * @file sdp.h
 * @brief FreeBSD Bluetooth Service Discovery Protocol (<sdp.h>) polyfill.
 */

/* clang-format off */
#include <bluetooth.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ENOATTR
#define ENOATTR 87
#endif

#define SDP_SERVICE_CLASS_LAN_ACCESS_USING_PPP 0x1102
#define SDP_SERVICE_CLASS_DIALUP_NETWORKING 0x1103
#define SDP_SERVICE_CLASS_SERIAL_PORT 0x1101
#define SDP_SERVICE_CLASS_FAX 0x1111

#define SDP_UUID_PROTOCOL_L2CAP 0x0100
#define SDP_UUID_PROTOCOL_RFCOMM 0x0003

#define SDP_ATTR_INVALID 0x00
#define SDP_ATTR_OK 0x01
#define SDP_ATTR_TRUNCATED 0x02
#define SDP_ATTR_PROTOCOL_DESCRIPTOR_LIST 0x0004
#define SDP_ATTR_RANGE(lo, hi)                                                 \
  (((uint32_t)(uint16_t)(lo) << 16) | ((uint16_t)(hi)))

#define SDP_DATA_NIL 0x00
#define SDP_DATA_UINT8 0x08
#define SDP_DATA_UINT16 0x09
#define SDP_DATA_UINT32 0x0A
#define SDP_DATA_UUID16 0x19
#define SDP_DATA_UUID32 0x1A
#define SDP_DATA_UUID128 0x1C
#define SDP_DATA_SEQ8 0x35
#define SDP_DATA_SEQ16 0x36
#define SDP_DATA_SEQ32 0x37
#define SDP_DATA_ALT8 0x3D
#define SDP_DATA_ALT16 0x3E
#define SDP_DATA_ALT32 0x3F

#define SDP_GET8(b, cp)                                                        \
  do {                                                                         \
    (b) = *(const uint8_t *)(cp);                                              \
    (cp) += 1;                                                                 \
  } while (0)
#define SDP_GET16(s, cp)                                                       \
  do {                                                                         \
    const uint8_t *_p = (const uint8_t *)(cp);                                 \
    (s) = ((uint16_t)_p[0] << 8) | _p[1];                                      \
    (cp) += 2;                                                                 \
  } while (0)
#define SDP_GET32(l, cp)                                                       \
  do {                                                                         \
    const uint8_t *_p = (const uint8_t *)(cp);                                 \
    (l) = ((uint32_t)_p[0] << 24) | ((uint32_t)_p[1] << 16) |                  \
          ((uint32_t)_p[2] << 8) | _p[3];                                      \
    (cp) += 4;                                                                 \
  } while (0)

typedef struct sdp_attr {
  uint16_t flags;
  uint16_t attr;
  uint32_t vlen;
  uint8_t *value;
} sdp_attr_t;

struct sdp_sp_profile {
  uint8_t server_channel;
  uint8_t reserved[3];
};
typedef struct sdp_sp_profile sdp_sp_profile_t;

struct sdp_lan_profile {
  uint8_t server_channel;
  uint8_t load_factor;
  uint8_t reserved;
  uint8_t net_ip_subnet_radius;
  uint32_t net_ip_subnet;
};
typedef struct sdp_lan_profile sdp_lan_profile_t;

struct sdp_dun_profile {
  uint8_t server_channel;
  uint8_t audio_feedback_support;
  uint8_t reserved[2];
};
typedef struct sdp_dun_profile sdp_dun_profile_t;

struct sdp_ftrn_profile {
  uint8_t server_channel;
  uint8_t reserved[3];
};
typedef struct sdp_ftrn_profile sdp_ftrn_profile_t;

/** @brief Open remote Bluetooth SDP session between local and remote bdaddr. */
void *sdp_open(const bdaddr_t *l, const bdaddr_t *r);
/** @brief Open local Bluetooth SDP control socket session. */
void *sdp_open_local(const char *control);
/** @brief Close an open Bluetooth SDP session handle. */
int sdp_close(void *xs);
/** @brief Return last error code recorded on an SDP session handle. */
int sdp_error(void *xs);
/** @brief Search remote or local SDP database for service attributes. */
int sdp_search(void *xss, uint32_t plen, const uint16_t *sp, uint32_t alen,
               const uint32_t *ap, uint32_t vlen, sdp_attr_t *vp);
/** @brief Register a Bluetooth service record with the local SDP server. */
int sdp_register_service(void *xss, uint16_t uuid, const bdaddr_t *bdaddr,
                         const uint8_t *data, uint32_t datalen,
                         uint32_t *handle);
/** @brief Unregister a Bluetooth service record from the local SDP server. */
int sdp_unregister_service(void *xss, uint32_t handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SDP_H */
