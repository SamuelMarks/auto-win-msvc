#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)
/**
 * @file bsd-sys-sysctl.c
 * @brief Strict C89 implementation of BSD <sys/sysctl.h>.
 */

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
#elif !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
#include <unistd.h>
#endif
/* clang-format on */

#if defined(_MSC_VER)
__declspec(dllexport) int g_mock_sysconf_fail = 0;
#else
int g_mock_sysconf_fail = 0;
#endif

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)

static int sysctl_write_int(int val, void *oldp, size_t *oldlenp) {
  if (oldp == NULL) {
    if (oldlenp != NULL) {
      *oldlenp = sizeof(int);
    }
    return 0;
  }
  if (oldlenp == NULL || *oldlenp < sizeof(int)) {
    errno = ENOMEM;
    return -1;
  }
  memcpy(oldp, &val, sizeof(int));
  *oldlenp = sizeof(int);
  return 0;
}

static int sysctl_write_str(const char *str, void *oldp, size_t *oldlenp) {
  size_t slen = strlen(str) + 1;
  if (oldp == NULL) {
    if (oldlenp != NULL) {
      *oldlenp = slen;
    }
    return 0;
  }
  if (oldlenp == NULL || *oldlenp < slen) {
    errno = ENOMEM;
    return -1;
  }
  memcpy(oldp, str, slen);
  *oldlenp = slen;
  return 0;
}

int sysctl(const int *name, unsigned int namelen, void *oldp, size_t *oldlenp,
           const void *newp, size_t newlen) {
  (void)newp;
  (void)newlen;
  if (name == NULL || namelen < 2 || namelen > CTL_MAXNAME) {
    errno = EINVAL;
    return -1;
  }
  if (name[0] == CTL_HW) {
    if (name[1] == HW_NCPU) {
      int ncpu = 1;
      (void)bsd_sys_sysctl_get_ncpu(&ncpu);
      return sysctl_write_int(ncpu, oldp, oldlenp);
    }
    if (name[1] == HW_PAGESIZE) {
      return sysctl_write_int(4096, oldp, oldlenp);
    }
    if (name[1] == HW_BYTEORDER) {
      return sysctl_write_int(1234, oldp, oldlenp);
    }
    if (name[1] == HW_MACHINE || name[1] == HW_MACHINE_ARCH) {
#if defined(_M_X64) || defined(__x86_64__)
      return sysctl_write_str("amd64", oldp, oldlenp);
#elif defined(_M_ARM64) || defined(__aarch64__)
      return sysctl_write_str("arm64", oldp, oldlenp);
#else
      return sysctl_write_str("i386", oldp, oldlenp);
#endif
    }
    if (name[1] == HW_MODEL) {
      return sysctl_write_str("Generic x86 Processor", oldp, oldlenp);
    }
  } else if (name[0] == CTL_KERN) {
    if (name[1] == KERN_OSTYPE) {
      return sysctl_write_str("FreeBSD", oldp, oldlenp);
    }
    if (name[1] == KERN_OSRELEASE) {
      return sysctl_write_str("15.0-CURRENT", oldp, oldlenp);
    }
    if (name[1] == KERN_OSRELDATE) {
      return sysctl_write_int(1500000, oldp, oldlenp);
    }
    if (name[1] == KERN_ARGMAX) {
      return sysctl_write_int(262144, oldp, oldlenp);
    }
    if (name[1] == KERN_NGROUPS) {
      return sysctl_write_int(1024, oldp, oldlenp);
    }
    if (name[1] == KERN_HOSTNAME) {
      return sysctl_write_str("localhost", oldp, oldlenp);
    }
  }
  errno = ENOENT;
  return -1;
}

int sysctlnametomib(const char *name, int *mibp, size_t *sizep) {
  if (name == NULL || mibp == NULL || sizep == NULL || *sizep < 2) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(name, "hw.ncpu") == 0) {
    mibp[0] = CTL_HW;
    mibp[1] = HW_NCPU;
    *sizep = 2;
    return 0;
  }
  if (strcmp(name, "hw.pagesize") == 0) {
    mibp[0] = CTL_HW;
    mibp[1] = HW_PAGESIZE;
    *sizep = 2;
    return 0;
  }
  if (strcmp(name, "kern.ostype") == 0) {
    mibp[0] = CTL_KERN;
    mibp[1] = KERN_OSTYPE;
    *sizep = 2;
    return 0;
  }
  if (strcmp(name, "kern.osrelease") == 0) {
    mibp[0] = CTL_KERN;
    mibp[1] = KERN_OSRELEASE;
    *sizep = 2;
    return 0;
  }
  if (strcmp(name, "kern.osreldate") == 0) {
    mibp[0] = CTL_KERN;
    mibp[1] = KERN_OSRELDATE;
    *sizep = 2;
    return 0;
  }
  errno = ENOENT;
  return -1;
}

int sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
                 const void *newp, size_t newlen) {
  int mib[CTL_MAXNAME];
  size_t miblen = CTL_MAXNAME;
  if (name == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(name, "hw.ncpu") == 0) {
    int ncpu = 1;
#if defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    ncpu = (int)sysinfo.dwNumberOfProcessors;
    if (ncpu < 1) {
      ncpu = 1;
    }
#elif defined(_SC_NPROCESSORS_ONLN)
    if (!g_mock_sysconf_fail) {
      long count = sysconf(_SC_NPROCESSORS_ONLN);
      if (count > 0) {
        ncpu = (int)count;
      }
    }
#endif
    return sysctl_write_int(ncpu, oldp, oldlenp);
  }
  if (sysctlnametomib(name, mib, &miblen) == 0) {
    return sysctl(mib, (unsigned int)miblen, oldp, oldlenp, newp, newlen);
  }
  errno = ENOENT;
  return -1;
}
#endif /* !BSD && !__APPLE__ */

auto_win_msvc_error_t bsd_sys_sysctl_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

auto_win_msvc_error_t bsd_sys_sysctl_get_ncpu(int *out_ncpu) {
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__)
  int ncpu = 1;
  size_t len = sizeof(ncpu);
#elif defined(_SC_NPROCESSORS_ONLN)
  long count = -1;
#endif
  if (out_ncpu == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__)
  if (sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0) == 0) {
    *out_ncpu = ncpu;
    return AUTO_WIN_MSVC_SUCCESS;
  }
#elif defined(_SC_NPROCESSORS_ONLN)
  if (!g_mock_sysconf_fail) {
    count = sysconf(_SC_NPROCESSORS_ONLN);
  }
  if (count > 0) {
    *out_ncpu = (int)count;
    return AUTO_WIN_MSVC_SUCCESS;
  }
#endif
  *out_ncpu = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_sysctl;

#endif
typedef int make_iso_compilers_happy_mega_batch2_12;
