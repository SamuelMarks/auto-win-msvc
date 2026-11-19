#ifndef BSD_AR_H
#define BSD_AR_H

/**
 * @file ar.h
 * @brief BSD/SVR4 archive file format (<ar.h>) definitions.
 */

/* clang-format off */
#include <sys/cdefs.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define OARMAG1 0177555
#define OARMAG2 0177545
#define ARMAG "!<arch>\n"
#define SARMAG 8
#define AR_EFMT1 "#1/"
#define ARFMAG "`\n"

/** @brief Archive member header structure. */
struct ar_hdr {
  char ar_name[16];
  char ar_date[12];
  char ar_uid[6];
  char ar_gid[6];
  char ar_mode[8];
  char ar_size[10];
  char ar_fmag[2];
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_AR_H */
