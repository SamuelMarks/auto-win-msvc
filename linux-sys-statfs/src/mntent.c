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
__declspec(dllimport)
WIN_DWORD __stdcall GetLogicalDriveStringsA(WIN_DWORD nBufferLength,
                                            char *lpBuffer);
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

FILE *setmntent(const char *filename, const char *type) {
  WIN_DWORD len;
  (void)filename;
  (void)type;
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
  return (FILE *)&s_drives;
}

struct mntent *getmntent(FILE *stream) {
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

int endmntent(FILE *stream) {
  if (stream == NULL) {
    return 1;
  }
  s_drive_ptr = NULL;
  return 1;
}

char *hasmntopt(const struct mntent *mnt, const char *opt) {
  if (mnt == NULL || opt == NULL || mnt->mnt_opts == NULL) {
    return NULL;
  }
  return strstr(mnt->mnt_opts, opt);
}

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||    \
    defined(__NetBSD__)

static int s_apple_mnt_idx = 0;
static struct mntent s_apple_mnt;
static char s_apple_fsname[] = "/dev/disk0";
static char s_apple_dir[] = "/";
static char s_apple_type[] = "apfs";
static char s_apple_opts[] = "rw";

FILE *setmntent(const char *filename, const char *type) {
  if (filename == NULL || type == NULL) {
    return NULL;
  }
  s_apple_mnt_idx = 0;
  return (FILE *)&s_apple_mnt_idx;
}

struct mntent *getmntent(FILE *stream) {
  if (stream == NULL || s_apple_mnt_idx > 0) {
    return NULL;
  }
  s_apple_mnt_idx++;
  s_apple_mnt.mnt_fsname = s_apple_fsname;
  s_apple_mnt.mnt_dir = s_apple_dir;
  s_apple_mnt.mnt_type = s_apple_type;
  s_apple_mnt.mnt_opts = s_apple_opts;
  s_apple_mnt.mnt_freq = 0;
  s_apple_mnt.mnt_passno = 0;
  return &s_apple_mnt;
}

int endmntent(FILE *stream) {
  (void)stream;
  s_apple_mnt_idx = 0;
  return 1;
}

char *hasmntopt(const struct mntent *mnt, const char *opt) {
  if (mnt == NULL || opt == NULL || mnt->mnt_opts == NULL) {
    return NULL;
  }
  return strstr(mnt->mnt_opts, opt);
}

#endif
