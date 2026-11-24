/**
 * @file mntent.c
 * @brief Polyfill implementation for Linux <mntent.h> mounted filesystem
 * enumeration.
 */

/* clang-format off */
#include "mntent.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32) && !defined(_MSC_VER)
#include <sys/stat.h>
#endif
/* clang-format on */

#if defined(_WIN32) || defined(_MSC_VER)

#ifndef MOCK_GETLOGICALDRIVESTRINGS
typedef unsigned long WIN_DWORD;
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) WIN_DWORD __stdcall
GetLogicalDriveStringsA(WIN_DWORD nBufferLength, char *lpBuffer);
#ifdef __cplusplus
}
#endif
#endif

static char s_drives[512];
static const char *s_drive_ptr = NULL;
static struct mntent s_mnt;
static char s_fsname[16];
static char s_dir[16];
static char s_type[16];
static char s_opts[16];

static FILE *setmntent_win_impl(const char *filename, const char *type) {
  WIN_DWORD len;
  if (filename == NULL || type == NULL) {
    return NULL;
  }
  len = GetLogicalDriveStringsA((WIN_DWORD)(sizeof(s_drives) - 2), s_drives);
  if (len == 0 || len >= sizeof(s_drives)) {
    s_drive_ptr = NULL;
    return NULL;
  }
  s_drives[len] = '\0';
  s_drives[len + 1] = '\0';
  s_drive_ptr = s_drives;
  {
    static void *s_win_mnt_sentinel = NULL;
    return (FILE *)(void *)&s_win_mnt_sentinel;
  }
}

static struct mntent *getmntent_win_impl(FILE *stream) {
  size_t len;
  if (stream == NULL || s_drive_ptr == NULL || *s_drive_ptr == '\0') {
    return NULL;
  }
  len = strlen(s_drive_ptr);
  if (len >= sizeof(s_fsname)) {
    len = sizeof(s_fsname) - 1;
  }
  memcpy(s_fsname, s_drive_ptr, len);
  s_fsname[len] = '\0';
  if (len > 1 && (s_fsname[len - 1] == '\\' || s_fsname[len - 1] == '/')) {
    s_fsname[len - 1] = '\0';
  }

  memcpy(s_dir, s_drive_ptr, len);
  s_dir[len] = '\0';

#if defined(_MSC_VER)
  strcpy_s(s_type, sizeof(s_type), "ntfs");
  strcpy_s(s_opts, sizeof(s_opts), "rw");
#else
  strcpy(s_type, "ntfs");
  strcpy(s_opts, "rw");
#endif

  s_mnt.mnt_fsname = s_fsname;
  s_mnt.mnt_dir = s_dir;
  s_mnt.mnt_type = s_type;
  s_mnt.mnt_opts = s_opts;
  s_mnt.mnt_freq = 0;
  s_mnt.mnt_passno = 0;

  s_drive_ptr += strlen(s_drive_ptr) + 1;
  return &s_mnt;
}

static int endmntent_win_impl(FILE *stream) {
  if (stream == NULL) {
    return 1;
  }
  s_drive_ptr = NULL;
  return 1;
}

static char *hasmntopt_win_impl(const struct mntent *mnt, const char *opt) {
  if (mnt == NULL || opt == NULL || mnt->mnt_opts == NULL) {
    return NULL;
  }
  return strstr(mnt->mnt_opts, opt);
}

/**
 * @brief Opens the filesystem description table for enumeration.
 * @param[in] filename Path to mount table file.
 * @param[in] type Open mode string.
 * @return Non-NULL opaque FILE pointer on success, or NULL on error.
 */
FILE *setmntent(const char *filename, const char *type) {
  return setmntent_win_impl(filename, type);
}

/**
 * @brief Reads the next entry from the mounted filesystem table.
 * @param[in] stream Stream returned by setmntent().
 * @return Pointer to static mntent structure, or NULL at EOF/error.
 */
struct mntent *getmntent(FILE *stream) { return getmntent_win_impl(stream); }

