#ifndef MNTENT_H
#define MNTENT_H

/**
 * @file mntent.h
 * @brief Polyfill for Linux mntent.h mounted filesystem description.
 */

/* clang-format off */
#include <stdio.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct mntent
 * @brief Structure describing a mounted filesystem.
 */
struct mntent {
  /** @brief File system name. */
  char *mnt_fsname;
  /** @brief Directory mount point. */
  char *mnt_dir;
  /** @brief File system type. */
  char *mnt_type;
  /** @brief Mount options. */
  char *mnt_opts;
  /** @brief Dump frequency in days. */
  int mnt_freq;
  /** @brief Pass number on parallel fsck. */
  int mnt_passno;
};

#ifndef MOUNTED
/** @brief Path to file describing mounted filesystems. */
#define MOUNTED "/etc/mtab"
#endif

/**
 * @brief Opens a filesystem description file.
 * @param filename Path to file.
 * @param type Open mode.
 * @return File pointer or NULL.
 */
FILE *setmntent(const char *filename, const char *type);

/**
 * @brief Reads the next filesystem description entry.
 * @param stream File stream pointer.
 * @return Pointer to mntent structure or NULL.
 */
struct mntent *getmntent(FILE *stream);

/**
 * @brief Closes filesystem description file.
 * @param stream File stream pointer.
 * @return 1 on success, 0 on failure.
 */
int endmntent(FILE *stream);

/**
 * @brief Searches mount options for matching option.
 * @param mnt Pointer to mntent structure.
 * @param opt Option substring to search.
 * @return Pointer to option substring or NULL.
 */
char *hasmntopt(const struct mntent *mnt, const char *opt);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MNTENT_H */
