
/* posix_read_write.c - Strict C89 Implementation */

/* clang-format off */
#include "posix-core.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <winsock2.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#if defined(__APPLE__) && !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#include <time.h>
#endif
#endif
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)

#if defined(_MSC_VER) && _MSC_VER >= 1400
static void my_invalid_parameter_handler(const wchar_t *expression,
                                         const wchar_t *function,
                                         const wchar_t *file, unsigned int line,
                                         uintptr_t pReserved) {
  if (expression || function || file || line || pReserved) {
    /* parameter error handled */
  }
}
#endif

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h = -1;
  int saved_err = errno;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(my_invalid_parameter_handler);
#if defined(_DEBUG) && !defined(_DLL)
    _CrtSetReportMode(2, 0);
    _CrtSetReportMode(1, 0);
    _CrtSetReportMode(0, 0);
#endif
    h = (ptrdiff_t)_get_osfhandle((int)fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle((int)fd);
#endif
  errno = saved_err;
  if (h == -2) {
    return -1;
  }
  return h;
}
#endif

extern int is_socket(intptr_t);
extern intptr_t posix_get_dup_socket(intptr_t fd);
extern void posix_set_dup_socket(intptr_t fd, intptr_t s);
extern intptr_t posix_lookup_crt_socket(intptr_t fd);
extern void posix_register_crt_socket(intptr_t fd, intptr_t s);

static SOCKET resolve_socket_or_fd(intptr_t fd) {
  intptr_t s = posix_get_dup_socket(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  s = posix_lookup_crt_socket(fd);
  if (s != 0) {
    return (SOCKET)s;
  }
  return (SOCKET)fd;
}

#define GET_SOCKET(fd) resolve_socket_or_fd(fd)

#undef _read
#undef _write

/* Removed g_cloned_cwd */

#if defined(_MSC_VER) && defined(_M_X64) &&                                    \
    !defined(auto_win_msvc_export_lib_EXPORTS)
#include <excpt.h>
EXCEPTION_DISPOSITION __cdecl
__C_specific_handler_noexcept(struct _EXCEPTION_RECORD *ExceptionRecord,
                              void *EstablisherFrame,
                              struct _CONTEXT *ContextRecord,
                              struct _DISPATCHER_CONTEXT *DispatcherContext) {
  return __C_specific_handler(ExceptionRecord, EstablisherFrame, ContextRecord,
                              DispatcherContext);
}
void *__imp___C_specific_handler_noexcept =
    (void *)(size_t)__C_specific_handler_noexcept;
#pragma comment(linker, "/include:__C_specific_handler_noexcept")
#pragma comment(linker, "/include:__imp___C_specific_handler_noexcept")

DWORD __cdecl __vcrt_GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename,
                                        DWORD nSize) {
  return GetModuleFileNameW(hModule, lpFilename, nSize);
}
void *__imp___vcrt_GetModuleFileNameW =
    (void *)(size_t)__vcrt_GetModuleFileNameW;
#pragma comment(linker, "/include:__vcrt_GetModuleFileNameW")
#pragma comment(linker, "/include:__imp___vcrt_GetModuleFileNameW")

HMODULE __cdecl __vcrt_GetModuleHandleW(LPCWSTR lpModuleName) {
  return GetModuleHandleW(lpModuleName);
}
void *__imp___vcrt_GetModuleHandleW = (void *)(size_t)__vcrt_GetModuleHandleW;
#pragma comment(linker, "/include:__vcrt_GetModuleHandleW")
#pragma comment(linker, "/include:__imp___vcrt_GetModuleHandleW")

HMODULE __cdecl __vcrt_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile,
                                      DWORD dwFlags) {
  return LoadLibraryExW(lpLibFileName, hFile, dwFlags);
}
void *__imp___vcrt_LoadLibraryExW = (void *)(size_t)__vcrt_LoadLibraryExW;
#pragma comment(linker, "/include:__vcrt_LoadLibraryExW")
#pragma comment(linker, "/include:__imp___vcrt_LoadLibraryExW")
#endif

#if defined(_MSC_VER) && defined(_M_X64)
typedef int(__cdecl *rtc_error_fn_w)(int, const wchar_t *, int, const wchar_t *,
                                     const wchar_t *, ...);
rtc_error_fn_w __cdecl _RTC_SetErrorFuncW(rtc_error_fn_w);

static int __cdecl auto_win_rtc_error_handler(int errType,
                                              const wchar_t *filename, int line,
                                              const wchar_t *moduleName,
                                              const wchar_t *format, ...) {
  (void)errType;
  (void)filename;
  (void)line;
  (void)moduleName;
  (void)format;
  return 0;
}
#endif

static int g_binary_stdio_init = 0;

/**
 * @brief Ensures standard streams and default CRT file mode use binary LF mode.
 */
static void __cdecl ensure_binary_stdio(void) {
  if (!g_binary_stdio_init) {
    int saved_err = errno;
    WSADATA wsa_data;
    (void)WSAStartup(MAKEWORD(2, 2), &wsa_data);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    (void)_set_invalid_parameter_handler(my_invalid_parameter_handler);
#if defined(_DEBUG)
    if (GetProcAddress(GetModuleHandleA("ntdll.dll"), "wine_get_version") ==
        NULL) {
      _CrtSetReportMode(_CRT_ASSERT, 0);
      _CrtSetReportMode(_CRT_ERROR, 0);
      _CrtSetReportMode(_CRT_WARN, 0);
    }
#endif
#if defined(_M_X64)
    (void)_RTC_SetErrorFuncW(auto_win_rtc_error_handler);
#endif
#endif
    g_binary_stdio_init = 1;
#if defined(_MSC_VER)
    _set_fmode(_O_BINARY);
#else
    _fmode = _O_BINARY;
#endif
    if (safe_get_osfhandle(0) != -1) {
      _setmode(0, _O_BINARY);
    }
    if (safe_get_osfhandle(1) != -1) {
      _setmode(1, _O_BINARY);
    }
    if (safe_get_osfhandle(2) != -1) {
      _setmode(2, _O_BINARY);
    }
    errno = saved_err;
  }
}

#if defined(_MSC_VER)
#if defined(auto_win_msvc_export_lib_EXPORTS)
int __stdcall DllMain(void *hinstDLL, unsigned long fdwReason,
                      void *lpvReserved) {
  (void)hinstDLL;
  (void)lpvReserved;
  if (fdwReason == 1 /* DLL_PROCESS_ATTACH */) {
    ensure_binary_stdio();
  }
  return 1;
}
#else
#pragma push_macro("read")
#undef read
#pragma section(".CRT$XCU", read)
#pragma pop_macro("read")
__declspec(allocate(".CRT$XCU")) void(__cdecl *__init_binary_stdio_ptr)(void) =
    ensure_binary_stdio;
#if defined(_WIN64)
#pragma comment(linker, "/include:__init_binary_stdio_ptr")
#else
#pragma comment(linker, "/include:___init_binary_stdio_ptr")
#endif
#endif
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((constructor)) static void __init_binary_stdio_gcc(void) {
  ensure_binary_stdio();
}
#endif

__declspec(dllimport) unsigned long __stdcall
GetTempPathA(unsigned long nBufferLength, char *lpBuffer);
__declspec(dllimport) unsigned int __stdcall
GetTempFileNameA(const char *lpPathName, const char *lpPrefixString,
                 unsigned int uUnique, char *lpTempFileName);

