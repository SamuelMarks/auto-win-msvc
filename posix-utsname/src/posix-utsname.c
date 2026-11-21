/**
 * @file posix-utsname.c
 * @brief Implementation of POSIX uname() for MSVC and Windows.
 */

/* clang-format off */
#include "posix-utsname.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#ifndef EFAULT
#define EFAULT 14
#endif

#ifndef EINVAL
#define EINVAL 38
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define UTS_WINAPI __stdcall
/** @brief DLL import attribute on MSVC. */
#define UTS_DECLSPEC_IMPORT __declspec(dllimport)
#else
/** @brief stdcall calling convention on MinGW/GCC. */
#define UTS_WINAPI __attribute__((stdcall))
/** @brief DLL import attribute on MinGW/GCC. */
#define UTS_DECLSPEC_IMPORT __attribute__((dllimport))
#endif

typedef unsigned long UTS_DWORD;
typedef unsigned short UTS_WORD;
typedef int UTS_BOOL;
typedef void *UTS_HANDLE;
typedef UTS_HANDLE UTS_HMODULE;
typedef const char *UTS_LPCSTR;
typedef char *UTS_LPSTR;
typedef UTS_DWORD *UTS_LPDWORD;
typedef long UTS_LONG;
typedef unsigned short UTS_WCHAR;
typedef unsigned long UTS_ULONG;

typedef struct _UTS_OSVERSIONINFOA {
  UTS_DWORD dwOSVersionInfoSize;
  UTS_DWORD dwMajorVersion;
  UTS_DWORD dwMinorVersion;
  UTS_DWORD dwBuildNumber;
  UTS_DWORD dwPlatformId;
  char szCSDVersion[128];
} UTS_OSVERSIONINFOA;

typedef struct _UTS_SYSTEM_INFO {
  union {
    UTS_DWORD dwOemId;
    struct {
      UTS_WORD wProcessorArchitecture;
      UTS_WORD wReserved;
    } s;
  } u;
  UTS_DWORD dwPageSize;
  void *lpMinimumApplicationAddress;
  void *lpMaximumApplicationAddress;
  UTS_DWORD *dwActiveProcessorMask;
  UTS_DWORD dwNumberOfProcessors;
  UTS_DWORD dwProcessorType;
  UTS_DWORD dwAllocationGranularity;
  UTS_WORD wProcessorLevel;
  UTS_WORD wProcessorRevision;
} UTS_SYSTEM_INFO;

typedef struct _UTS_RTL_OSVERSIONINFOW {
  UTS_ULONG dwOSVersionInfoSize;
  UTS_ULONG dwMajorVersion;
  UTS_ULONG dwMinorVersion;
  UTS_ULONG dwBuildNumber;
  UTS_ULONG dwPlatformId;
  UTS_WCHAR szCSDVersion[128];
} UTS_RTL_OSVERSIONINFOW;

typedef UTS_LONG(UTS_WINAPI *RtlGetVersion_Func)(UTS_RTL_OSVERSIONINFOW *);
typedef UTS_LONG(UTS_WINAPI *UTS_FARPROC)(void);

/** @brief Retrieves NetBIOS name of local computer. */
UTS_DECLSPEC_IMPORT UTS_BOOL UTS_WINAPI GetComputerNameA(UTS_LPSTR lpBuffer,
                                                         UTS_LPDWORD nSize);
/** @brief Retrieves module handle for loaded DLL. */
UTS_DECLSPEC_IMPORT UTS_HMODULE UTS_WINAPI
GetModuleHandleA(UTS_LPCSTR lpModuleName);
/** @brief Retrieves exported procedure address from DLL. */
UTS_DECLSPEC_IMPORT UTS_FARPROC UTS_WINAPI
GetProcAddress(UTS_HMODULE hModule, UTS_LPCSTR lpProcName);
/** @brief Retrieves hardware processor architecture information. */
UTS_DECLSPEC_IMPORT void UTS_WINAPI
GetSystemInfo(UTS_SYSTEM_INFO *lpSystemInfo);
/** @brief Retrieves Windows version information. */
UTS_DECLSPEC_IMPORT UTS_BOOL UTS_WINAPI
GetVersionExA(UTS_OSVERSIONINFOA *lpVersionInformation);

#define UTS_PROCESSOR_ARCHITECTURE_INTEL 0
#define UTS_PROCESSOR_ARCHITECTURE_ARM 5
#define UTS_PROCESSOR_ARCHITECTURE_IA64 6
#define UTS_PROCESSOR_ARCHITECTURE_AMD64 9
#define UTS_PROCESSOR_ARCHITECTURE_ARM64 12

#ifdef __cplusplus
}
#endif

#ifndef UTS_MOCK_GetComputerNameA
#define UTS_MOCK_GetComputerNameA GetComputerNameA
#endif
#ifndef UTS_MOCK_GetModuleHandleA
#define UTS_MOCK_GetModuleHandleA GetModuleHandleA
#endif
#ifndef UTS_MOCK_GetProcAddress
#define UTS_MOCK_GetProcAddress GetProcAddress
#endif
#ifndef UTS_MOCK_GetSystemInfo
#define UTS_MOCK_GetSystemInfo GetSystemInfo
#endif
#ifndef UTS_MOCK_GetVersionExA
#define UTS_MOCK_GetVersionExA GetVersionExA
#endif

#ifndef UTS_NUM_FORMAT
#define UTS_NUM_FORMAT "%lu"
#endif

