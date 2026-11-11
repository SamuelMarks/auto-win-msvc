#ifndef BSD_LOGIN_CAP_H
#define BSD_LOGIN_CAP_H

/**
 * @file login_cap.h
 * @brief FreeBSD login class capability database (<login_cap.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define LOGIN_SETLOGIN 0x0001
#define LOGIN_SETGROUP 0x0002
#define LOGIN_SETUSER 0x0004
#define LOGIN_SETRESOURCES 0x0008
#define LOGIN_SETPRIORITY 0x0010
#define LOGIN_SETENV 0x0020
#define LOGIN_SETPATH 0x0040
#define LOGIN_SETUMASK 0x0080
#define LOGIN_SETMAC 0x0100
#define LOGIN_SETCPUMASK 0x0200
#define LOGIN_SETLOGINCLASS 0x0400
#define LOGIN_SETALL 0x07ff

#define LOGIN_DEFCLASS "default"

struct passwd;

/** @brief Login class capability record structure. */
typedef struct login_cap {
  char *lc_class;
  char *lc_cap;
  char *lc_style;
} login_cap_t;

/**
 * @brief Retrieve login class entry by class name.
 * @param[in] name Class name, or NULL for default.
 * @return Allocated login_cap_t pointer.
 */
login_cap_t *login_getclass(const char *name);

/**
 * @brief Retrieve login class entry for a passwd struct.
 * @param[in] pwd User passwd structure.
 * @return Allocated login_cap_t pointer.
 */
login_cap_t *login_getpwclass(const struct passwd *pwd);

/**
 * @brief Retrieve user-specific login class overrides.
 * @param[in] pwd User passwd structure.
 * @return Allocated login_cap_t pointer.
 */
login_cap_t *login_getuserclass(const struct passwd *pwd);

/**
 * @brief Close and free a login_cap_t structure.
 * @param[in] lc Login class structure.
 */
void login_close(login_cap_t *lc);

/**
 * @brief Retrieve boolean capability from login class.
 * @param[in] lc Login class structure.
 * @param[in] cap Capability name.
 * @param[in] def Default value.
 * @return Boolean value (0 or 1).
 */
int login_getcapbool(login_cap_t *lc, const char *cap, int def);

/**
 * @brief Retrieve numeric capability from login class.
 * @param[in] lc Login class structure.
 * @param[in] cap Capability name.
 * @param[in] def Default value if missing.
 * @param[in] err Error value.
 * @return Capability number.
 */
int64_t login_getcapnum(login_cap_t *lc, const char *cap, int64_t def,
                        int64_t err);

/**
 * @brief Retrieve size capability from login class.
 * @param[in] lc Login class structure.
 * @param[in] cap Capability name.
 * @param[in] def Default size.
 * @param[in] err Error size.
 * @return Capability size in bytes.
 */
int64_t login_getcapsize(login_cap_t *lc, const char *cap, int64_t def,
                         int64_t err);

/**
 * @brief Retrieve string capability from login class.
 * @param[in] lc Login class structure.
 * @param[in] cap Capability name.
 * @param[in] def Default string.
 * @param[in] err Error string.
 * @return Capability string.
 */
const char *login_getcapstr(login_cap_t *lc, const char *cap, const char *def,
                            const char *err);

/**
 * @brief Retrieve time duration capability from login class.
 * @param[in] lc Login class structure.
 * @param[in] cap Capability name.
 * @param[in] def Default time in seconds.
 * @param[in] err Error value.
 * @return Capability time in seconds.
 */
int64_t login_getcaptime(login_cap_t *lc, const char *cap, int64_t def,
                         int64_t err);

/**
 * @brief Retrieve path capability from login class.
 * @param[in] lc Login class structure.
 * @param[in] cap Capability name.
 * @param[in] def Default path.
 * @param[in] err Error path.
 * @return Capability path string.
 */
const char *login_getpath(login_cap_t *lc, const char *cap, const char *def,
                          const char *err);

/**
 * @brief Apply login class context settings.
 * @param[in] lcname Login class name.
 * @param[in] flags LOGIN_SET* bitmask.
 * @return 0 on success.
 */
int setclasscontext(const char *lcname, unsigned int flags);

/**
 * @brief Apply user login context settings.
 * @param[in] lc Login class handle.
 * @param[in] pwd User passwd structure.
 * @param[in] uid User ID.
 * @param[in] flags LOGIN_SET* bitmask.
 * @return 0 on success.
 */
int setusercontext(login_cap_t *lc, const struct passwd *pwd, uid_t uid,
                   unsigned int flags);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_LOGIN_CAP_H */
