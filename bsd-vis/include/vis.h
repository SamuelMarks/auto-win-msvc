#ifndef BSD_VIS_H
#define BSD_VIS_H

/**
 * @file vis.h
 * @brief BSD character encoding/decoding (<vis.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/* vis encoding flags */
#define VIS_OCTAL 0x0001
#define VIS_CSTYLE 0x0002
#define VIS_SP 0x0004
#define VIS_TAB 0x0008
#define VIS_NL 0x0010
#define VIS_WHITE (VIS_SP | VIS_TAB | VIS_NL)
#define VIS_SAFE 0x0020
#define VIS_NOSLASH 0x0040
#define VIS_HTTP1808 0x0080
#define VIS_HTTPSTYLE 0x0080
#define VIS_MIMESTYLE 0x0100
#define VIS_HTTP1866 0x0200
#define VIS_NOESCAPE 0x0400
#define _VIS_END 0x0800
#define VIS_GLOB 0x1000
#define VIS_SHELL 0x2000
#define VIS_META (VIS_WHITE | VIS_GLOB | VIS_SHELL)
#define VIS_NOLOCALE 0x4000

/* unvis return codes and flags */
#define UNVIS_VALID 1
#define UNVIS_VALIDPUSH 2
#define UNVIS_NOCHAR 3
#define UNVIS_SYNBAD -1
#define UNVIS_ERROR -2
#define UNVIS_END 1

/** @brief Encode a single character visually. */
char *vis(char *dst, int c, int flag, int nextc);
/** @brief Encode a single character into a bounded buffer. */
char *nvis(char *dst, size_t dlen, int c, int flag, int nextc);
/** @brief Encode a single character with extra characters. */
char *svis(char *dst, int c, int flag, int nextc, const char *extra);
/** @brief Encode a single character with extra characters into bounded buffer.
 */
char *snvis(char *dst, size_t dlen, int c, int flag, int nextc,
            const char *extra);

/** @brief Visually encode a NUL-terminated string. */
int strvis(char *dst, const char *src, int flag);
/** @brief Visually encode a string into dynamically allocated buffer. */
int stravis(char **outp, const char *src, int flag);
/** @brief Visually encode a string into bounded buffer. */
int strnvis(char *dst, const char *src, size_t dlen, int flag);
/** @brief Visually encode a string with extra characters. */
int strsvis(char *dst, const char *src, int flag, const char *extra);
/** @brief Visually encode a string with extra characters into bounded buffer.
 */
int strsnvis(char *dst, size_t dlen, const char *src, int flag,
             const char *extra);

/** @brief Visually encode a fixed-length buffer. */
int strvisx(char *dst, const char *src, size_t len, int flag);
/** @brief Visually encode a fixed-length buffer into bounded destination. */
int strnvisx(char *dst, size_t dlen, const char *src, size_t len, int flag);
/** @brief Visually encode a fixed-length buffer with extra options. */
int strenvisx(char *dst, size_t dlen, const char *src, size_t len, int flag,
              int *cerr_ptr);
/** @brief Visually encode a fixed-length buffer with extra characters. */
int strsvisx(char *dst, const char *src, size_t len, int flag,
             const char *extra);
/** @brief Visually encode a fixed-length buffer with extra chars into bounded
 * dst. */
int strsnvisx(char *dst, size_t dlen, const char *src, size_t len, int flag,
              const char *extra);

/** @brief Decode a visual representation of a character step-by-step. */
int unvis(char *cp, int c, int *astate, int flag);
/** @brief Decode a vis-encoded string. */
int strunvis(char *dst, const char *src);
/** @brief Decode a vis-encoded string into a bounded buffer. */
int strnunvis(char *dst, const char *src, size_t dlen);
/** @brief Decode a vis-encoded string with flags. */
int strunvisx(char *dst, const char *src, int flag);
/** @brief Decode a vis-encoded string with flags into bounded buffer. */
int strnunvisx(char *dst, size_t dlen, const char *src, int flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_VIS_H */
