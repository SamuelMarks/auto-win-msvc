/* clang-format off */
#ifndef BSD_STDLIB_H_
#define BSD_STDLIB_H_

/**
 * @file stdlib.h
 * @brief BSD standard library, sorting, random, and capability database
 * polyfills.
 */

#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
#ifndef _A_W_M_STDINT_TYPES_DEFINED
#define _A_W_M_STDINT_TYPES_DEFINED
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) && \
    !defined(_MODE_T_) && !defined(HAVE_MODE_T) && \
    (defined(_MSC_VER) || defined(_WIN32))
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
#define HAVE_MODE_T 1
/** @brief POSIX file mode type. */
typedef unsigned short mode_t;
#endif

#if defined(_MSC_VER)
#define AWM_BSD_LL __int64
#else
#define AWM_BSD_LL int64_t
#endif

#ifndef FPARSELN_UNESCESC
/** @brief fparseln flag: unescape escaped escape characters. */
#define FPARSELN_UNESCESC 0x01
/** @brief fparseln flag: unescape escaped newline characters. */
#define FPARSELN_UNESCCONT 0x02
/** @brief fparseln flag: unescape escaped comment characters. */
#define FPARSELN_UNESCCOMM 0x04
/** @brief fparseln flag: unescape any other escaped character. */
#define FPARSELN_UNESCREST 0x08
/** @brief fparseln flag: unescape all escaped characters. */
#define FPARSELN_UNESCALL 0x0f
#endif

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) && \
    !defined(__NetBSD__)

/**
 * @brief Reallocate memory safely with overflow checking.
 * @param[in] ptr Existing allocation pointer.
 * @param[in] nmemb Number of elements.
 * @param[in] size Size of each element.
 * @return Reallocated pointer, or NULL on overflow/error.
 */
void *reallocarray(void *ptr, size_t nmemb, size_t size);

/**
 * @brief Reallocate and zero-extend array safely.
 * @param[in] ptr Existing allocation pointer.
 * @param[in] oldnmemb Previous number of elements.
 * @param[in] newnmemb New number of elements.
 * @param[in] size Size of each element.
 * @return Reallocated pointer, or NULL on overflow/error.
 */
void *recallocarray(void *ptr, size_t oldnmemb, size_t newnmemb, size_t size);

/**
 * @brief Zero and free a memory allocation.
 * @param[in] ptr Allocation pointer.
 * @param[in] size Number of bytes to zero before freeing.
 */
void freezero(void *ptr, size_t size);

/**
 * @brief Retrieve the current program name.
 * @return Pointer to program name string.
 */
const char *getprogname(void);

/**
 * @brief Set the current program name.
 * @param[in] progname Program name or argv[0] path.
 */
void setprogname(const char *progname);

/**
 * @brief Reliably convert string value to an integer within bounds.
 * @param[in] numstr Input string.
 * @param[in] minval Minimum allowed value.
 * @param[in] maxval Maximum allowed value.
 * @param[out] errstrp Pointer receiving static error description, or NULL on
 * success.
 * @return Converted integer, or 0 on error.
 */
AWM_BSD_LL strtonum(const char *numstr, AWM_BSD_LL minval, AWM_BSD_LL maxval,
                    const char **errstrp);

/**
 * @brief Parse symbolic or octal file mode string into a bitmask state.
 * @param[in] p Mode string (e.g. "u+x" or "0755").
 * @return Allocated mode state pointer, or NULL on error.
 */
void *bsd_setmode(const char *p);

/**
 * @brief Apply parsed mode state to an initial file mode.
 * @param[in] bbox Opaque pointer returned by bsd_setmode().
 * @param[in] omode Original file mode.
 * @return Modified file mode.
 */
mode_t getmode(const void *bbox, mode_t omode);

/**
 * @brief Read a line from a stream without copying into a user buffer.
 * @param[in,out] stream Input FILE stream.
 * @param[out] len Pointer receiving length of line.
 * @return Pointer to internal line buffer, or NULL on EOF/error.
 */