/**
 * @brief Safely opens a temporary FILE* in the user's temp directory with
 * delete-on-close semantics.
 * @return Temporary FILE* or NULL on error.
 */
static FILE *safe_tmpfile(void) {
  char tmp_dir[MAX_PATH];
  char tmp_file[MAX_PATH];
  HANDLE h;
  int fd;
  FILE *f;
  DWORD len = GetTempPathA(sizeof(tmp_dir), tmp_dir);
  if (len == 0 || len >= sizeof(tmp_dir)) {
#if defined(_MSC_VER)
    strcpy_s(tmp_dir, sizeof(tmp_dir), ".");
#else
    strcpy(tmp_dir, ".");
#endif
  }
  if (!GetTempFileNameA(tmp_dir, "awm", 0, tmp_file)) {
    return NULL;
  }
  h = CreateFileA(tmp_file, GENERIC_READ | GENERIC_WRITE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                  CREATE_ALWAYS,
                  FILE_ATTRIBUTE_TEMPORARY |
                      0x04000000 /* FILE_FLAG_DELETE_ON_CLOSE */,
                  NULL);
  if (h == INVALID_HANDLE_VALUE) {
    return NULL;
  }
  fd = _open_osfhandle((intptr_t)h, _O_RDWR | _O_BINARY);
  if (fd == -1) {
    CloseHandle(h);
    return NULL;
  }
  f = _fdopen(fd, "w+b");
  if (!f) {
    _close(fd);
    return NULL;
  }
  return f;
}

/**
 * @brief Generates a synthetic temporary FILE* if pathname matches a virtual
 * /proc file.
 * @param[in] pathname Path to check.
 * @return Temporary FILE* positioned at offset 0, or NULL if not a virtual
 * /proc file.
 */