static void uts_copy_str(char *dst, const char *src) {
#if defined(_MSC_VER)
  strncpy_s(dst, _UTSNAME_LENGTH, src, _TRUNCATE);
#else
  strncpy(dst, src, _UTSNAME_LENGTH - 1);
  dst[_UTSNAME_LENGTH - 1] = '\0';
#endif
}

static int uname_win_impl(struct utsname *name) {
  UTS_OSVERSIONINFOA osvi;
  UTS_SYSTEM_INFO si;
  UTS_DWORD nodename_len = _UTSNAME_LENGTH;
  UTS_HMODULE hModNtdll;
  RtlGetVersion_Func pRtlGetVersion;
  UTS_RTL_OSVERSIONINFOW rosvi;

  if (!name) {
    errno = EFAULT;
    return -1;
  }

  memset(name, 0, sizeof(struct utsname));
  uts_copy_str(name->sysname, "Windows_NT");

  if (!UTS_MOCK_GetComputerNameA(name->nodename, &nodename_len)) {
    uts_copy_str(name->nodename, "unknown");
  }

  memset(&osvi, 0, sizeof(UTS_OSVERSIONINFOA));
  osvi.dwOSVersionInfoSize = sizeof(UTS_OSVERSIONINFOA);

  hModNtdll = UTS_MOCK_GetModuleHandleA("ntdll.dll");
  pRtlGetVersion = NULL;
  if (hModNtdll) {
    pRtlGetVersion = (RtlGetVersion_Func)(size_t)UTS_MOCK_GetProcAddress(
        hModNtdll, "RtlGetVersion");
  }

  if (pRtlGetVersion) {
    memset(&rosvi, 0, sizeof(UTS_RTL_OSVERSIONINFOW));
    rosvi.dwOSVersionInfoSize = sizeof(UTS_RTL_OSVERSIONINFOW);
    if (pRtlGetVersion(&rosvi) == 0) {
      osvi.dwMajorVersion = rosvi.dwMajorVersion;
      osvi.dwMinorVersion = rosvi.dwMinorVersion;
      osvi.dwBuildNumber = rosvi.dwBuildNumber;
    } else {
      osvi.dwMajorVersion = 0;
    }
  }

  if (osvi.dwMajorVersion == 0) {
    (void)UTS_MOCK_GetVersionExA(&osvi);
  }

#if defined(_MSC_VER)
  (void)sprintf_s(
      name->release, _UTSNAME_LENGTH, UTS_NUM_FORMAT "." UTS_NUM_FORMAT,
      (unsigned long)osvi.dwMajorVersion, (unsigned long)osvi.dwMinorVersion);
  (void)sprintf_s(name->version, _UTSNAME_LENGTH, UTS_NUM_FORMAT,
                  (unsigned long)osvi.dwBuildNumber);
#else
  (void)sprintf(name->release, UTS_NUM_FORMAT "." UTS_NUM_FORMAT,
                (unsigned long)osvi.dwMajorVersion,
                (unsigned long)osvi.dwMinorVersion);
  (void)sprintf(name->version, UTS_NUM_FORMAT,
                (unsigned long)osvi.dwBuildNumber);
#endif

  UTS_MOCK_GetSystemInfo(&si);
  switch (si.u.s.wProcessorArchitecture) {
  case UTS_PROCESSOR_ARCHITECTURE_INTEL:
    uts_copy_str(name->machine, "i686");
    break;
  case UTS_PROCESSOR_ARCHITECTURE_AMD64:
    uts_copy_str(name->machine, "x86_64");
    break;
  case UTS_PROCESSOR_ARCHITECTURE_ARM:
    uts_copy_str(name->machine, "arm");
    break;
  case UTS_PROCESSOR_ARCHITECTURE_ARM64:
    uts_copy_str(name->machine, "aarch64");
    break;
  case UTS_PROCESSOR_ARCHITECTURE_IA64:
    uts_copy_str(name->machine, "ia64");
    break;
  default:
    uts_copy_str(name->machine, "unknown");
    break;
  }

  return 0;
}

/**
 * @brief Get system identification.
 * @param[out] name Pointer to utsname structure to populate.
 * @return 0 on success, or -1 on error with errno set.
 */
int uname(struct utsname *name) { return uname_win_impl(name); }

#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)

/**
 * @brief Get system identification on MS-DOS / OpenWatcom.
 */
int uname(struct utsname *name) {
  if (!name) {
    errno = EFAULT;
    return -1;
  }
  memset(name, 0, sizeof(struct utsname));
  strncpy(name->sysname, "MS-DOS", _UTSNAME_LENGTH - 1);
  strncpy(name->nodename, "dos", _UTSNAME_LENGTH - 1);
#if defined(__WATCOMC__)
  sprintf(name->release, "%u.%02u", (unsigned)_osmajor, (unsigned)_osminor);
#else
  strncpy(name->release, "4.00", _UTSNAME_LENGTH - 1);
#endif
  strncpy(name->version, name->release, _UTSNAME_LENGTH - 1);
#if defined(_M_I386) || defined(__386__)
  strncpy(name->machine, "i386", _UTSNAME_LENGTH - 1);
#else
  strncpy(name->machine, "i86", _UTSNAME_LENGTH - 1);
#endif
  return 0;
}

#elif !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)

/**
 * @brief Get system identification (stub for non-Windows).
 */
int uname(struct utsname *name) {
  if (!name) {
    errno = EFAULT;
    return -1;
  }
  errno = EINVAL;
  return -1;
}

#endif

#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
/**
 * @brief Retrieves information on posix-utsname availability.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT if out_available is NULL.
 */
auto_win_msvc_error_t posix_utsname_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#endif
typedef int make_iso_compilers_happy_tu_posix_utsname;