char *fgetln(FILE *stream, size_t *len);

/**
 * @brief Read a wide-character line from a stream.
 * @param[in,out] stream Input FILE stream.
 * @param[out] lenp Pointer receiving length of wide line.
 * @return Pointer to internal wide line buffer, or NULL on EOF/error.
 */
wchar_t *fgetwln(FILE *stream, size_t *lenp);

/**
 * @brief Return the next logical line from a stream, handling continuations and
 * comments.
 * @param[in,out] fp Input FILE stream.
 * @param[out] size Optional pointer receiving length of parsed line.
 * @param[in,out] lineno Optional pointer incremented for each physical line
 * read.
 * @param[in] str Optional 3-character array [escape, continuation, comment].
 * @param[in] flags FPARSELN_* flags.
 * @return Newly allocated line string, or NULL on EOF/error.
 */
char *fparseln(FILE *fp, size_t *size, size_t *lineno, const char str[3],
               int flags);

/**
 * @brief Retrieve user-preferred block size from BLOCKSIZE/BLOCK_SIZE env.
 * @param[out] headerlenp Optional pointer receiving header string length.
 * @param[out] blocksizep Pointer receiving block size in bytes (minimum 512).
 * @return 0 on success.
 */
int getbsize(int *headerlenp, long *blocksizep);

/**
 * @brief Generate a 32-bit cryptographically strong pseudo-random number.
 * @return Random 32-bit unsigned integer.
 */
uint32_t arc4random(void);

/**
 * @brief Fill a buffer with pseudo-random bytes.
 * @param[out] buf Output buffer.
 * @param[in] nbytes Number of bytes to generate.
 */
void arc4random_buf(void *buf, size_t nbytes);

/**
 * @brief Generate a uniformly distributed random number less than upper_bound.
 * @param[in] upper_bound Exclusive upper bound.
 * @return Random integer in [0, upper_bound).
 */
uint32_t arc4random_uniform(uint32_t upper_bound);

/**
 * @brief Sort an array using merge sort.
 * @param[in,out] base Array base pointer.
 * @param[in] nmemb Number of elements.
 * @param[in] size Element size in bytes.
 * @param[in] cmp Comparison callback.
 * @return 0 on success, -1 on failure.
 */
int mergesort(void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *));

/**
 * @brief Sort an array using heap sort.
 * @param[in,out] base Array base pointer.
 * @param[in] nmemb Number of elements.
 * @param[in] size Element size in bytes.
 * @param[in] cmp Comparison callback.
 * @return 0 on success, -1 on failure.
 */
int heapsort(void *base, size_t nmemb, size_t size,
             int (*cmp)(const void *, const void *));

/**
 * @brief Sort an array of byte strings using radix sort.
 * @param[in,out] base Array of pointers to byte strings.
 * @param[in] nmemb Number of elements.
 * @param[in] table Optional 256-byte sort weight table, or NULL.
 * @param[in] endbyte Terminating byte value (typically 0).
 * @return 0 on success, -1 on failure.
 */
int radixsort(const unsigned char **base, int nmemb,
              const unsigned char *table, unsigned int endbyte);

/**
 * @brief Stable radix sort for an array of byte strings.
 * @param[in,out] base Array of pointers to byte strings.
 * @param[in] nmemb Number of elements.
 * @param[in] table Optional 256-byte sort weight table, or NULL.
 * @param[in] endbyte Terminating byte value (typically 0).
 * @return 0 on success, -1 on failure.
 */
int sradixsort(const unsigned char **base, int nmemb,
               const unsigned char *table, unsigned int endbyte);

/**
 * @brief Determine whether a response string is affirmative or negative.
 * @param[in] response User response string.
 * @return 1 if affirmative, 0 if negative, -1 if unrecognized.
 */
int rpmatch(const char *response);

