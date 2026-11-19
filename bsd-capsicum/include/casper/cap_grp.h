#ifndef CASPER_CAP_GRP_H
#define CASPER_CAP_GRP_H

/**
 * @file cap_grp.h
 * @brief FreeBSD Casper system.grp service passthrough shim.
 */

/* clang-format off */
#include <libcasper.h>
#include <grp.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Read next group database entry via Casper grp channel. */
static __inline struct group *cap_getgrent(cap_channel_t *chan) {
  (void)chan;
  return getgrent();
}

/** @brief Look up group entry by name via Casper grp channel. */
static __inline struct group *cap_getgrnam(cap_channel_t *chan,
                                           const char *name) {
  (void)chan;
  return getgrnam(name);
}

/** @brief Look up group entry by GID via Casper grp channel. */
static __inline struct group *cap_getgrgid(cap_channel_t *chan, gid_t gid) {
  (void)chan;
  return getgrgid(gid);
}

/** @brief Reentrant group lookup by name via Casper grp channel. */
static __inline int cap_getgrnam_r(cap_channel_t *chan, const char *name,
                                   struct group *grp, char *buffer,
                                   size_t bufsize, struct group **result) {
  (void)chan;
  return getgrnam_r(name, grp, buffer, bufsize, result);
}

/** @brief Reentrant group lookup by GID via Casper grp channel. */
static __inline int cap_getgrgid_r(cap_channel_t *chan, gid_t gid,
                                   struct group *grp, char *buffer,
                                   size_t bufsize, struct group **result) {
  (void)chan;
  return getgrgid_r(gid, grp, buffer, bufsize, result);
}

/** @brief Rewind group database with stayopen option via Casper grp channel. */
static __inline int cap_setgroupent(cap_channel_t *chan, int stayopen) {
  (void)chan;
  (void)stayopen;
  setgrent();
  return 1;
}

/** @brief Rewind group database via Casper grp channel. */
static __inline void cap_setgrent(cap_channel_t *chan) {
  (void)chan;
  setgrent();
}

/** @brief Close group database via Casper grp channel. */
static __inline void cap_endgrent(cap_channel_t *chan) {
  (void)chan;
  endgrent();
}

/** @brief Restrict allowed group database commands on Casper grp channel. */
static __inline int cap_grp_limit_cmds(cap_channel_t *chan,
                                       const char *const *cmds, size_t ncmds) {
  (void)chan;
  (void)cmds;
  (void)ncmds;
  return 0;
}

/** @brief Restrict returned struct group fields on Casper grp channel. */
static __inline int cap_grp_limit_fields(cap_channel_t *chan,
                                         const char *const *fields,
                                         size_t nfields) {
  (void)chan;
  (void)fields;
  (void)nfields;
  return 0;
}

/** @brief Restrict accessible groups by name or GID on Casper grp channel. */
static __inline int cap_grp_limit_groups(cap_channel_t *chan,
                                         const char *const *names,
                                         size_t nnames, const gid_t *gids,
                                         size_t ngids) {
  (void)chan;
  (void)names;
  (void)nnames;
  (void)gids;
  (void)ngids;
  return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_GRP_H */
