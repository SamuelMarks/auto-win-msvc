/* clang-format off */
#include "mntent.h"
#include <stddef.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFINED_WIN32_FOR_TEST
typedef unsigned long WIN_DWORD;
#endif
#if !defined(MOCK_GETLOGICALDRIVESTRINGS)
__declspec(dllimport)
WIN_DWORD __stdcall GetLogicalDriveStringsA(WIN_DWORD nBufferLength,
                                            char *lpBuffer);
#endif

#ifdef __cplusplus
}
#endif

static char s_drive_strings[512];
static char *s_drive_ptr = NULL;
static struct mntent s_mnt;
static char s_mnt_fsname[16];
static char s_mnt_dir[16];
static char s_mnt_type[16];
static char s_mnt_opts[16];

FILE *setmntent(const char *filename, const char *type) {
  WIN_DWORD len;
  if (filename == NULL || type == NULL) {
    return NULL;
  }
  len = GetLogicalDriveStringsA((WIN_DWORD)sizeof(s_drive_strings),
                                s_drive_strings);
  if (len == 0 || len > (WIN_DWORD)sizeof(s_drive_strings)) {
    return NULL;
  }
  s_drive_ptr = s_drive_strings;
  return (FILE *)1;
}

struct mntent *getmntent(FILE *stream) {
  size_t nlen;
  if (stream == NULL || s_drive_ptr == NULL || *s_drive_ptr == '\0') {
    return NULL;
  }

#if defined(_MSC_VER) && !defined(DEFINED_MSC_VER_FOR_TEST)
  strcpy_s(s_mnt_dir, sizeof(s_mnt_dir), s_drive_ptr);
  strcpy_s(s_mnt_fsname, sizeof(s_mnt_fsname), s_drive_ptr);
  strcpy_s(s_mnt_type, sizeof(s_mnt_type), "ntfs");
  strcpy_s(s_mnt_opts, sizeof(s_mnt_opts), "rw");
#else
  strcpy(s_mnt_dir, s_drive_ptr);
  strcpy(s_mnt_fsname, s_drive_ptr);
  strcpy(s_mnt_type, "ntfs");
  strcpy(s_mnt_opts, "rw");
#endif

  nlen = strlen(s_mnt_fsname);
  if (s_mnt_fsname[nlen - 1] == '\\' || s_mnt_fsname[nlen - 1] == '/') {
    s_mnt_fsname[nlen - 1] = '\0';
  }

  s_mnt.mnt_dir = s_mnt_dir;
  s_mnt.mnt_fsname = s_mnt_fsname;
  s_mnt.mnt_type = s_mnt_type;
  s_mnt.mnt_opts = s_mnt_opts;
  s_mnt.mnt_freq = 0;
  s_mnt.mnt_passno = 0;

  s_drive_ptr += strlen(s_drive_ptr) + 1;
  return &s_mnt;
}

int endmntent(FILE *stream) {
  s_drive_ptr = NULL;
  if (stream == NULL) {
    return 0;
  }
  return 1;
}

char *hasmntopt(const struct mntent *mnt, const char *opt) {
  if (mnt == NULL || opt == NULL || mnt->mnt_opts == NULL) {
    return NULL;
  }
  return strstr(mnt->mnt_opts, opt);
}

#else

FILE *setmntent(const char *filename, const char *type) {
  (void)filename;
  (void)type;
  return NULL;
}

struct mntent *getmntent(FILE *stream) {
  (void)stream;
  return NULL;
}

int endmntent(FILE *stream) {
  (void)stream;
  return 1;
}

char *hasmntopt(const struct mntent *mnt, const char *opt) {
  (void)mnt;
  (void)opt;
  return NULL;
}

#endif
