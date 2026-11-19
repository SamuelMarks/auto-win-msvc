#ifndef SYS_ACCT_H
#define SYS_ACCT_H

/**
 * @file acct.h
 * @brief FreeBSD process accounting structures (<sys/acct.h>).
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
#include <sys/types.h>
#include <time.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define AC_COMM_LEN 16

#define AFORK 0x01
#define ASU 0x02
#define ACOMPAT 0x04
#define ACORE 0x08
#define AXSIG 0x10
#define ANVER 0x20

struct acctv3 {
  uint8_t ac_zero;
  uint8_t ac_version;
  uint16_t ac_len;
  char ac_comm[AC_COMM_LEN];
  float ac_utime;
  float ac_stime;
  float ac_etime;
  time_t ac_btime;
  uid_t ac_uid;
  gid_t ac_gid;
  float ac_mem;
  float ac_io;
  dev_t ac_tty;
  uint32_t ac_pad0;
  uint16_t ac_len2;
  union {
    uint32_t ac_align;
    uint8_t ac_flag;
  } ac_trailer;
};
#define ac_flagx ac_trailer.ac_flag

struct acctv2 {
  uint8_t ac_zero;
  uint8_t ac_version;
  uint16_t ac_len;
  char ac_comm[AC_COMM_LEN];
  float ac_utime;
  float ac_stime;
  float ac_etime;
  time_t ac_btime;
  uid_t ac_uid;
  gid_t ac_gid;
  float ac_mem;
  float ac_io;
  uint32_t ac_tty;
  uint16_t ac_len2;
  union {
    uint32_t ac_align;
    uint8_t ac_flag;
  } ac_trailer;
};

typedef uint16_t comp_t;

struct acctv1 {
  char ac_comm[AC_COMM_LEN];
  comp_t ac_utime;
  comp_t ac_stime;
  comp_t ac_etime;
  time_t ac_btime;
  uid_t ac_uid;
  gid_t ac_gid;
  uint16_t ac_mem;
  comp_t ac_io;
  uint32_t ac_tty;
  uint8_t ac_flag;
};

#define AHZV1 64

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_ACCT_H */
