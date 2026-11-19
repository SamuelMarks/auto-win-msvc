#ifndef BSD_BITSTRING_H
#define BSD_BITSTRING_H

/**
 * @file bitstring.h
 * @brief BSD bit-string manipulation macros (<bitstring.h>).
 */

/* clang-format off */
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSD_BITSTRING_CALLOC
/** @brief Allocator hook for bit_alloc (overridable for OOM testing). */
#define BSD_BITSTRING_CALLOC(nmemb, size) calloc((nmemb), (size))
#endif

/** @brief Underlying byte type for bitstrings. */
typedef unsigned char bitstr_t;

/** @brief Compute byte offset of a bit index. */
#define _bit_byte(bit) ((bit) >> 3)
/** @brief Compute bit mask within a byte for a bit index. */
#define _bit_mask(bit) (1 << ((bit) & 0x7))

/** @brief Number of bytes needed to hold nbits bits. */
#define bitstr_size(nbits) ((((nbits) - 1) >> 3) + 1)

/** @brief Dynamically allocate a zeroed bitstring of nbits bits. */
#define bit_alloc(nbits)                                                       \
  ((bitstr_t *)BSD_BITSTRING_CALLOC((size_t)bitstr_size(nbits),                \
                                    sizeof(bitstr_t)))

/** @brief Declare a bitstring array of nbits bits on stack or in struct. */
#define bit_decl(name, nbits) ((name)[bitstr_size(nbits)])

/** @brief Test if bit is set in bitstring name. */
#define bit_test(name, bit) ((name)[_bit_byte(bit)] & _bit_mask(bit))

/** @brief Set bit in bitstring name. */
#define bit_set(name, bit) ((name)[_bit_byte(bit)] |= (bitstr_t)_bit_mask(bit))

/** @brief Clear bit in bitstring name. */
#define bit_clear(name, bit)                                                   \
  ((name)[_bit_byte(bit)] &= (bitstr_t)~_bit_mask(bit))

/**
 * @brief Internal inline helper to clear bits in [start, stop].
 * @param name Pointer to bitstring array.
 * @param start First bit index to clear.
 * @param stop Last bit index to clear (inclusive).
 */
static __inline void __awm_bit_nclear(bitstr_t *name, int start, int stop) {
  int _i;
  for (_i = start; _i <= stop; _i++) {
    bit_clear(name, _i);
  }
}
/** @brief Clear bits start..stop inclusive in bitstring name. */
#define bit_nclear(name, start, stop) __awm_bit_nclear((name), (start), (stop))

/**
 * @brief Internal inline helper to set bits in [start, stop].
 * @param name Pointer to bitstring array.
 * @param start First bit index to set.
 * @param stop Last bit index to set (inclusive).
 */
static __inline void __awm_bit_nset(bitstr_t *name, int start, int stop) {
  int _i;
  for (_i = start; _i <= stop; _i++) {
    bit_set(name, _i);
  }
}
/** @brief Set bits start..stop inclusive in bitstring name. */
#define bit_nset(name, start, stop) __awm_bit_nset((name), (start), (stop))

/**
 * @brief Internal inline helper to find first clear bit in [0, nbits).
 * @param name Pointer to bitstring array.
 * @param nbits Total number of bits to inspect.
 * @param[out] value Receives index of first clear bit, or -1 if none.
 */
static __inline void __awm_bit_ffc(const bitstr_t *name, int nbits,
                                   int *value) {
  int _awm_i;
  int _awm_res = -1;
  for (_awm_i = 0; _awm_i < nbits; _awm_i++) {
    if (!bit_test(name, _awm_i)) {
      _awm_res = _awm_i;
      break;
    }
  }
  *value = _awm_res;
}
/** @brief Find first bit clear in name of nbits length (-1 if none). */
#define bit_ffc(name, nbits, value) __awm_bit_ffc((name), (nbits), (value))

/**
 * @brief Internal inline helper to find first set bit in [0, nbits).
 * @param name Pointer to bitstring array.
 * @param nbits Total number of bits to inspect.
 * @param[out] value Receives index of first set bit, or -1 if none.
 */
static __inline void __awm_bit_ffs(const bitstr_t *name, int nbits,
                                   int *value) {
  int _awm_i;
  int _awm_res = -1;
  for (_awm_i = 0; _awm_i < nbits; _awm_i++) {
    if (bit_test(name, _awm_i)) {
      _awm_res = _awm_i;
      break;
    }
  }
  *value = _awm_res;
}
/** @brief Find first bit set in name of nbits length (-1 if none). */
#define bit_ffs(name, nbits, value) __awm_bit_ffs((name), (nbits), (value))

/**
 * @brief Compilation unit anchor symbol for bsd-bitstring coverage.
 * @return Always 0.
 */
int bsd_bitstring_dummy_for_coverage(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_BITSTRING_H */