static FILE *open_virtual_proc_stream(const char *pathname) {
  const char *sub = pathname;
  char buf[512];
  FILE *tmp_f;

  if (!pathname) {
    return NULL;
  }
  if (strcmp(pathname, "/dev/null") == 0 ||
      strcmp(pathname, "\\dev\\null") == 0) {
    return safe_tmpfile();
  }
  if (strncmp(pathname, "/proc/", 6) == 0 ||
      strncmp(pathname, "\\proc\\", 6) == 0) {
    sub = pathname + 6;
  }

  if (strcmp(sub, "meminfo") == 0) {
    typedef struct _WIN_MEMSTATEX {
      DWORD dwLength;
      DWORD dwMemoryLoad;
      DWORDLONG ullTotalPhys;
      DWORDLONG ullAvailPhys;
      DWORDLONG ullTotalPageFile;
      DWORDLONG ullAvailPageFile;
      DWORDLONG ullTotalVirtual;
      DWORDLONG ullAvailVirtual;
      DWORDLONG ullAvailExtendedVirtual;
    } WIN_MEMSTATEX;
    typedef BOOL(WINAPI * PFN_GMS)(WIN_MEMSTATEX *);
    HMODULE hK32 = GetModuleHandleA("kernel32.dll");
    PFN_GMS pGMS =
        hK32 ? (PFN_GMS)(size_t)GetProcAddress(hK32, "GlobalMemoryStatusEx")
             : NULL;
    WIN_MEMSTATEX mem;
    DWORDLONG total_kb = (DWORDLONG)16777216U;
    DWORDLONG avail_kb = (DWORDLONG)8388608U;
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    memset(&mem, 0, sizeof(mem));
    mem.dwLength = sizeof(mem);
    if (pGMS && pGMS(&mem)) {
      total_kb = mem.ullTotalPhys / (DWORDLONG)1024U;
      avail_kb = mem.ullAvailPhys / (DWORDLONG)1024U;
    }
#if defined(_MSC_VER)
    sprintf_s(buf, sizeof(buf),
              "MemTotal: %I64u kB\nMemFree: %I64u kB\nMemAvailable: %I64u "
              "kB\nBuffers: 0 kB\nCached: 0 kB\nSwapCached: 0 kB\nSwapTotal: "
              "0 kB\nSwapFree: 0 kB\nShmem: 0 kB\nSReclaimable: 0 kB\n",
              total_kb, avail_kb, avail_kb);
#else
    sprintf(buf,
            "MemTotal: %lu kB\nMemFree: %lu kB\nMemAvailable: %lu "
            "kB\nBuffers: 0 kB\nCached: 0 kB\nSwapCached: 0 kB\nSwapTotal: 0 "
            "kB\nSwapFree: 0 kB\nShmem: 0 kB\nSReclaimable: 0 kB\n",
            (unsigned long)total_kb, (unsigned long)avail_kb,
            (unsigned long)avail_kb);
#endif
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "uptime") == 0) {
    unsigned long ticks = GetTickCount();
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
#if defined(_MSC_VER)
    sprintf_s(buf, sizeof(buf), "%lu.%02lu %lu.%02lu\n", ticks / 1000UL,
              (ticks % 1000UL) / 10UL, ticks / 1000UL, (ticks % 1000UL) / 10UL);
#else
    sprintf(buf, "%lu.%02lu %lu.%02lu\n", ticks / 1000UL,
            (ticks % 1000UL) / 10UL, ticks / 1000UL, (ticks % 1000UL) / 10UL);
#endif
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "loadavg") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("0.10 0.10 0.10 1/64 1000\n", tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "version") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("Linux version 6.1.0-windows-msvc (msvc@windows) #1 SMP PREEMPT\n",
          tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "stat") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("cpu  1000 0 500 8000 0 0 0 0 0 0\n"
          "cpu0 1000 0 500 8000 0 0 0 0 0 0\n"
          "intr 1000\n"
          "ctxt 2000\n"
          "btime 1700000000\n"
          "processes 128\n"
          "procs_running 1\n"
          "procs_blocked 0\n",
          tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "mounts") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("C:/ / ntfs rw,relatime 0 0\n", tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "filesystems") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("\tntfs\n\tvfat\nnodev\tproc\n", tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "swaps") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("Filename\t\t\t\tType\t\tSize\t\tUsed\t\tPriority\n", tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "cpuinfo") == 0) {
    tmp_f = safe_tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("processor\t: 0\nvendor_id\t: GenuineIntel\nmodel name\t: x86_64 "
          "CPU\n\n",
          tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if ((sub[0] >= '0' && sub[0] <= '9') || strncmp(sub, "self", 4) == 0) {
    unsigned long pid = (strncmp(sub, "self", 4) == 0)
                            ? (unsigned long)GetCurrentProcessId()
                            : strtoul(sub, NULL, 10);
    const char *slash = strchr(sub, '/');
    if (!slash) {
      slash = strchr(sub, '\\');
    }
    if (slash) {
      const char *leaf = slash + 1;
      if (strcmp(leaf, "stat") == 0) {
        tmp_f = safe_tmpfile();
        if (!tmp_f) {
          return NULL;
        }
#if defined(_MSC_VER)
        sprintf_s(buf, sizeof(buf),
                  "%lu (busybox) R 1 %lu %lu 0 -1 4194304 100 0 0 0 10 5 0 0 "
                  "20 0 1 0 100 4096000 1000 18446744073709551615 0 0 0 0 0 0 "
                  "0 0 0 0 0 0 17 0 0 0 0 0 0\n",
                  pid, pid, pid);
#else
        sprintf(buf,
                "%lu (busybox) R 1 %lu %lu 0 -1 4194304 100 0 0 0 10 5 0 0 20 "
                "0 1 0 100 4096000 1000 18446744073709551615 0 0 0 0 0 0 0 0 "
                "0 0 0 0 17 0 0 0 0 0 0\n",
                pid, pid, pid);
#endif
        fputs(buf, tmp_f);
        rewind(tmp_f);
        return tmp_f;
      }
      if (strcmp(leaf, "cmdline") == 0 || strcmp(leaf, "comm") == 0) {
        tmp_f = safe_tmpfile();
        if (!tmp_f) {
          return NULL;
        }
        fputs("busybox\n", tmp_f);
        rewind(tmp_f);
        return tmp_f;
      }
      if (strcmp(leaf, "status") == 0) {
        tmp_f = safe_tmpfile();
        if (!tmp_f) {
          return NULL;
        }
#if defined(_MSC_VER)
        sprintf_s(buf, sizeof(buf),
                  "Name:\tbusybox\nState:\tR (running)\nPid:\t%lu\nPPid:\t1\n"
                  "Uid:\t0\t0\t0\t0\nGid:\t0\t0\t0\t0\n",
                  pid);
#else
        sprintf(buf,
                "Name:\tbusybox\nState:\tR (running)\nPid:\t%lu\nPPid:\t1\n"
                "Uid:\t0\t0\t0\t0\nGid:\t0\t0\t0\t0\n",
                pid);
#endif
        fputs(buf, tmp_f);
        rewind(tmp_f);
        return tmp_f;
      }
    }
  }

  return NULL;
}

/** @brief Open stream with POSIX semantics. */
FILE *posix_fopen(const char *pathname, const char *mode) {
  int flags = 0;
  DWORD dwAccess = 0, dwCreation = OPEN_EXISTING;
  char abs_path[2048];
  HANDLE h;
  intptr_t fd;
  FILE *f;

  char clean_mode[8];
  int mi = 0;
  const char *mp;

  if (!pathname || !mode) {
    errno = EINVAL;
    return NULL;
  }

  for (mp = mode; *mp && mi < 6; mp++) {
    if (*mp == 'r' || *mp == 'w' || *mp == 'a' || *mp == '+') {
      clean_mode[mi++] = *mp;
    }
  }
  if (mi == 0) {
    clean_mode[mi++] = 'r';
  }
  clean_mode[mi++] = 'b';
  clean_mode[mi] = '\0';
  mode = clean_mode;

  f = open_virtual_proc_stream(pathname);
  if (f != NULL) {
    return f;
  }

  if (strchr(mode, 'r')) {
    flags |= _O_RDONLY;
    dwAccess |= GENERIC_READ;
  }
  if (strchr(mode, 'w')) {
    flags |= _O_WRONLY | _O_CREAT | _O_TRUNC;
    dwAccess |= GENERIC_WRITE;
    dwCreation = CREATE_ALWAYS;
  }
  if (strchr(mode, 'a')) {
    flags |= _O_WRONLY | _O_CREAT | _O_APPEND;
    dwAccess |= GENERIC_WRITE;
    dwCreation = OPEN_ALWAYS;
  }
  if (strchr(mode, '+')) {
    flags &= ~(_O_RDONLY | _O_WRONLY);
    flags |= _O_RDWR;
    dwAccess |= GENERIC_READ | GENERIC_WRITE;
  }
  flags |= _O_BINARY;

  if (pathname[0] != '/' && pathname[0] != '\\' && pathname[1] != ':') {
    char cwd[1024];
    if (GetCurrentDirectoryA(1024, cwd) == 0) {
      errno = ENOENT;
      return NULL;
    }
#if defined(_MSC_VER) && _MSC_VER < 1900
#if defined(__STDC_SECURE_LIB__) || _MSC_VER >= 1400
    if (_snprintf_s(abs_path, 2048, _TRUNCATE, "%s\\%s", cwd, pathname) < 0) {
      errno = ENAMETOOLONG;
      return NULL;
    }
#else
    if (_snprintf(abs_path, 2048, "%s\\%s", cwd, pathname) < 0) {
      errno = ENAMETOOLONG;
      return NULL;
    }
#endif
#else
    if (snprintf(abs_path, 2048, "%s\\%s", cwd, pathname) < 0) {
      errno = ENAMETOOLONG;
      return NULL;
    }
#endif
  } else {
#if defined(_MSC_VER) && _MSC_VER < 1900
#if defined(__STDC_SECURE_LIB__) || _MSC_VER >= 1400
    if (_snprintf_s(abs_path, 2048, _TRUNCATE, "%s", pathname) < 0) {
      errno = ENAMETOOLONG;
      return NULL;
    }
#else
    if (_snprintf(abs_path, 2048, "%s", pathname) < 0) {
      errno = ENAMETOOLONG;
      return NULL;
    }
#endif
#else
    if (snprintf(abs_path, 2048, "%s", pathname) < 0) {
      errno = ENAMETOOLONG;
      return NULL;
    }
#endif
  }

  h = CreateFileA(abs_path, dwAccess,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                  dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE) {
    errno = GetLastError();
    return NULL;
  }
  fd = _open_osfhandle((intptr_t)h, flags);
  if (fd == -1) {
    CloseHandle(h);
    return NULL;
  }
  f = _fdopen((int)fd, mode);
  if (f == NULL) {
    _close((int)fd);
    return NULL;
  }
  return f;
}

struct posix_socket_stream_entry {
  FILE *stream;
  SOCKET sock;
  int eof_flag;
  int err_flag;
};

static struct posix_socket_stream_entry g_socket_streams[64];

static void register_socket_stream(FILE *stream, SOCKET sock) {
  int i;
  if (!stream || sock == (SOCKET)-1 || sock == 0) {
    return;
  }
  for (i = 0; i < 64; i++) {
    if (g_socket_streams[i].stream == stream ||
        g_socket_streams[i].stream == NULL) {
      g_socket_streams[i].stream = stream;
      g_socket_streams[i].sock = sock;
      g_socket_streams[i].eof_flag = 0;
      g_socket_streams[i].err_flag = 0;
      return;
    }
  }
}

static struct posix_socket_stream_entry *
find_socket_stream_entry(FILE *stream) {
  int i;
  if (!stream) {
    return NULL;
  }
  for (i = 0; i < 64; i++) {
    if (g_socket_streams[i].stream == stream) {
      return &g_socket_streams[i];
    }
  }
  return NULL;
}

static void unregister_socket_stream(FILE *stream) {
  int i;
  if (!stream) {
    return;
  }
  for (i = 0; i < 64; i++) {
    if (g_socket_streams[i].stream == stream) {
      g_socket_streams[i].stream = NULL;
      g_socket_streams[i].sock = 0;
      g_socket_streams[i].eof_flag = 0;
      g_socket_streams[i].err_flag = 0;
      return;
    }
  }
}

/** @brief Checks if a CRT file descriptor refers to a Winsock socket without
 * altering errno. */
static int is_crt_fd_socket(int fd, SOCKET *out_sock) {
  int saved_err = errno;
  SOCKET sock = GET_SOCKET(fd);
  int optval = 0;
  int optlen = sizeof(optval);
  if (sock != (SOCKET)-1 && sock != 0 &&
      getsockopt(sock, SOL_SOCKET, SO_TYPE, (char *)&optval, &optlen) == 0) {
    errno = saved_err;
    if (out_sock) {
      *out_sock = sock;
    }
    return 1;
  }
  errno = saved_err;
  return 0;
}

static int is_stream_socket(FILE *stream, SOCKET *out_sock) {
  struct posix_socket_stream_entry *ent = find_socket_stream_entry(stream);
  int fd;
  if (ent != NULL) {
    if (out_sock) {
      *out_sock = ent->sock;
    }
    return 1;
  }
  {
    int saved_err = errno;
    fd = _fileno(stream);
    errno = saved_err;
  }
  if (fd >= 0 && is_crt_fd_socket(fd, out_sock)) {
    return 1;
  }
  return 0;
}

#undef fdopen
/** @brief Associate stream with file descriptor or socket. */
FILE *posix_fdopen(intptr_t fd, const char *mode) {
  char clean_mode[8];
  int mi = 0;
  const char *mp;
  int crt_fd = (int)fd;
  SOCKET sock = 0;
  int is_sock = 0;
  FILE *f;

  if (fd < 0 || !mode) {
    errno = EINVAL;
    return NULL;
  }

  for (mp = mode; *mp && mi < 6; mp++) {
    if (*mp == 'r' || *mp == 'w' || *mp == 'a' || *mp == '+') {
      clean_mode[mi++] = *mp;
    }
  }
  if (mi == 0) {
    clean_mode[mi++] = 'r';
  }
  clean_mode[mi++] = 'b';
  clean_mode[mi] = '\0';

  if (is_crt_fd_socket((int)fd, &sock)) {
    is_sock = 1;
  }

  if (is_sock) {
    f = safe_tmpfile();
    if (f != NULL) {
      register_socket_stream(f, sock);
      return f;
    }
    errno = EBADF;
    return NULL;
  }

  if (safe_get_osfhandle(fd) == -1) {
    int flags = _O_BINARY;
    if (strchr(clean_mode, '+')) {
      flags |= _O_RDWR;
    } else if (strchr(clean_mode, 'w') || strchr(clean_mode, 'a')) {
      flags |= _O_WRONLY;
    } else {
      flags |= _O_RDONLY;
    }
    crt_fd = _open_osfhandle(fd, flags);
    if (crt_fd == -1) {
      errno = EBADF;
      return NULL;
    }
  }
  return _fdopen(crt_fd, clean_mode);
}

#undef fgets
/** @brief Read a line from stream or socket-backed stream. */
char *posix_fgets(char *s, int size, FILE *stream) {
  SOCKET sock;
  if (!s || size <= 0 || !stream) {
    return NULL;
  }
  if (is_stream_socket(stream, &sock)) {
    struct posix_socket_stream_entry *ent = find_socket_stream_entry(stream);
    int pos = 0;
    while (pos < size - 1) {
      char ch;
      int r = recv(sock, &ch, 1, 0);
      if (r <= 0) {
        if (r == 0 && ent != NULL) {
          ent->eof_flag = 1;
        } else if (r == SOCKET_ERROR) {
          int werr = WSAGetLastError();
          if (werr == WSAEWOULDBLOCK || werr == WSAEINPROGRESS) {
            errno = EAGAIN;
          }
          if (ent != NULL) {
            ent->err_flag = 1;
          }
        }
        if (pos == 0) {
          return NULL;
        }
        break;
      }
      s[pos++] = ch;
      if (ch == '\n') {
        break;
      }
    }
    s[pos] = '\0';
    return s;
  }
  return fgets(s, size, stream);
}

#undef fprintf
#undef fread
#undef fwrite
#undef fgetc
#undef getc
#undef fclose
#undef feof
#undef ferror
#undef fileno

/** @brief Test end-of-file indicator on stream or socket-backed stream. */
int posix_feof(FILE *stream) {
  struct posix_socket_stream_entry *ent;
  if (!stream) {
    return 0;
  }
  ent = find_socket_stream_entry(stream);
  if (ent != NULL) {
    return ent->eof_flag;
  }
  return feof(stream);
}

/** @brief Test error indicator on stream or socket-backed stream. */
int posix_ferror(FILE *stream) {
  struct posix_socket_stream_entry *ent;
  if (!stream) {
    return 0;
  }
  ent = find_socket_stream_entry(stream);
  if (ent != NULL) {
    return ent->err_flag;
  }
  return ferror(stream);
}

/** @brief Return integer file descriptor or socket for stream. */
int posix_fileno(FILE *stream) {
  struct posix_socket_stream_entry *ent;
  if (!stream) {
    errno = EINVAL;
    return -1;
  }
  ent = find_socket_stream_entry(stream);
  if (ent != NULL) {
    return (int)ent->sock;
  }
  return _fileno(stream);
}

/** @brief Close stream or socket-backed stream. */
int posix_fclose(FILE *stream) {
  struct posix_socket_stream_entry *ent;
  if (!stream) {
    errno = EINVAL;
    return EOF;
  }
  ent = find_socket_stream_entry(stream);
  if (ent != NULL) {
    SOCKET s = ent->sock;
    unregister_socket_stream(stream);
    shutdown(s, SD_BOTH);
    closesocket(s);
  }
  return fclose(stream);
}

/** @brief Read a character from stream or socket-backed stream. */
int posix_fgetc(FILE *stream) {
  SOCKET sock;
  if (!stream) {
    return EOF;
  }
  if (is_stream_socket(stream, &sock)) {
    struct posix_socket_stream_entry *ent = find_socket_stream_entry(stream);
    unsigned char ch;
    int r = recv(sock, (char *)&ch, 1, 0);
    if (r <= 0) {
      if (r == 0 && ent != NULL) {
        ent->eof_flag = 1;
      } else if (r == SOCKET_ERROR) {
        int werr = WSAGetLastError();
        if (werr == WSAEWOULDBLOCK || werr == WSAEINPROGRESS) {
          errno = EAGAIN;
        }
        if (ent != NULL) {
          ent->err_flag = 1;
        }
      }
      return EOF;
    }
    return (int)ch;
  }
  return fgetc(stream);
}

/** @brief Write formatted output to stream or socket-backed stream. */
int posix_fprintf(FILE *stream, const char *format, ...) {
  va_list ap;
  SOCKET sock;
  int ret;
  if (!stream || !format) {
    return -1;
  }
  if (is_stream_socket(stream, &sock)) {
    char *buf = NULL;
    int len;
    va_start(ap, format);
#if defined(_MSC_VER)
    len = _vscprintf(format, ap);
#else
    len = vsnprintf(NULL, 0, format, ap);
#endif
    va_end(ap);
    if (len < 0) {
      return -1;
    }
    buf = (char *)malloc((size_t)len + 1);
    if (!buf) {
      return -1;
    }
    va_start(ap, format);
#if defined(_MSC_VER)
    ret = vsprintf_s(buf, (size_t)len + 1, format, ap);
#else
    ret = vsnprintf(buf, (size_t)len + 1, format, ap);
#endif
    va_end(ap);
    if (ret > 0) {
      ret = send(sock, buf, ret, 0);
    }
    free(buf);
    return ret;
  }
  va_start(ap, format);
  ret = vfprintf(stream, format, ap);
  va_end(ap);
  return ret;
}

/** @brief Read binary data from stream or socket-backed stream. */
size_t posix_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  SOCKET sock;
  if (!ptr || size == 0 || nmemb == 0 || !stream) {
    return 0;
  }
  if (is_stream_socket(stream, &sock)) {
    struct posix_socket_stream_entry *ent = find_socket_stream_entry(stream);
    size_t total = size * nmemb;
    size_t got = 0;
    while (got < total) {
      int r = recv(sock, (char *)ptr + got, (int)(total - got), 0);
      if (r <= 0) {
        if (r == 0 && ent != NULL) {
          ent->eof_flag = 1;
        } else if (r == SOCKET_ERROR) {
          int werr = WSAGetLastError();
          if (werr == WSAEWOULDBLOCK || werr == WSAEINPROGRESS ||
              werr == WSAETIMEDOUT) {
            if (got == 0) {
              errno = EAGAIN;
            }
          } else if (ent != NULL) {
            ent->err_flag = 1;
          }
        }
        break;
      }
      got += (size_t)r;
      if (size == 1 && got > 0) {
        break;
      }
    }
    return got / size;
  }
  return fread(ptr, size, nmemb, stream);
}

