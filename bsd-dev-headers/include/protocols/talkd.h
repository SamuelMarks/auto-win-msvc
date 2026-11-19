#ifndef BSD_PROTOCOLS_TALKD_H
#define BSD_PROTOCOLS_TALKD_H

/**
 * @file talkd.h
 * @brief BSD talk daemon protocol (<protocols/talkd.h>).
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <protocols/talkd.h>
#else
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__) || !(defined(__GNUC__) || defined(__clang__))

#define NAME_SIZE 12
#define TTY_SIZE 16

struct tsockaddr {
  uint16_t sa_family;
  char sa_data[14];
};

typedef struct {
  unsigned char vers;
  unsigned char type;
  unsigned char answer;
  unsigned char pad;
  uint32_t id_num;
  struct tsockaddr addr;
  struct tsockaddr ctl_addr;
  int32_t pid;
  char l_name[NAME_SIZE];
  char r_name[NAME_SIZE];
  char r_tty[TTY_SIZE];
} CTL_MSG;

typedef struct {
  unsigned char vers;
  unsigned char type;
  unsigned char answer;
  unsigned char pad;
  uint32_t id_num;
  struct tsockaddr addr;
} CTL_RESPONSE;

#define TALK_VERSION 1
#define LEAVE_INVITE 0
#define LOOK_UP 1
#define DELETE 2
#define ANNOUNCE 3
#define SUCCESS 0
#define NOT_HERE 1
#define FAILED 2
#define MACHINE_UNKNOWN 3
#define PERMISSION_DENIED 4
#define UNKNOWN_REQUEST 5
#define BADVERSION 6
#define BADADDR 7
#define BADCTLADDR 8

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_PROTOCOLS_TALKD_H */
