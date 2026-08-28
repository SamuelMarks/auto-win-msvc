/* clang-format off */
#include "bsd-malloc-np.h"
#include <stdio.h>
#include <stddef.h>
/* clang-format on */

#if defined(_WIN32) || defined(_WIN64)

typedef unsigned long WIN_DWORD;
typedef void *WIN_HANDLE;
typedef const char *WIN_LPCSTR;
typedef struct HINSTANCE__ *WIN_HINSTANCE;
typedef WIN_HINSTANCE WIN_HMODULE;
typedef int(__stdcall *WIN_FARPROC)(void);

#pragma pack(push, 8)
typedef struct _PROCESS_MEMORY_COUNTERS {
  WIN_DWORD cb;
  WIN_DWORD PageFaultCount;
  size_t PeakWorkingSetSize;
  size_t WorkingSetSize;
  size_t QuotaPeakPagedPoolUsage;
  size_t QuotaPagedPoolUsage;
  size_t QuotaPeakNonPagedPoolUsage;
  size_t QuotaNonPagedPoolUsage;
  size_t PagefileUsage;
  size_t PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS;
#pragma pack(pop)

#if defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long bsd_malloc_u64;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 bsd_malloc_u64;
#else
typedef unsigned long long bsd_malloc_u64;
#endif

typedef struct _MEMORYSTATUSEX {
  WIN_DWORD dwLength;
  WIN_DWORD dwMemoryLoad;
  bsd_malloc_u64 ullTotalPhys;
  bsd_malloc_u64 ullAvailPhys;
  bsd_malloc_u64 ullTotalPageFile;
  bsd_malloc_u64 ullAvailPageFile;
  bsd_malloc_u64 ullTotalVirtual;
  bsd_malloc_u64 ullAvailVirtual;
  bsd_malloc_u64 ullAvailExtendedVirtual;
} MEMORYSTATUSEX;

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) WIN_HANDLE __stdcall GetCurrentProcess(void);
__declspec(dllimport) int __stdcall GlobalMemoryStatusEx(
    MEMORYSTATUSEX *lpBuffer);
__declspec(dllimport)
WIN_HMODULE __stdcall GetModuleHandleA(WIN_LPCSTR lpModuleName);
__declspec(dllimport)
WIN_HMODULE __stdcall LoadLibraryA(WIN_LPCSTR lpLibFileName);
__declspec(dllimport)
WIN_FARPROC __stdcall GetProcAddress(WIN_HMODULE hModule,
                                     WIN_LPCSTR lpProcName);

#ifdef __cplusplus
}
#endif

typedef int(__stdcall *GetProcessMemoryInfo_t)(WIN_HANDLE,
                                               PROCESS_MEMORY_COUNTERS *,
                                               WIN_DWORD);

/* Fallback macro for C89 64-bit int formatting */
#if defined(_WIN32) || defined(_WIN64)
#define U64_FMT "%I64u"
#else
#define U64_FMT "%llu"
#endif

/** \brief je_malloc_stats_print function. */
void je_malloc_stats_print(void (*write_cb)(void *, const char *),
                           void *cbopaque, const char *opts) {
  char buf[1024];
  PROCESS_MEMORY_COUNTERS pmc;
  MEMORYSTATUSEX memex;
  WIN_HMODULE hPsapi;
  WIN_HMODULE hKernel32;
  GetProcessMemoryInfo_t pGetProcessMemoryInfo = NULL;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#endif

  (void)opts;

  if (!write_cb)
    return;

  write_cb(cbopaque, "___ Begin Windows Native Memory Stats ___\n");

  /* Try Kernel32 first (Windows 7+ K32GetProcessMemoryInfo), then fallback to
   * psapi.dll */
  hKernel32 = GetModuleHandleA("kernel32.dll");
  if (hKernel32) {
    pGetProcessMemoryInfo = (GetProcessMemoryInfo_t)(size_t)GetProcAddress(
        hKernel32, "K32GetProcessMemoryInfo");
  }
  if (!pGetProcessMemoryInfo) {
    hPsapi = GetModuleHandleA("psapi.dll");
    if (!hPsapi) {
      hPsapi = LoadLibraryA("psapi.dll");
    }
    if (hPsapi) {
      pGetProcessMemoryInfo = (GetProcessMemoryInfo_t)(size_t)GetProcAddress(
          hPsapi, "GetProcessMemoryInfo");
    }
  }

  if (pGetProcessMemoryInfo) {
    pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
    if (pGetProcessMemoryInfo(GetCurrentProcess(), &pmc, pmc.cb)) {
#if defined(_MSC_VER)
      sprintf_s(buf, sizeof(buf),
#else
      sprintf(buf,
#endif
                "Process Memory:\n"
                "  WorkingSetSize: %lu\n"
                "  PeakWorkingSetSize: %lu\n"
                "  PagefileUsage: %lu\n"
                "  PeakPagefileUsage: %lu\n"
                "  PageFaultCount: %lu\n",
                (unsigned long)pmc.WorkingSetSize,
                (unsigned long)pmc.PeakWorkingSetSize,
                (unsigned long)pmc.PagefileUsage,
                (unsigned long)pmc.PeakPagefileUsage,
                (unsigned long)pmc.PageFaultCount);
      write_cb(cbopaque, buf);
    }
  }

  memex.dwLength = sizeof(memex);
  if (GlobalMemoryStatusEx(&memex)) {
#if defined(_MSC_VER)
    sprintf_s(buf, sizeof(buf),
#else
    sprintf(buf,
#endif
              "System Memory:\n"
              "  MemoryLoad: %lu%%\n"
              "  TotalPhys: " U64_FMT "\n"
              "  AvailPhys: " U64_FMT "\n"
              "  TotalPageFile: " U64_FMT "\n"
              "  AvailPageFile: " U64_FMT "\n"
              "  TotalVirtual: " U64_FMT "\n"
              "  AvailVirtual: " U64_FMT "\n",
              (unsigned long)memex.dwMemoryLoad,
              (bsd_malloc_u64)memex.ullTotalPhys,
              (bsd_malloc_u64)memex.ullAvailPhys,
              (bsd_malloc_u64)memex.ullTotalPageFile,
              (bsd_malloc_u64)memex.ullAvailPageFile,
              (bsd_malloc_u64)memex.ullTotalVirtual,
              (bsd_malloc_u64)memex.ullAvailVirtual);
    write_cb(cbopaque, buf);
  }

  write_cb(cbopaque, "___ End Windows Native Memory Stats ___\n");

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_malloc_np(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_malloc_np;
