#ifndef SYS_FNV_HASH_H
#define SYS_FNV_HASH_H

/**
 * @file fnv_hash.h
 * @brief Fowler-Noll-Vo 32-bit and 64-bit hash functions (<sys/fnv_hash.h>).
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 32-bit FNV hash accumulator type. */
typedef uint32_t Fnv32_t;
/** @brief 64-bit FNV hash accumulator type. */
typedef uint64_t Fnv64_t;

/** @brief 32-bit FNV-1 offset basis. */
#define FNV1_32_INIT ((Fnv32_t)0x811c9dc5UL)
/** @brief 64-bit FNV-1 offset basis. */
#define FNV1_64_INIT ((((Fnv64_t)0xcbf29ce4UL) << 32) | (Fnv64_t)0x84222325UL)
/** @brief 32-bit FNV prime multiplier. */
#define FNV_32_PRIME ((Fnv32_t)0x01000193UL)
/** @brief 64-bit FNV prime multiplier. */
#define FNV_64_PRIME ((((Fnv64_t)0x00000100UL) << 32) | (Fnv64_t)0x000001b3UL)

/** @brief Compute 32-bit FNV-1 hash over a byte buffer. */
static __inline Fnv32_t fnv_32_buf(const void *buf, size_t len, Fnv32_t hval) {
  const uint8_t *s = (const uint8_t *)buf;
  while (len-- != 0) {
    hval *= FNV_32_PRIME;
    hval ^= *s++;
  }
  return hval;
}

/** @brief Compute 32-bit FNV-1 hash over a NUL-terminated string. */
static __inline Fnv32_t fnv_32_str(const char *str, Fnv32_t hval) {
  const uint8_t *s = (const uint8_t *)str;
  Fnv32_t c;
  while ((c = *s++) != 0) {
    hval *= FNV_32_PRIME;
    hval ^= c;
  }
  return hval;
}

/** @brief Compute 64-bit FNV-1 hash over a byte buffer. */
static __inline Fnv64_t fnv_64_buf(const void *buf, size_t len, Fnv64_t hval) {
  const uint8_t *s = (const uint8_t *)buf;
  while (len-- != 0) {
    hval *= FNV_64_PRIME;
    hval ^= *s++;
  }
  return hval;
}

/** @brief Compute 64-bit FNV-1 hash over a NUL-terminated string. */
static __inline Fnv64_t fnv_64_str(const char *str, Fnv64_t hval) {
  const uint8_t *s = (const uint8_t *)str;
  Fnv64_t c;
  while ((c = *s++) != 0) {
    hval *= FNV_64_PRIME;
    hval ^= c;
  }
  return hval;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_FNV_HASH_H */
