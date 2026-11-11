#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)
/**
 * @file bsd-sys-endian.c
 * @brief Implementation of BSD <sys/endian.h> byte-order encoding and decoding.
 */

/* clang-format off */
#include "bsd-sys-endian.h"
#include <stddef.h>
/* clang-format on */

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)

uint16_t be16dec(const void *pp) {
  const uint8_t *p = (const uint8_t *)pp;
  return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

uint32_t be32dec(const void *pp) {
  const uint8_t *p = (const uint8_t *)pp;
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

uint64_t be64dec(const void *pp) {
  const uint8_t *p = (const uint8_t *)pp;
  return ((uint64_t)be32dec(p) << 32) | (uint64_t)be32dec(p + 4);
}

uint16_t le16dec(const void *pp) {
  const uint8_t *p = (const uint8_t *)pp;
  return (uint16_t)(((uint16_t)p[1] << 8) | p[0]);
}

uint32_t le32dec(const void *pp) {
  const uint8_t *p = (const uint8_t *)pp;
  return ((uint32_t)p[3] << 24) | ((uint32_t)p[2] << 16) |
         ((uint32_t)p[1] << 8) | (uint32_t)p[0];
}

uint64_t le64dec(const void *pp) {
  const uint8_t *p = (const uint8_t *)pp;
  return ((uint64_t)le32dec(p + 4) << 32) | (uint64_t)le32dec(p);
}

void be16enc(void *pp, uint16_t u) {
  uint8_t *p = (uint8_t *)pp;
  p[0] = (uint8_t)((u >> 8) & 0xffU);
  p[1] = (uint8_t)(u & 0xffU);
}

void be32enc(void *pp, uint32_t u) {
  uint8_t *p = (uint8_t *)pp;
  p[0] = (uint8_t)((u >> 24) & 0xffU);
  p[1] = (uint8_t)((u >> 16) & 0xffU);
  p[2] = (uint8_t)((u >> 8) & 0xffU);
  p[3] = (uint8_t)(u & 0xffU);
}

void be64enc(void *pp, uint64_t u) {
  uint8_t *p = (uint8_t *)pp;
  be32enc(p, (uint32_t)(u >> 32));
  be32enc(p + 4, (uint32_t)(u & 0xffffffffUL));
}

void le16enc(void *pp, uint16_t u) {
  uint8_t *p = (uint8_t *)pp;
  p[0] = (uint8_t)(u & 0xffU);
  p[1] = (uint8_t)((u >> 8) & 0xffU);
}

void le32enc(void *pp, uint32_t u) {
  uint8_t *p = (uint8_t *)pp;
  p[0] = (uint8_t)(u & 0xffU);
  p[1] = (uint8_t)((u >> 8) & 0xffU);
  p[2] = (uint8_t)((u >> 16) & 0xffU);
  p[3] = (uint8_t)((u >> 24) & 0xffU);
}

void le64enc(void *pp, uint64_t u) {
  uint8_t *p = (uint8_t *)pp;
  le32enc(p, (uint32_t)(u & 0xffffffffUL));
  le32enc(p + 4, (uint32_t)(u >> 32));
}

#endif /* !__FreeBSD__ && !__OpenBSD__ && !__NetBSD__ */

auto_win_msvc_error_t bsd_sys_endian_get_byte_order(int *out_byte_order) {
  if (out_byte_order == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_byte_order = _BYTE_ORDER;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_endian;

#endif
typedef int make_iso_compilers_happy_mega_batch2_6;
