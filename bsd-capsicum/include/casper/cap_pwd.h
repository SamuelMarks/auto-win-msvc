#ifndef CASPER_CAP_PWD_H
#define CASPER_CAP_PWD_H

/**
 * @file cap_pwd.h
 * @brief FreeBSD Casper system.pwd service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <pwd.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Read next password database entry via Casper pwd channel. */
static __inline struct passwd *cap_getpwent(cap_channel_t *chan) {
  (void)chan;
  return getpwent();
}

/** @brief Look up password entry by login name via Casper pwd channel. */
static __inline struct passwd *cap_getpwnam(cap_channel_t *chan,
                                            const char *login) {
  (void)chan;
  return getpwnam(login);
}

/** @brief Look up password entry by UID via Casper pwd channel. */
static __inline struct passwd *cap_getpwuid(cap_channel_t *chan, uid_t uid) {
  (void)chan;
  return getpwuid(uid);
}

/** @brief Reentrant password lookup by login name via Casper pwd channel. */
static __inline int cap_getpwnam_r(cap_channel_t *chan, const char *name,
                                   struct passwd *pwd, char *buffer,
                                   size_t bufsize, struct passwd **result) {
  (void)chan;
  return getpwnam_r(name, pwd, buffer, bufsize, result);
}

/** @brief Reentrant password lookup by UID via Casper pwd channel. */
static __inline int cap_getpwuid_r(cap_channel_t *chan, uid_t uid,
                                   struct passwd *pwd, char *buffer,
                                   size_t bufsize, struct passwd **result) {
  (void)chan;
  return getpwuid_r(uid, pwd, buffer, bufsize, result);
}

/** @brief Rewind password database with stayopen option via Casper pwd channel.
 */
static __inline int cap_setpassent(cap_channel_t *chan, int stayopen) {
  (void)chan;
  (void)stayopen;
  setpwent();
  return 1;
}

/** @brief Rewind password database via Casper pwd channel. */
static __inline void cap_setpwent(cap_channel_t *chan) {
  (void)chan;
  setpwent();
}

/** @brief Close password database via Casper pwd channel. */
static __inline void cap_endpwent(cap_channel_t *chan) {
  (void)chan;
  endpwent();
}

/** @brief Restrict allowed password database commands on Casper pwd channel. */
static __inline int cap_pwd_limit_cmds(cap_channel_t *chan,
                                       const char *const *cmds, size_t ncmds) {
  (void)chan;
  (void)cmds;
  (void)ncmds;
  return 0;
}

/** @brief Restrict returned struct passwd fields on Casper pwd channel. */
static __inline int cap_pwd_limit_fields(cap_channel_t *chan,
                                         const char *const *fields,
                                         size_t nfields) {
  (void)chan;
  (void)fields;
  (void)nfields;
  return 0;
}

/** @brief Restrict accessible users by login name or UID on Casper pwd channel.
 */
static __inline int cap_pwd_limit_users(cap_channel_t *chan,
                                        const char *const *names, size_t nnames,
                                        const uid_t *uids, size_t nuids) {
  (void)chan;
  (void)names;
  (void)nnames;
  (void)uids;
  (void)nuids;
  return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_PWD_H */
