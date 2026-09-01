/* clang-format off */
#include "bsd-sys-sysctl.h"
#include <errno.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32)

typedef unsigned long DWORD;
typedef int BOOL;
typedef void *PVOID;
typedef unsigned __int64 ULONGLONG;

#pragma pack(push, 8)
typedef struct _SYSTEM_INFO {
  DWORD dwOemId;
  DWORD dwPageSize;
  PVOID lpMinimumApplicationAddress;
  PVOID lpMaximumApplicationAddress;
  DWORD *dwActiveProcessorMask;
  DWORD dwNumberOfProcessors;
  DWORD dwProcessorType;
  DWORD dwAllocationGranularity;
  unsigned short wProcessorLevel;
  unsigned short wProcessorRevision;
} SYSTEM_INFO;

typedef struct _MEMORYSTATUSEX {
  DWORD dwLength;
  DWORD dwMemoryLoad;
  ULONGLONG ullTotalPhys;
  ULONGLONG ullAvailPhys;
  ULONGLONG ullTotalPageFile;
  ULONGLONG ullAvailPageFile;
  ULONGLONG ullTotalVirtual;
  ULONGLONG ullAvailVirtual;
  ULONGLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX;
#pragma pack(pop)

__declspec(dllimport) void __stdcall GetSystemInfo(SYSTEM_INFO *lpSystemInfo);
__declspec(dllimport)
BOOL __stdcall GlobalMemoryStatusEx(MEMORYSTATUSEX *lpBuffer);

error_type_t sysctl(const int *name, unsigned int namelen, void *oldp,
                    size_t *oldlenp, const void *newp, size_t newlen) {

  if (newp || newlen) {
    /* unused */
  }

  if (!name || namelen == 0) {
    return EINVAL;
  }

  if (name[0] == CTL_HW) {
    if (namelen > 1) {
      if (name[1] == HW_NCPU) {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        if (oldp && oldlenp) {
          if (*oldlenp >= sizeof(int)) {
            *(int *)oldp = (int)sysinfo.dwNumberOfProcessors;
            *oldlenp = sizeof(int);
            return ERR_NONE;
          } else {
            return ENOMEM;
          }
        }
        if (oldlenp)
          *oldlenp = sizeof(int);
        return ERR_NONE;
      } else if (name[1] == HW_PAGESIZE) {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        if (oldp && oldlenp) {
          if (*oldlenp >= sizeof(int)) {
            *(int *)oldp = (int)sysinfo.dwPageSize;
            *oldlenp = sizeof(int);
            return ERR_NONE;
          } else {
            return ENOMEM;
          }
        }
        if (oldlenp)
          *oldlenp = sizeof(int);
        return ERR_NONE;
      } else if (name[1] == HW_PHYSMEM || name[1] == HW_REALMEM ||
                 name[1] == HW_USERMEM || name[1] == HW_MEMSIZE) {
        MEMORYSTATUSEX memex;
        memex.dwLength = sizeof(memex);
        if (GlobalMemoryStatusEx(&memex)) {
          if (oldp && oldlenp) {
            if (*oldlenp >= sizeof(ULONGLONG)) {
              *(ULONGLONG *)oldp = memex.ullTotalPhys;
              *oldlenp = sizeof(ULONGLONG);
              return ERR_NONE;
            } else if (*oldlenp >= sizeof(unsigned int)) {
              *(unsigned int *)oldp =
                  (unsigned int)(memex.ullTotalPhys > (ULONGLONG)4294967295UL
                                     ? (ULONGLONG)4294967295UL
                                     : memex.ullTotalPhys);
              *oldlenp = sizeof(unsigned int);
              return ERR_NONE;
            } else {
              return ENOMEM;
            }
          }
          if (oldlenp)
            *oldlenp = sizeof(ULONGLONG);
          return ERR_NONE;
        }
      }
    }
  } else if (name[0] == CTL_VM) {
    if (namelen > 1 && name[1] == VM_OVERCOMMIT) {
      if (oldp && oldlenp) {
        if (*oldlenp >= sizeof(int)) {
          *(int *)oldp = 0; /* Not overcommitting on Windows by default */
          *oldlenp = sizeof(int);
          return ERR_NONE;
        } else {
          return ENOMEM;
        }
      }
      if (oldlenp)
        *oldlenp = sizeof(int);
      return ERR_NONE;
    }
  } else if (name[0] == CTL_KERN) {
    if (namelen > 2 && name[1] == KERN_IPC && name[2] == KIPC_SOMAXCONN) {
      if (oldp && oldlenp) {
        if (*oldlenp >= sizeof(int)) {
          *(int *)oldp = 0x7FFFFFFF; /* SOMAXCONN effectively unbounded */
          *oldlenp = sizeof(int);
          return ERR_NONE;
        } else {
          return ENOMEM;
        }
      }
      if (oldlenp)
        *oldlenp = sizeof(int);
      return ERR_NONE;
    }
  }

  return ENOENT;
}

error_type_t sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
                          const void *newp, size_t newlen) {
  int mib[4];
  unsigned int namelen = 0;
  error_type_t rc;

  if (!name) {
    return EINVAL;
  }

  if (strcmp(name, "hw.ncpu") == 0 || strcmp(name, "kern.smp.cpus") == 0) {
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    namelen = 2;
  } else if (strcmp(name, "hw.pagesize") == 0) {
    mib[0] = CTL_HW;
    mib[1] = HW_PAGESIZE;
    namelen = 2;
  } else if (strcmp(name, "hw.physmem") == 0) {
    mib[0] = CTL_HW;
    mib[1] = HW_PHYSMEM;
    namelen = 2;
  } else if (strcmp(name, "hw.realmem") == 0) {
    mib[0] = CTL_HW;
    mib[1] = HW_REALMEM;
    namelen = 2;
  } else if (strcmp(name, "hw.usermem") == 0) {
    mib[0] = CTL_HW;
    mib[1] = HW_USERMEM;
    namelen = 2;
  } else if (strcmp(name, "hw.memsize") == 0) {
    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    namelen = 2;
  } else if (strcmp(name, "vm.overcommit") == 0) {
    mib[0] = CTL_VM;
    mib[1] = VM_OVERCOMMIT;
    namelen = 2;
  } else if (strcmp(name, "kern.ipc.somaxconn") == 0) {
    mib[0] = CTL_KERN;
    mib[1] = KERN_IPC;
    mib[2] = KIPC_SOMAXCONN;
    namelen = 3;
  } else {
    return ENOENT;
  }

  rc = sysctl(mib, namelen, oldp, oldlenp, newp, newlen);
  return rc;
}

#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_sys_sysctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_sys_sysctl;
