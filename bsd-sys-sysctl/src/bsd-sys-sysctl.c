/* bsd-sys-sysctl.c - Strict C89 Implementation */
/* clang-format off */
#define BSD_SYS_SYSCTL_EXPORTS
#include "bsd-sys-sysctl.h"
#include <errno.h>
#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

#if defined(_MSC_VER)
__declspec(dllexport) int g_mock_sysconf_fail = 0;
#else
int g_mock_sysconf_fail = 0;
#endif

#if defined(_WIN32) || defined(_MSC_VER)
int sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
                 const void *newp, size_t newlen) {
  (void)newp;
  (void)newlen;
  if (name == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (strcmp(name, "hw.ncpu") == 0) {
    SYSTEM_INFO sysinfo;
    int ncpu;

    if (oldp == NULL || oldlenp == NULL) {
      if (oldlenp != NULL) {
        *oldlenp = sizeof(int);
      }
      return 0;
    }

    if (*oldlenp < sizeof(int)) {
      errno = ENOMEM;
      return -1;
    }

    GetSystemInfo(&sysinfo);
    ncpu = (int)sysinfo.dwNumberOfProcessors;

    if (ncpu < 1) {
      ncpu = 1;
    }

    memcpy(oldp, &ncpu, sizeof(int));
    *oldlenp = sizeof(int);

    return 0;
  }

  errno = ENOENT;
  return -1;
}
#endif

auto_win_msvc_error_t bsd_sys_sysctl_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

auto_win_msvc_error_t bsd_sys_sysctl_get_ncpu(int *out_ncpu) {
  if (out_ncpu == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }

#if defined(_WIN32)
  {
    int ncpu;
    size_t len = sizeof(ncpu);
    if (sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0) == 0) {
      *out_ncpu = ncpu;
      return AUTO_WIN_MSVC_SUCCESS;
    }
  }
#elif defined(_SC_NPROCESSORS_ONLN)
  {
    long count = -1;
    if (!g_mock_sysconf_fail) {
      count = sysconf(_SC_NPROCESSORS_ONLN);
    }
    if (count > 0) {
      *out_ncpu = (int)count;
      return AUTO_WIN_MSVC_SUCCESS;
    }
  }
#endif

  *out_ncpu = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_sysctl;