/**
 * @brief Set process title for ps/top display (no-op on Windows/DOS).
 * @param[in] fmt Format string, or NULL.
 */
void setproctitle(const char *fmt, ...);

/**
 * @brief Fast process title update (no-op on Windows/DOS).
 * @param[in] fmt Format string, or NULL.
 */
void setproctitle_fast(const char *fmt, ...);

/**
 * @brief Detach process into background daemon.
 * @param[in] nochdir Non-zero to keep current working directory.
 * @param[in] noclose Non-zero to keep standard file descriptors open.
 * @return 0 on success, -1 on failure.
 */
int daemon(int nochdir, int noclose);

/**
 * @brief Get device name from device number and file type.
 * @param[in] dev Device number.
 * @param[in] type File type mask (S_IFCHR or S_IFBLK).
 * @return Device name string.
 */
char *devname(int dev, mode_t type);

/**
 * @brief Thread-safe get device name from device number and file type.
 * @param[in] dev Device number.
 * @param[in] type File type mask.
 * @param[out] buf Output buffer.
 * @param[in] len Buffer length.
 * @return Pointer to buf.
 */
char *devname_r(int dev, mode_t type, char *buf, int len);

/**
 * @brief Look up capability database entry.
 * @param[out] buf Pointer receiving allocated entry buffer.
 * @param[in] db_array Null-terminated array of database file paths.
 * @param[in] name Entry name.
 * @return 0 on success, -1 if not found, -2 on system error.
 */
int cgetent(char **buf, char **db_array, const char *name);

/**
 * @brief Locate capability field in entry buffer.
 * @param[in] buf Entry buffer.
 * @param[in] cap Capability name.
 * @param[in] type Delimiter character (e.g. ':' or '=').
 * @return Pointer to capability in buf, or NULL.
 */
char *cgetcap(char *buf, const char *cap, int type);

/**
 * @brief Retrieve numeric capability value.
 * @param[in] buf Entry buffer.
 * @param[in] cap Capability name.
 * @param[out] num Pointer receiving numeric value.
 * @return 0 on success, -1 if not found.
 */
int cgetnum(char *buf, const char *cap, long *num);

/**
 * @brief Retrieve string capability value.
 * @param[in] buf Entry buffer.
 * @param[in] cap Capability name.
 * @param[out] str Pointer receiving newly allocated string value.
 * @return Length of decoded string on success, -1 if not found.
 */
int cgetstr(char *buf, const char *cap, char **str);

/**
 * @brief Retrieve literal string capability value.
 * @param[in] buf Entry buffer.
 * @param[in] cap Capability name.
 * @param[out] str Pointer receiving newly allocated literal string.
 * @return Length of string on success, -1 if not found.
 */
int cgetustr(char *buf, const char *cap, char **str);

/**
 * @brief Sequential scan of capability database files.
 * @param[out] bp Pointer receiving allocated entry buffer.
 * @param[in] db_array Null-terminated array of database file paths.
 * @return 1 on record read, 0 on EOF, -1 on error.
 */
int cgetnext(char **bp, char **db_array);

/**
 * @brief Close sequential capability scan.
 * @return Always 0.
 */
int cgetclose(void);

/**
 * @brief Prepend an additional capability entry for cgetent lookups.
 * @param[in] ent Capability entry string.
 * @return 0 on success.
 */
int cgetset(const char *ent);

/**
 * @brief Match capability name alias in entry buffer.
 * @param[in] buf Entry buffer.
 * @param[in] name Capability record name to match.
 * @return 0 on match, -1 on mismatch.
 */
int cgetmatch(const char *buf, const char *name);

#else
void *bsd_setmode(const char *p);
#endif /* !BSD && !__APPLE__ */

/**
 * @brief Coverage helper for bsd-stdlib module.
 * @return Always 0.
 */
int bsd_stdlib_dummy_for_coverage(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_STDLIB_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