/**
 * @brief Closes the filesystem description table stream.
 * @param[in] stream Stream returned by setmntent().
 * @return Always 1.
 */
int endmntent(FILE *stream) { return endmntent_win_impl(stream); }

/**
 * @brief Scans mnt_opts for a matching substring option.
 * @param[in] mnt Mount table entry.
 * @param[in] opt Option substring to locate.
 * @return Pointer to match within mnt_opts, or NULL if absent.
 */
char *hasmntopt(const struct mntent *mnt, const char *opt) {
  return hasmntopt_win_impl(mnt, opt);
}

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||    \
    defined(__NetBSD__) || defined(__WATCOMC__) || defined(__MSDOS__) ||       \
    defined(__DOS__)

static int s_apple_mnt_idx = 0;
static struct mntent s_apple_mnt;
static void *s_apple_mnt_sentinel = NULL;
static char s_apple_fsname[] = "/dev/disk0";
static char s_apple_dir[] = "/";
static char s_apple_type[] = "apfs";
static char s_apple_opts[] = "rw";

/**
 * @brief Opens the filesystem description table on BSD/macOS/DOS.
 * @param[in] filename Path to mount table file.
 * @param[in] type Open mode string.
 * @return Non-NULL opaque FILE pointer on success, or NULL on error.
 */
FILE *setmntent(const char *filename, const char *type) {
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (filename == NULL || type == NULL) {
    return NULL;
  }
#else
  (void)filename;
  (void)type;
#endif
  s_apple_mnt_idx = 0;
  return (FILE *)(void *)&s_apple_mnt_sentinel;
}

/**
 * @brief Reads the next entry from the mounted filesystem table on
 * BSD/macOS/DOS.
 * @param[in] stream Stream returned by setmntent().
 * @return Pointer to static mntent structure, or NULL at EOF/error.
 */
struct mntent *getmntent(FILE *stream) {
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (stream == NULL || s_apple_mnt_idx > 0) {
    return NULL;
  }
#else
  (void)stream;
  if (s_apple_mnt_idx > 0) {
    return NULL;
  }
#endif
  s_apple_mnt_idx++;
  s_apple_mnt.mnt_fsname = s_apple_fsname;
  s_apple_mnt.mnt_dir = s_apple_dir;
  s_apple_mnt.mnt_type = s_apple_type;
  s_apple_mnt.mnt_opts = s_apple_opts;
  s_apple_mnt.mnt_freq = 0;
  s_apple_mnt.mnt_passno = 0;
  return &s_apple_mnt;
}

/**
 * @brief Closes the filesystem description table stream on BSD/macOS/DOS.
 * @param[in] stream Stream returned by setmntent().
 * @return Always 1.
 */
int endmntent(FILE *stream) {
  (void)stream;
  s_apple_mnt_idx = 0;
  return 1;
}

#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
/**
 * @brief Scans mnt_opts for a matching substring option on BSD/macOS/DOS.
 * @param[in] mnt Mount table entry.
 * @param[in] opt Option substring to locate.
 * @return Pointer to match within mnt_opts, or NULL if absent.
 */
char *hasmntopt(const struct mntent *mnt, const char *opt) {
  if (mnt == NULL || opt == NULL || mnt->mnt_opts == NULL) {
    return NULL;
  }
  return strstr(mnt->mnt_opts, opt);
}
#endif

#elif defined(__CYGWIN__)

/**
 * @brief Scans mnt_opts for a matching substring option on Cygwin.
 * @param[in] mnt Mount table entry.
 * @param[in] opt Option substring to locate.
 * @return Pointer to match within mnt_opts, or NULL if absent.
 */
char *hasmntopt(const struct mntent *mnt, const char *opt) {
  if (mnt == NULL || opt == NULL || mnt->mnt_opts == NULL) {
    return NULL;
  }
  return strstr(mnt->mnt_opts, opt);
}

#endif

typedef int make_iso_compilers_happy_tu_mntent;