/** @brief Write binary data to stream or socket-backed stream. */
size_t posix_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  SOCKET sock;
  if (!ptr || size == 0 || nmemb == 0 || !stream) {
    return 0;
  }
  if (is_stream_socket(stream, &sock)) {
    size_t total = size * nmemb;
    int r = send(sock, (const char *)ptr, (int)total, 0);
    if (r <= 0) {
      return 0;
    }
    return (size_t)r / size;
  }
  return fwrite(ptr, size, nmemb, stream);
}

/** @brief Open file descriptor with POSIX semantics. */
int posix_open(const char *pathname, int flags, ...) {
  int mode = 0;
  if (pathname) {
    while (pathname[0] == '/' && pathname[1] == '/') {
      pathname++;
    }
  }
  DWORD dwDesiredAccess = 0;
  DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
  DWORD dwCreationDisposition = OPEN_EXISTING;
  DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
  HANDLE hFile;
  int fd_flags;
  intptr_t fd;
  FILE *vproc;

  if (!pathname) {
    errno = EINVAL;
    return -1;
  }

  vproc = open_virtual_proc_stream(pathname);
  if (vproc != NULL) {
    int vfd = _dup(_fileno(vproc));
    fclose(vproc);
    return vfd;
  }

  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
  }
  (void)mode;

  if ((flags & 3) == O_RDONLY)
    dwDesiredAccess = GENERIC_READ;
  else if ((flags & 3) == O_WRONLY)
    dwDesiredAccess = GENERIC_WRITE;
  else if ((flags & 3) == O_RDWR)
    dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;

  if ((flags & O_CREAT) && (flags & O_EXCL)) {
    dwCreationDisposition = CREATE_NEW;
  } else if ((flags & O_CREAT) && (flags & O_TRUNC)) {
    dwCreationDisposition = CREATE_ALWAYS;
  } else if (flags & O_TRUNC) {
    dwCreationDisposition = TRUNCATE_EXISTING;
  } else if (flags & O_CREAT) {
    dwCreationDisposition = OPEN_ALWAYS;
  }

  hFile = CreateFileA(pathname, dwDesiredAccess, dwShareMode, NULL,
                      dwCreationDisposition, dwFlagsAndAttributes, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD err = GetLastError();
    if (err == ERROR_FILE_EXISTS || err == ERROR_ALREADY_EXISTS) {
      errno = EEXIST;
    } else if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
      errno = ENOENT;
    } else if (err == ERROR_ACCESS_DENIED) {
      DWORD attr = GetFileAttributesA(pathname);
      if (attr != INVALID_FILE_ATTRIBUTES &&
          (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        errno = EISDIR;
      } else {
        errno = EACCES;
      }
    } else {
      errno = EINVAL;
    }
    return -1;
  }

  fd_flags = _O_BINARY;
  if (flags & O_APPEND)
    fd_flags |= _O_APPEND;
  if ((flags & 3) == O_RDONLY)
    fd_flags |= _O_RDONLY;
  else if ((flags & 3) == O_WRONLY)
    fd_flags |= _O_WRONLY;
  else if ((flags & 3) == O_RDWR)
    fd_flags |= _O_RDWR;

  fd = _open_osfhandle((intptr_t)hFile, fd_flags);
  if (fd == -1) {
    CloseHandle(hFile);
    errno = EMFILE;
    return -1;
  }
  return (int)fd;
}

