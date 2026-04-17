#include <errno.h>
#ifndef ENOSYS
#define ENOSYS 38
#endif
/* clang-format off */
#include "linux-sys-prctl.h"
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) && !defined(__clang__)

typedef unsigned long DWORD;
typedef const char *LPCSTR;
typedef void *HANDLE;
typedef const wchar_t *PCWSTR;
typedef long HRESULT;
typedef struct HINSTANCE__ *HINSTANCE;
typedef HINSTANCE HMODULE;
typedef unsigned int UINT;
typedef int(__stdcall *FARPROC)(void);
typedef unsigned long ULONG_PTR;
typedef void *PVOID;
typedef unsigned long ULONG;
typedef long NTSTATUS;
typedef ULONG *PULONG;

#define CP_UTF8 65001
#define PROCESS_DUP_HANDLE 0x0040
#define SYNCHRONIZE 0x00100000L
#define DUPLICATE_SAME_ACCESS 0x00000002
#define JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE 0x00002000

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;
  LPCSTR szName;
  DWORD dwThreadID;
  DWORD dwFlags;
} THREADNAME_INFO;

typedef struct _PROCESS_BASIC_INFORMATION {
  PVOID Reserved1;
  PVOID PebBaseAddress;
  PVOID Reserved2[2];
  ULONG_PTR UniqueProcessId;
  ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    long HighPart;
  } u;
  long long QuadPart;
} LARGE_INTEGER;

typedef struct _JOBOBJECT_BASIC_LIMIT_INFORMATION {
  LARGE_INTEGER PerProcessUserTimeLimit;
  LARGE_INTEGER PerJobUserTimeLimit;
  DWORD LimitFlags;
  size_t MinimumWorkingSetSize;
  size_t MaximumWorkingSetSize;
  DWORD ActiveProcessLimit;
  ULONG_PTR Affinity;
  DWORD PriorityClass;
  DWORD SchedulingClass;
} JOBOBJECT_BASIC_LIMIT_INFORMATION;

typedef struct _IO_COUNTERS {
  unsigned long long ReadOperationCount;
  unsigned long long WriteOperationCount;
  unsigned long long OtherOperationCount;
  unsigned long long ReadTransferCount;
  unsigned long long WriteTransferCount;
  unsigned long long OtherTransferCount;
} IO_COUNTERS;

typedef struct _JOBOBJECT_EXTENDED_LIMIT_INFORMATION {
  JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInformation;
  IO_COUNTERS IoInfo;
  size_t ProcessMemoryLimit;
  size_t JobMemoryLimit;
  size_t PeakProcessMemoryUsed;
  size_t PeakJobMemoryUsed;
} JOBOBJECT_EXTENDED_LIMIT_INFORMATION;
#pragma pack(pop)

typedef enum _JOBOBJECTINFOCLASS {
  JobObjectExtendedLimitInformation = 9
} JOBOBJECTINFOCLASS;

typedef HRESULT(__stdcall *SetThreadDescription_t)(HANDLE, PCWSTR);
typedef NTSTATUS(__stdcall *NtQueryInformationProcess_t)(HANDLE, ULONG, PVOID,
                                                         ULONG, PULONG);
typedef HANDLE(__stdcall *OpenProcess_t)(DWORD, int, DWORD);
typedef HANDLE(__stdcall *CreateJobObjectA_t)(PVOID, LPCSTR);
typedef int(__stdcall *SetInformationJobObject_t)(HANDLE, JOBOBJECTINFOCLASS,
                                                  PVOID, DWORD);
typedef int(__stdcall *AssignProcessToJobObject_t)(HANDLE, HANDLE);
typedef int(__stdcall *DuplicateHandle_t)(HANDLE, HANDLE, HANDLE, HANDLE *,
                                          DWORD, int, DWORD);
typedef int(__stdcall *CloseHandle_t)(HANDLE);
typedef HANDLE(__stdcall *GetCurrentProcess_t)(void);

__declspec(dllimport) HMODULE __stdcall GetModuleHandleA(LPCSTR lpModuleName);
__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE hModule,
                                                       LPCSTR lpProcName);
__declspec(dllimport) HANDLE __stdcall GetCurrentThread(void);
__declspec(dllimport) int __stdcall MultiByteToWideChar(
    UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte,
    wchar_t *lpWideCharStr, int cchWideChar);
__declspec(dllimport) void __stdcall RaiseException(
    DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments,
    const ULONG_PTR *lpArguments);

/** \brief Internal state to track if we already assigned pdeathsig */
static int g_pdeathsig_assigned = 0;
static int g_pdeathsig_signum = 0;

/** \brief prctl function.
 *
 * Implements process control operations.
 * Currently supports PR_SET_NAME to set the calling thread's name,
 * and PR_SET_PDEATHSIG to terminate the child when the parent dies.
 *
 * \param option The operation to perform.
 * \param ... Variable arguments based on the option.
 * \return 0 on success, or -1 on error with errno set.
 */
int prctl(int option, ...) {
  va_list ap;

  if (option == PR_SET_NAME) {
    char *name;
    va_start(ap, option);
    name = va_arg(ap, char *);
    va_end(ap);

    if (name != NULL) {
      HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
      if (hKernel32) {
        SetThreadDescription_t pSetThreadDescription =
            (SetThreadDescription_t)(void *)GetProcAddress(
                hKernel32, "SetThreadDescription");
        if (pSetThreadDescription) {
          wchar_t wname[1024];
          MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, 1024);
          pSetThreadDescription(GetCurrentThread(), wname);
          return 0;
        }
      }

#if !defined(_WIN64) || defined(_MSC_VER)
      {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = (DWORD)-1;
        info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
        __try {
          RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR),
                         (const ULONG_PTR *)&info);
        } __except (1) {
        }
#pragma warning(pop)
      }
