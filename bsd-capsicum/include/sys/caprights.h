#ifndef SYS_CAPRIGHTS_H
#define SYS_CAPRIGHTS_H

/**
 * @file caprights.h
 * @brief FreeBSD Capsicum capability rights structure definition.
 */

/* clang-format off */
#include <sys/types.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CAP_RIGHTS_VERSION_00
/** @brief Capsicum rights version 0. */
#define CAP_RIGHTS_VERSION_00 0
#endif

#ifndef CAP_RIGHTS_VERSION
/** @brief Active Capsicum rights version. */
#define CAP_RIGHTS_VERSION CAP_RIGHTS_VERSION_00
#endif

#ifndef _CAP_RIGHTS_T_DEFINED
#define _CAP_RIGHTS_T_DEFINED
/** @brief Capsicum capability rights container. */
struct cap_rights {
  uint64_t cr_rights[CAP_RIGHTS_VERSION + 2];
};
typedef struct cap_rights cap_rights_t;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CAPRIGHTS_H */