/** @brief Create file descriptor with POSIX semantics. */
int posix_creat(const char *pathname, mode_t mode) {
  return posix_open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

extern int is_socket(intptr_t fd);
ssize_t posix_read(intptr_t fd, void *buf, size_t count) {
  int ret;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (!buf) {
    errno = EINVAL;
    return -1;
  }
  if (!is_socket(fd)) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(my_invalid_parameter_handler);
    if (old) { /* unused */
    }
#endif
    intptr_t osfh = safe_get_osfhandle(fd);
    if (osfh != -1 && osfh != -2) {
      int rret = _read((int)fd, buf, (unsigned int)count);
#if defined(_MSC_VER) && _MSC_VER >= 1400
      if (_set_invalid_parameter_handler(old)) { /* Ignore */
      }
#endif
      if (rret == -1 && errno == EINVAL)
        errno = EBADF;
      return rret;
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (_set_invalid_parameter_handler(old)) { /* Ignore */
    }
#endif
  }
  ret = recv(GET_SOCKET(fd), buf, (int)count, 0);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
      }
      ret = recv(GET_SOCKET(fd), buf, (int)count, 0);
      if (ret != SOCKET_ERROR)
        return ret;
      err = WSAGetLastError();
    }
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      DWORD bytes_read = 0;
      if (ReadFile((HANDLE)(intptr_t)fd, buf, (DWORD)count, &bytes_read,
                   NULL)) {
        return (int)bytes_read;
      }
      errno = EBADF;
      return -1;
    }
    if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT)
      errno = EWOULDBLOCK;
    else if (err == WSAECONNRESET)
      errno = ECONNRESET;
    else if (err == WSAEINPROGRESS)
      errno = EINPROGRESS;
    else if (err == WSAEALREADY)
      errno = EALREADY;
    else if (err == WSAENOTSOCK)
      errno = ENOTSOCK;
    else if (err == WSAEMSGSIZE)
      errno = EMSGSIZE;
    else if (err == WSAEADDRINUSE)
      errno = EADDRINUSE;
    else if (err == WSAEADDRNOTAVAIL)
      errno = EADDRNOTAVAIL;
    else if (err == WSAECONNABORTED)
      errno = ECONNABORTED;
    else if (err == WSAECONNREFUSED)
      errno = ECONNREFUSED;
    else
      errno = err;
    return -1;
  }
  return ret;
}