#endif
      return 0;
    }
    errno = EINVAL;
    return -1;
  }

  if (option == PR_SET_PDEATHSIG) {
    int sig;
    HMODULE hNtDll;
    HMODULE hKernel32;
    NtQueryInformationProcess_t pNtQueryInformationProcess;
    OpenProcess_t pOpenProcess;
    CreateJobObjectA_t pCreateJobObjectA;
    SetInformationJobObject_t pSetInformationJobObject;
    AssignProcessToJobObject_t pAssignProcessToJobObject;
    DuplicateHandle_t pDuplicateHandle;
    CloseHandle_t pCloseHandle;
    GetCurrentProcess_t pGetCurrentProcess;
    PROCESS_BASIC_INFORMATION pbi;
    ULONG retLen = 0;
    HANDLE hParent;
    HANDLE hJob;
    HANDLE hDup;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;

    va_start(ap, option);
    sig = va_arg(ap, int);
    va_end(ap);

    /* Update internal tracking */
    g_pdeathsig_signum = sig;
    if (sig == 0) {
      /* Linux semantics allow clearing it. Windows Job limit cannot be easily
         revoked once assigned, but we update our local tracking. */
      return 0;
    }

    if (g_pdeathsig_assigned) {
      /* Already assigned to a job for this purpose */
      return 0;
    }

    hNtDll = GetModuleHandleA("ntdll.dll");
    hKernel32 = GetModuleHandleA("kernel32.dll");

    if (!hNtDll || !hKernel32) {
      errno = ENOSYS;
      return -1;
    }

    pNtQueryInformationProcess =
        (NtQueryInformationProcess_t)(void *)GetProcAddress(
            hNtDll, "NtQueryInformationProcess");
    pOpenProcess =
        (OpenProcess_t)(void *)GetProcAddress(hKernel32, "OpenProcess");
    pCreateJobObjectA = (CreateJobObjectA_t)(void *)GetProcAddress(
        hKernel32, "CreateJobObjectA");
    pSetInformationJobObject =
        (SetInformationJobObject_t)(void *)GetProcAddress(
            hKernel32, "SetInformationJobObject");
    pAssignProcessToJobObject =
        (AssignProcessToJobObject_t)(void *)GetProcAddress(
            hKernel32, "AssignProcessToJobObject");
    pDuplicateHandle =
        (DuplicateHandle_t)(void *)GetProcAddress(hKernel32, "DuplicateHandle");
    pCloseHandle =
        (CloseHandle_t)(void *)GetProcAddress(hKernel32, "CloseHandle");
    pGetCurrentProcess = (GetCurrentProcess_t)(void *)GetProcAddress(
        hKernel32, "GetCurrentProcess");

    if (!pNtQueryInformationProcess || !pOpenProcess || !pCreateJobObjectA ||
        !pSetInformationJobObject || !pAssignProcessToJobObject ||
        !pDuplicateHandle || !pCloseHandle || !pGetCurrentProcess) {
      errno = ENOSYS;
      return -1;
    }

    if (pNtQueryInformationProcess(pGetCurrentProcess(),
                                   0 /* ProcessBasicInformation */, &pbi,
                                   sizeof(pbi), &retLen) != 0) {
      errno = EINVAL;
      return -1;
    }

    hParent = pOpenProcess(PROCESS_DUP_HANDLE | SYNCHRONIZE, 0,
                           (DWORD)pbi.InheritedFromUniqueProcessId);
    if (!hParent) {
      /* Parent is already dead or we don't have permission */
      errno = EINVAL;
      return -1;
    }

    hJob = pCreateJobObjectA(NULL, NULL);
    if (!hJob) {
      pCloseHandle(hParent);
      errno = ENOMEM;
      return -1;
    }

    memset(&jeli, 0, sizeof(jeli));
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    if (!pSetInformationJobObject(hJob, JobObjectExtendedLimitInformation,
                                  &jeli, sizeof(jeli))) {
      pCloseHandle(hJob);
      pCloseHandle(hParent);
      errno = EINVAL;
      return -1;
    }

    if (!pAssignProcessToJobObject(hJob, pGetCurrentProcess())) {
      /* Might fail if process is already in a job on older Windows versions */
      pCloseHandle(hJob);
      pCloseHandle(hParent);
      errno = EINVAL;
      return -1;
    }

    if (!pDuplicateHandle(pGetCurrentProcess(), hJob, hParent, &hDup, 0, 0,
                          DUPLICATE_SAME_ACCESS)) {
      pCloseHandle(hJob);
      pCloseHandle(hParent);
      errno = EINVAL;
      return -1;
    }

    pCloseHandle(hJob);
    pCloseHandle(hParent);

    g_pdeathsig_assigned = 1;
    return 0;
  }

  if (option == PR_GET_PDEATHSIG) {
    int *sig;
    va_start(ap, option);
    sig = va_arg(ap, int *);
    va_end(ap);

    if (!sig) {
      errno = EFAULT;
      return -1;
    }

    *sig = g_pdeathsig_signum;
    return 0;
  }

  errno = ENOSYS;
  return -1;
}

#endif

/* Dummy function to prevent empty translation unit */
int dummy_linux_sys_prctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_sys_prctl;