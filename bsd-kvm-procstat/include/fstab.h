#ifndef BSD_FSTAB_H
#define BSD_FSTAB_H

/**
 * @file fstab.h
 * @brief BSD filesystem table (<fstab.h>) polyfill.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define _PATH_FSTAB "/etc/fstab"
#define FSTAB_RW "rw"
#define FSTAB_RQ "rq"
#define FSTAB_RO "ro"
#define FSTAB_SW "sw"
#define FSTAB_XX "xx"

/** @brief Filesystem table entry structure. */
struct fstab {
  char *fs_spec;
  char *fs_file;
  char *fs_vfstype;
  char *fs_mntops;
  char *fs_type;
  int fs_freq;
  int fs_passno;
};

/** @brief Read next entry from filesystem table. */
struct fstab *getfsent(void);
/** @brief Search filesystem table for special device name. */
struct fstab *getfsspec(const char *spec);
/** @brief Search filesystem table for mount point path. */
struct fstab *getfsfile(const char *file);
/** @brief Open or rewind filesystem table. */
int setfsent(void);
/** @brief Close filesystem table. */
void endfsent(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_FSTAB_H */