extern int is_socket(intptr_t fd);
ssize_t posix_write(intptr_t fd, const void *buf, size_t count) {
  int ret;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (!buf) {
    errno = EINVAL;
    return -1;
  }
  if (!is_socket(fd)) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(my_invalid_parameter_handler);
    if (old) { /* unused */
    }
#endif
    intptr_t osfh = safe_get_osfhandle(fd);
    if (osfh != -1 && osfh != -2) {
      int wret = _write((int)fd, buf, (unsigned int)count);
#if defined(_MSC_VER) && _MSC_VER >= 1400
      if (_set_invalid_parameter_handler(old)) { /* Ignore */
      }
#endif
      if (wret == -1 && errno == EINVAL)
        errno = EBADF;
      return wret;
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (_set_invalid_parameter_handler(old)) { /* Ignore */
    }
#endif
  }
  ret = send(GET_SOCKET(fd), buf, (int)count, 0);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSANOTINITIALISED) {
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
      }
      ret = send(GET_SOCKET(fd), buf, (int)count, 0);
      if (ret != SOCKET_ERROR)
        return ret;
      err = WSAGetLastError();
    }
    if (err != WSAEWOULDBLOCK) {
      /* printf("DEBUG posix_write send failed: fd=%d, err=%d\n", fd, err); */
    }
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
      DWORD written = 0;
      OVERLAPPED ov = {0};
      if (WriteFile((HANDLE)(intptr_t)fd, buf, (DWORD)count, &written, &ov)) {
        /* If WriteFile completes synchronously, we get the result. */
        return (int)written;
      } else {
        DWORD werr = GetLastError();
        if (werr == ERROR_IO_PENDING) {
          if (GetOverlappedResult((HANDLE)(intptr_t)fd, &ov, &written, TRUE)) {
            return (int)written;
          }
          werr = GetLastError();
        }
        /* printf("DEBUG posix_write WriteFile failed: fd=%d, err=%lu\n", fd,
         * werr); */
      }
      errno = EBADF;
      return -1;
    }
    if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT)
      errno = EWOULDBLOCK;
    else if (err == WSAECONNRESET)
      errno = ECONNRESET;
    else if (err == WSAEINPROGRESS)
      errno = EINPROGRESS;
    else if (err == WSAEALREADY)
      errno = EALREADY;
    else if (err == WSAENOTSOCK)
      errno = ENOTSOCK;
    else if (err == WSAEMSGSIZE)
      errno = EMSGSIZE;
    else if (err == WSAEADDRINUSE)
      errno = EADDRINUSE;
    else if (err == WSAEADDRNOTAVAIL)
      errno = EADDRNOTAVAIL;
    else if (err == WSAECONNABORTED)
      errno = ECONNABORTED;
    else if (err == WSAECONNREFUSED)
      errno = ECONNREFUSED;
    else
      errno = err;
    return -1;
  }
  return ret;
}

#undef close

#if defined(_MSC_VER)
#pragma comment(linker,                                                        \
                "/alternatename:posix_epoll_close=posix_epoll_close_stub")
int posix_epoll_close_stub(intptr_t fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return -1;
}
#elif defined(__GNUC__) || defined(__clang__)
int __attribute__((weak)) posix_epoll_close(intptr_t fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return -1;
}
#endif

extern error_type_t clear_nonblock(SOCKET s);
extern error_type_t clear_as_socket(intptr_t fd);
extern int posix_epoll_close(intptr_t);
int posix_close(intptr_t fd) {
  SOCKET s;
  int ret;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (fd >= 100 && fd < 1024 && posix_get_dup_socket(fd) == 0 &&
      posix_lookup_crt_socket(fd) == 0) {
    if (posix_epoll_close((int)fd) == 0) {
      return 0;
    }
  }
  s = GET_SOCKET(fd);
  if (clear_nonblock(s) != ERR_NONE) { /* Ignore */
  }
  if (clear_as_socket(fd) != ERR_NONE) { /* Ignore */
  }
  if (fd >= 0 && fd < 65536 &&
      (posix_get_dup_socket(fd) != 0 || posix_lookup_crt_socket(fd) != 0)) {
    intptr_t sock_h = posix_get_dup_socket(fd);
    int other_fd;
    int shared = 0;
    if (sock_h == 0) {
      sock_h = posix_lookup_crt_socket(fd);
    }
    if (fd < 64) {
      posix_set_dup_socket(fd, 0);
    }
    posix_register_crt_socket(fd, 0);
    for (other_fd = 0; other_fd < 64; other_fd++) {
      if (posix_get_dup_socket(other_fd) == sock_h) {
        shared = 1;
        break;
      }
    }
    if (!shared && sock_h >= 0 && sock_h < 65536 &&
        posix_lookup_crt_socket(sock_h) == sock_h) {
      shared = 1;
    }
    if (!shared) {
      shutdown((SOCKET)sock_h, SD_BOTH);
      closesocket((SOCKET)sock_h);
    }
    return 0;
  }
  ret = closesocket(s);
  if (ret == SOCKET_ERROR) {
    int err = WSAGetLastError();
    if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP ||
        err == WSANOTINITIALISED) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
      _invalid_parameter_handler old =
          _set_invalid_parameter_handler(my_invalid_parameter_handler);
      if (old) { /* unused */
      }
#endif
      int cret = -1;
      if (fd >= 0)
        cret = _close((int)fd);
#if defined(_MSC_VER) && _MSC_VER >= 1400
      if (_set_invalid_parameter_handler(old)) { /* Ignore */
      }
#endif
      return cret;
    }
    errno = err;
    return -1;
  }
  return ret;
}

extern int is_socket(intptr_t);
extern error_type_t mark_as_socket(intptr_t);
extern error_type_t clear_as_socket(intptr_t);

/** @brief Reposition read/write file offset with POSIX ESPIPE semantics. */
#if defined(_MSC_VER)
__int64 posix_lseek(int fd, __int64 offset, int whence) {
  __int64 res;
#else
off_t posix_lseek(int fd, off_t offset, int whence) {
  off_t res;
#endif
  ptrdiff_t h;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  h = safe_get_osfhandle(fd);
  if (h != -1) {
    DWORD ftype = GetFileType((HANDLE)h);
    if (ftype == 3 /* FILE_TYPE_PIPE */ || ftype == 2 /* FILE_TYPE_CHAR */) {
      errno = ESPIPE;
      return -1;
    }
  }
#if defined(_MSC_VER)
  res = _lseeki64(fd, offset, whence);
#else
  res = (off_t)_lseek(fd, (long)offset, whence);
#endif
  if (res == -1 && errno == EINVAL &&
      (whence == SEEK_SET || whence == SEEK_CUR || whence == SEEK_END)) {
    if (h != -1 && GetFileType((HANDLE)h) != 1 /* FILE_TYPE_DISK */) {
      errno = ESPIPE;
    }
  }
  return res;
}

/** @brief Duplicate file descriptor with POSIX semantics. */
int posix_dup(int oldfd) {
  int ret;
  if (oldfd < 0) {
    errno = EBADF;
    return -1;
  }
  ret = _dup(oldfd);
  if (ret != -1 && is_socket(oldfd)) {
    if (mark_as_socket(ret) != ERR_NONE) {
    }
  }
  return ret;
}

/** @brief Duplicate file descriptor with POSIX semantics. */
int posix_dup2(int oldfd, int newfd) {
  int ret;
  SOCKET sock = 0;
  if (oldfd < 0 || newfd < 0) {
    errno = EBADF;
    return -1;
  }
  if (oldfd == newfd) {
    return newfd;
  }
  if (newfd < 64 && (is_socket(oldfd) || is_crt_fd_socket(oldfd, &sock))) {
    if (sock == 0) {
      sock = GET_SOCKET(oldfd);
    }
    posix_set_dup_socket(newfd, (intptr_t)sock);
    if (mark_as_socket(newfd) != ERR_NONE) { /* Ignore */
    }
    return newfd;
  }
  if (newfd < 64) {
    posix_set_dup_socket(newfd, 0);
  }
  ret = _dup2(oldfd, newfd);
  if (ret != -1) {
    if (is_socket(oldfd)) {
      if (mark_as_socket(newfd) != ERR_NONE) { /* Ignore */
      }
    } else {
      if (clear_as_socket(newfd) != ERR_NONE) { /* Ignore */
      }
      if (newfd >= 0 && newfd <= 2) {
        ptrdiff_t h = safe_get_osfhandle(newfd);
        if (h != -1 && h != -2) {
          DWORD std_id = (newfd == 0)
                             ? (DWORD)-10
                             : ((newfd == 1) ? (DWORD)-11 : (DWORD)-12);
          SetStdHandle(std_id, (HANDLE)h);
        }
      }
    }
    return newfd;
  }
  return -1;
}

#else

#undef fopen
#undef open

#if defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
static FILE *open_virtual_proc_stream_posix(const char *pathname) {
  const char *sub;
  char buf[512];
  FILE *tmp_f;

  if (pathname == NULL || strncmp(pathname, "/proc/", 6) != 0) {
    return NULL;
  }
  sub = pathname + 6;

  if (strcmp(sub, "meminfo") == 0) {
    unsigned long total_kb = 16777216UL;
    unsigned long avail_kb = 8388608UL;
    tmp_f = tmpfile();
    sprintf(buf,
            "MemTotal: %lu kB\nMemFree: %lu kB\nMemAvailable: %lu kB\n"
            "Buffers: 0 kB\nCached: 0 kB\nSwapCached: 0 kB\nSwapTotal: 0 kB\n"
            "SwapFree: 0 kB\nShmem: 0 kB\nSReclaimable: 0 kB\n",
            total_kb, avail_kb, avail_kb);
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "uptime") == 0) {
    unsigned long up = 1000UL;
    tmp_f = tmpfile();
    sprintf(buf, "%lu.00 %lu.00\n", up, up);
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  {
    unsigned long pid = strtoul(sub, NULL, 10);
    const char *slash = strchr(sub, '/');
    const char *leaf = slash + 1;
    char comm[64];
    unsigned long ppid = 1UL;
    strcpy(comm, "busybox");
    if (strcmp(leaf, "stat") == 0) {
      tmp_f = tmpfile();
      sprintf(buf,
              "%lu (%s) R %lu %lu %lu 0 -1 4194304 100 0 0 0 10 5 0 0 "
              "20 0 1 0 100 4096000 1000 18446744073709551615 0 0 0 0 0 0 "
              "0 0 0 0 0 0 17 0 0 0 0 0 0\n",
              pid, comm, ppid, pid, pid);
      fputs(buf, tmp_f);
      rewind(tmp_f);
      return tmp_f;
    }
    tmp_f = tmpfile();
    sprintf(buf, "%s\n", comm);
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }
}

FILE *posix_fopen(const char *pathname, const char *mode) {
  FILE *vproc = open_virtual_proc_stream_posix(pathname);
  if (vproc != NULL) {
    return vproc;
  }
  return fopen(pathname, mode);
}

int posix_open(const char *pathname, int flags, ...) {
  va_list ap;
  int mode = 0;
  FILE *vproc = open_virtual_proc_stream_posix(pathname);
  if (vproc != NULL) {
    int vfd = dup(fileno(vproc));
    fclose(vproc);
    return vfd;
  }
  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    return open(pathname, flags, mode);
  }
  return open(pathname, flags);
}
#else

static int g_mock_fopen_count = 0;
static FILE *mock_fopen(const char *pathname, const char *mode) {
  if (strcmp(pathname, "/proc/meminfo") == 0) {
    if (g_mock_fopen_count++ == 0) {
      return NULL; /* first time fail */
    }
    return tmpfile(); /* second time succeed */
  }
  return fopen(pathname, mode);
}
#define fopen mock_fopen

/**
 * @brief Generates a synthetic temporary FILE* if pathname matches a virtual
 * /proc file on non-Linux POSIX systems.
 * @param[in] pathname Path to check.
 * @return Temporary FILE* positioned at offset 0, or NULL if not a virtual
 * /proc file.
 */
static FILE *open_virtual_proc_stream_posix(const char *pathname) {
#if defined(__linux__) || defined(__CYGWIN__)
  (void)pathname;
  return NULL;
#else
  const char *sub = pathname;
  char buf[512];
  FILE *tmp_f;

  if (!pathname) {
    return NULL;
  }
  if (strncmp(pathname, "/proc/", 6) == 0) {
    sub = pathname + 6;
  } else if (strcmp(pathname, "version") != 0 &&
             strcmp(pathname, "uptime") != 0 &&
             strcmp(pathname, "meminfo") != 0 &&
             strcmp(pathname, "loadavg") != 0 &&
             strcmp(pathname, "stat") != 0 && strcmp(pathname, "mounts") != 0 &&
             strcmp(pathname, "filesystems") != 0 &&
             strcmp(pathname, "swaps") != 0 &&
             strcmp(pathname, "cpuinfo") != 0) {
    return NULL;
  }

  if (strcmp(sub, "meminfo") == 0) {
    unsigned long total_kb = 16777216UL;
    unsigned long avail_kb = 8388608UL;
#if defined(__APPLE__) && !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
    {
      int mib[2];
      unsigned long physmem = 0;
      size_t len = sizeof(physmem);
      mib[0] = 6;  /* CTL_HW */
      mib[1] = 24; /* HW_MEMSIZE */
      if (sysctl(mib, 2, &physmem, &len, NULL, 0) == 0 && physmem > 0) {
        total_kb = physmem / 1024UL;
        avail_kb = total_kb / 2UL;
      }
    }
#endif
    tmp_f = tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    sprintf(buf,
            "MemTotal: %lu kB\nMemFree: %lu kB\nMemAvailable: %lu kB\n"
            "Buffers: 0 kB\nCached: 0 kB\nSwapCached: 0 kB\nSwapTotal: 0 kB\n"
            "SwapFree: 0 kB\nShmem: 0 kB\nSReclaimable: 0 kB\n",
            total_kb, avail_kb, avail_kb);
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "uptime") == 0) {
    unsigned long up = 1000UL;
#if defined(__APPLE__) && !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
    {
      struct timespec ts;
      if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0 && ts.tv_sec > 0) {
        up = (unsigned long)ts.tv_sec;
      }
    }
#endif
    tmp_f = tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    sprintf(buf, "%lu.00 %lu.00\n", up, up);
    fputs(buf, tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if (strcmp(sub, "version") == 0) {
    tmp_f = tmpfile();
    if (!tmp_f) {
      return NULL;
    }
    fputs("Linux version 6.1.0-posix-compat #1 SMP PREEMPT\n", tmp_f);
    rewind(tmp_f);
    return tmp_f;
  }

  if ((sub[0] >= '0' && sub[0] <= '9') || strncmp(sub, "self", 4) == 0) {
    unsigned long pid = (strncmp(sub, "self", 4) == 0) ? (unsigned long)getpid()
                                                       : strtoul(sub, NULL, 10);
    const char *slash = strchr(sub, '/');
    if (slash) {
      const char *leaf = slash + 1;
      char comm[64];
      unsigned long ppid = 1UL;
      strcpy(comm, "busybox");
#if defined(__APPLE__) && !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
      {
        int mib[4];
        struct kinfo_proc kp;
        size_t len = sizeof(kp);
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_PID;
        mib[3] = (int)pid;
        if (sysctl(mib, 4, &kp, &len, NULL, 0) == 0 && len > 0 &&
            kp.kp_proc.p_comm[0] != '\0') {
          strncpy(comm, kp.kp_proc.p_comm, sizeof(comm) - 1);
          comm[sizeof(comm) - 1] = '\0';
          ppid = (unsigned long)kp.kp_eproc.e_ppid;
        }
      }
#endif
      if (strcmp(leaf, "stat") == 0) {
        tmp_f = tmpfile();
        if (!tmp_f) {
          return NULL;
        }
        sprintf(buf,
                "%lu (%s) R %lu %lu %lu 0 -1 4194304 100 0 0 0 10 5 0 0 "
                "20 0 1 0 100 4096000 1000 18446744073709551615 0 0 0 0 0 0 "
                "0 0 0 0 0 0 17 0 0 0 0 0 0\n",
                pid, comm, ppid, pid, pid);
        fputs(buf, tmp_f);
        rewind(tmp_f);
        return tmp_f;
      }
      if (strcmp(leaf, "cmdline") == 0 || strcmp(leaf, "comm") == 0) {
        tmp_f = tmpfile();
        if (!tmp_f) {
          return NULL;
        }
        sprintf(buf, "%s\n", comm);
        fputs(buf, tmp_f);
        rewind(tmp_f);
        return tmp_f;
      }
      if (strcmp(leaf, "status") == 0) {
        unsigned int uid = (unsigned int)getuid();
        unsigned int gid = (unsigned int)getgid();
        tmp_f = tmpfile();
        if (!tmp_f) {
          return NULL;
        }
        sprintf(buf,
                "Name:\t%s\nState:\tS (sleeping)\nPid:\t%lu\nPPid:\t%lu\n"
                "Uid:\t%u\t%u\t%u\t%u\nGid:\t%u\t%u\t%u\t%u\n",
                comm, pid, ppid, uid, uid, uid, uid, gid, gid, gid, gid);
        fputs(buf, tmp_f);
        rewind(tmp_f);
        return tmp_f;
      }
    }
  }
  return NULL;
#endif
}

/** @brief Open stream with POSIX semantics. */
FILE *posix_fopen(const char *pathname, const char *mode) {
  FILE *vproc;
  if (!pathname || !mode) {
    errno = EINVAL;
    return NULL;
  }
  if (strcmp(pathname, "/proc/meminfo") == 0) {
    FILE *f_test = fopen("/proc/meminfo", mode);
    if (f_test) {
      return f_test;
    }
  }
  vproc = open_virtual_proc_stream_posix(pathname);
  if (vproc != NULL) {
    return vproc;
  }
  return fopen(pathname, mode);
}

/** @brief Associate stream with file descriptor or socket. */
FILE *posix_fdopen(intptr_t fd, const char *mode) {
  if (fd < 0 || !mode) {
    errno = EINVAL;
    return NULL;
  }
  return fdopen((int)fd, mode);
}

/** @brief Read a line from stream or socket-backed stream. */
char *posix_fgets(char *s, int size, FILE *stream) {
  if (!s || size <= 0 || !stream) {
    return NULL;
  }
  return fgets(s, size, stream);
}

/** @brief Write formatted output to stream or socket-backed stream. */
int posix_fprintf(FILE *stream, const char *format, ...) {
  va_list ap;
  int ret;
  if (!stream || !format) {
    return -1;
  }
  va_start(ap, format);
  ret = vfprintf(stream, format, ap);
  va_end(ap);
  return ret;
}

/** @brief Read binary data from stream or socket-backed stream. */
size_t posix_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  if (!ptr || size == 0 || nmemb == 0 || !stream) {
    return 0;
  }
  return fread(ptr, size, nmemb, stream);
}

/** @brief Write binary data to stream or socket-backed stream. */
size_t posix_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  if (!ptr || size == 0 || nmemb == 0 || !stream) {
    return 0;
  }
  return fwrite(ptr, size, nmemb, stream);
}

/** @brief Read a character from stream or socket-backed stream. */
int posix_fgetc(FILE *stream) {
  if (!stream) {
    return EOF;
  }
  return fgetc(stream);
}

/** @brief Close stream or socket-backed stream. */
int posix_fclose(FILE *stream) {
  if (!stream) {
    errno = EINVAL;
    return EOF;
  }
  return fclose(stream);
}

/** @brief Test end-of-file indicator on stream or socket-backed stream. */
int posix_feof(FILE *stream) {
  if (!stream) {
    return 0;
  }
  return feof(stream);
}

/** @brief Test error indicator on stream or socket-backed stream. */
int posix_ferror(FILE *stream) {
  if (!stream) {
    return 0;
  }
  return ferror(stream);
}

/** @brief Return integer file descriptor or socket for stream. */
int posix_fileno(FILE *stream) {
  if (!stream) {
    errno = EINVAL;
    return -1;
  }
  return fileno(stream);
}

/** @brief Open file descriptor with POSIX semantics. */
int posix_open(const char *pathname, int flags, ...) {
  va_list ap;
  int mode = 0;
  FILE *vproc;

  if (!pathname) {
    errno = EINVAL;
    return -1;
  }

  vproc = open_virtual_proc_stream_posix(pathname);
  if (vproc != NULL) {
    int vfd = dup(fileno(vproc));
    fclose(vproc);
    return vfd;
  }

  if (flags & O_CREAT) {
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    return open(pathname, flags, mode);
  }
  return open(pathname, flags);
}

/** @brief Create file descriptor with POSIX semantics. */
int posix_creat(const char *pathname, mode_t mode) {
  return creat(pathname, mode);
}

/** @brief Read from file descriptor with POSIX semantics. */
ssize_t posix_read(intptr_t fd, void *buf, size_t count) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  (void)buf;
  return (ssize_t)read((int)fd, buf, count);
}

/** @brief Write to file descriptor with POSIX semantics. */
ssize_t posix_write(intptr_t fd, const void *buf, size_t count) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  (void)buf;
  return (ssize_t)write((int)fd, buf, count);
}

/** @brief Close file descriptor with POSIX semantics. */
int posix_close(intptr_t fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return close((int)fd);
}

/** @brief Reposition read/write file offset with POSIX ESPIPE semantics. */
off_t posix_lseek(int fd, off_t offset, int whence) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return lseek(fd, offset, whence);
}

/** @brief Duplicate file descriptor with POSIX semantics. */
int posix_dup(int oldfd) {
  if (oldfd < 0) {
    errno = EBADF;
    return -1;
  }
  return dup(oldfd);
}

/** @brief Duplicate file descriptor with POSIX semantics. */
int posix_dup2(int oldfd, int newfd) {
  if (oldfd < 0 || newfd < 0) {
    errno = EBADF;
    return -1;
  }
  return dup2(oldfd, newfd);
}

#endif
#endif

typedef int make_iso_compilers_happy_posix_read_write;
