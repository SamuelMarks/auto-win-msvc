/* posix-core.c - Strict C89 Implementation */
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
/* clang-format off */
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#include <process.h>
#endif
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "posix-core.h"
#include <string.h>
/* clang-format on */

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif /* _MSC_VER */
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fcntl function. */

#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <stdint.h>
#else
#include <stddef.h>
#endif
#include <io.h>

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) int __stdcall ReadFile(void *hFile, void *lpBuffer,
                                             unsigned long nNumberOfBytesToRead,
                                             unsigned long *lpNumberOfBytesRead,
                                             void *lpOverlapped);
__declspec(dllimport) int __stdcall WriteFile(
    void *hFile, const void *lpBuffer, unsigned long nNumberOfBytesToWrite,
    unsigned long *lpNumberOfBytesWritten, void *lpOverlapped);
__declspec(dllimport) int __stdcall GetFileSizeEx(void *hFile,
                                                  __int64 *lpFileSize);
__declspec(dllimport) int __stdcall SetFilePointerEx(
    void *hFile, __int64 liDistanceToMove, __int64 *lpNewFilePointer,
    unsigned long dwMoveMethod);
__declspec(dllimport) int __stdcall SetEndOfFile(void *hFile);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall LockFileEx(
    void *hFile, unsigned long dwFlags, unsigned long dwReserved,
    unsigned long nNumberOfBytesToLockLow,
    unsigned long nNumberOfBytesToLockHigh, void *lpOverlapped);
__declspec(dllimport) int __stdcall UnlockFileEx(
    void *hFile, unsigned long dwReserved,
    unsigned long nNumberOfBytesToUnlockLow,
    unsigned long nNumberOfBytesToUnlockHigh, void *lpOverlapped);
__declspec(dllimport) int __stdcall ioctlsocket(uintptr_t s, long cmd,
                                                unsigned long *argp);

typedef struct _POSIX_SYSTEM_INFO {
  union {
    unsigned long dwOemId;
    struct {
      unsigned short wProcessorArchitecture;
      unsigned short wReserved;
    } s;
  } u;
  unsigned long dwPageSize;
  void *lpMinimumApplicationAddress;
  void *lpMaximumApplicationAddress;
  unsigned long *dwActiveProcessorMask;
  unsigned long dwNumberOfProcessors;
  unsigned long dwProcessorType;
  unsigned long dwAllocationGranularity;
  unsigned short wProcessorLevel;
  unsigned short wProcessorRevision;
} POSIX_SYSTEM_INFO;

__declspec(dllimport) void __stdcall GetSystemInfo(
    POSIX_SYSTEM_INFO *lpSystemInfo);

typedef struct _POSIX_MEMORYSTATUSEX {
  unsigned long dwLength;
  unsigned long dwMemoryLoad;
  unsigned __int64 ullTotalPhys;
  unsigned __int64 ullAvailPhys;
  unsigned __int64 ullTotalPageFile;
  unsigned __int64 ullAvailPageFile;
  unsigned __int64 ullTotalVirtual;
  unsigned __int64 ullAvailVirtual;
  unsigned __int64 ullAvailExtendedVirtual;
} POSIX_MEMORYSTATUSEX;

__declspec(dllimport) int __stdcall GlobalMemoryStatusEx(
    POSIX_MEMORYSTATUSEX *lpBuffer);
__declspec(dllimport) int __stdcall GetComputerNameA(char *lpBuffer,
                                                     unsigned long *nSize);
__declspec(dllimport) int __stdcall GetUserNameA(char *lpBuffer,
                                                 unsigned long *pcbBuffer);
__declspec(dllimport) int __stdcall CreateHardLinkA(
    const char *lpFileName, const char *lpExistingFileName,
    void *lpSecurityAttributes);
__declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
__declspec(dllimport) void *__stdcall GetModuleHandleA(
    const char *lpModuleName);
__declspec(dllimport) void *__stdcall GetProcAddress(void *hModule,
                                                     const char *lpProcName);
__declspec(dllimport) unsigned long __stdcall GetFileAttributesA(
    const char *lpFileName);

__declspec(dllimport) unsigned long __stdcall GetCurrentDirectoryA(
    unsigned long nBufferLength, char *lpBuffer);
__declspec(dllimport) int __stdcall SetCurrentDirectoryA(
    const char *lpPathName);

__declspec(dllimport) void *__stdcall CreateFileA(
    const char *lpFileName, unsigned long dwDesiredAccess,
    unsigned long dwShareMode, void *lpSecurityAttributes,
    unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes,
    void *hTemplateFile);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);

__declspec(dllimport) int __stdcall CreateTimerQueueTimer(
    void **phNewTimer, void *TimerQueue,
    void(__stdcall *Callback)(void *, unsigned char), void *Parameter,
    unsigned long DueTime, unsigned long Period, unsigned long Flags);
__declspec(dllimport) int __stdcall DeleteTimerQueueTimer(
    void *TimerQueue, void *Timer, void *CompletionEvent);
__declspec(dllimport) unsigned long __stdcall GetTickCount(void);

typedef size_t WIN_HCRYPTPROV;
typedef size_t WIN_HCRYPTHASH;
typedef size_t WIN_HCRYPTKEY;

__declspec(dllimport) int __stdcall CryptAcquireContextA(
    WIN_HCRYPTPROV *phProv, const char *pszContainer, const char *pszProvider,
    unsigned long dwProvType, unsigned long dwFlags);
__declspec(dllimport) int __stdcall CryptCreateHash(WIN_HCRYPTPROV hProv,
                                                    unsigned int Algid,
                                                    WIN_HCRYPTKEY hKey,
                                                    unsigned long dwFlags,
                                                    WIN_HCRYPTHASH *phHash);
__declspec(dllimport) int __stdcall CryptHashData(WIN_HCRYPTHASH hHash,
                                                  const unsigned char *pbData,
                                                  unsigned long dwDataLen,
                                                  unsigned long dwFlags);
__declspec(dllimport) int __stdcall CryptGetHashParam(WIN_HCRYPTHASH hHash,
                                                      unsigned long dwParam,
                                                      unsigned char *pbData,
                                                      unsigned long *pdwDataLen,
                                                      unsigned long dwFlags);
__declspec(dllimport) int __stdcall CryptDestroyHash(WIN_HCRYPTHASH hHash);
__declspec(dllimport) int __stdcall CryptReleaseContext(WIN_HCRYPTPROV hProv,
                                                        unsigned long dwFlags);
__declspec(dllimport) int __stdcall CryptDeriveKey(WIN_HCRYPTPROV hProv,
                                                   unsigned int Algid,
                                                   WIN_HCRYPTHASH hBaseData,
                                                   unsigned long dwFlags,
                                                   WIN_HCRYPTKEY *phKey);
__declspec(dllimport) int __stdcall CryptEncrypt(
    WIN_HCRYPTKEY hKey, WIN_HCRYPTHASH hHash, int Final, unsigned long dwFlags,
    unsigned char *pbData, unsigned long *pdwDataLen, unsigned long dwBufLen);
__declspec(dllimport) int __stdcall CryptDecrypt(
    WIN_HCRYPTKEY hKey, WIN_HCRYPTHASH hHash, int Final, unsigned long dwFlags,
    unsigned char *pbData, unsigned long *pdwDataLen);
__declspec(dllimport) int __stdcall CryptDestroyKey(WIN_HCRYPTKEY hKey);

#ifdef __cplusplus
}
#endif

__declspec(dllimport) void *__stdcall GetCurrentProcess(void);
__declspec(dllimport) int __stdcall OpenProcessToken(
    void *ProcessHandle, unsigned long DesiredAccess, void **TokenHandle);
__declspec(dllimport) int __stdcall GetTokenInformation(
    void *TokenHandle, int TokenInformationClass, void *TokenInformation,
    unsigned long TokenInformationLength, unsigned long *ReturnLength);
__declspec(dllimport) void *__stdcall GetSidSubAuthority(
    void *pSid, unsigned long nSubAuthority);
__declspec(dllimport) unsigned char *__stdcall GetSidSubAuthorityCount(
    void *pSid);

typedef unsigned char BOOLEAN;
typedef BOOLEAN(__stdcall *CreateSymbolicLinkA_Func)(
    const char *lpSymlinkFileName, const char *lpTargetFileName,
    unsigned long dwFlags);
typedef unsigned long(__stdcall *GetFinalPathNameByHandleA_Func)(
    void *hFile, char *lpszFilePath, unsigned long cchFilePath,
    unsigned long dwFlags);

typedef uintptr_t SOCKET;

typedef struct _local_overlapped {
  uintptr_t Internal;
  uintptr_t InternalHigh;
  unsigned long Offset;
  unsigned long OffsetHigh;
  void *hEvent;
} local_overlapped;

typedef int(__stdcall *OpenProcessToken_f)(void *, unsigned long, void **);
typedef int(__stdcall *GetTokenInformation_f)(void *, int, void *,
                                              unsigned long, unsigned long *);
typedef void *(__stdcall *GetSidSubAuthority_f)(void *, unsigned long);
typedef unsigned char *(__stdcall *GetSidSubAuthorityCount_f)(void *);
typedef int(__stdcall *IsValidSid_f)(void *);

static unsigned long get_current_rid(int is_group) {
  void *hToken = NULL;
  unsigned long rid = 0;
  void *advapi32;
  OpenProcessToken_f pOpenProcessToken;
  GetTokenInformation_f pGetTokenInformation;
  GetSidSubAuthority_f pGetSidSubAuthority;
  GetSidSubAuthorityCount_f pGetSidSubAuthorityCount;
  IsValidSid_f pIsValidSid;

  advapi32 = GetModuleHandleA("advapi32.dll");
  if (!advapi32) {
    return 0;
  }

  pOpenProcessToken = (OpenProcessToken_f)(intptr_t)GetProcAddress(
      advapi32, "OpenProcessToken");
  pGetTokenInformation = (GetTokenInformation_f)(intptr_t)GetProcAddress(
      advapi32, "GetTokenInformation");
  pGetSidSubAuthority = (GetSidSubAuthority_f)(intptr_t)GetProcAddress(
      advapi32, "GetSidSubAuthority");
  pGetSidSubAuthorityCount =
      (GetSidSubAuthorityCount_f)(intptr_t)GetProcAddress(
          advapi32, "GetSidSubAuthorityCount");
  pIsValidSid = (IsValidSid_f)(intptr_t)GetProcAddress(advapi32, "IsValidSid");

  if (!pOpenProcessToken || !pGetTokenInformation || !pGetSidSubAuthority ||
      !pGetSidSubAuthorityCount || !pIsValidSid) {
    return 0;
  }

  if (pOpenProcessToken((void *)(intptr_t)-1, 0x0008 /* TOKEN_QUERY */,
                        &hToken)) {
    unsigned long dwLength = 0;
    int token_class = is_group ? 2 /* TokenPrimaryGroup */ : 1 /* TokenUser */;
    pGetTokenInformation(hToken, token_class, NULL, 0, &dwLength);
    if (dwLength > 0) {
      void *pInfo = malloc(dwLength);
      if (pInfo) {
        if (pGetTokenInformation(hToken, token_class, pInfo, dwLength,
                                 &dwLength)) {
          void *pSid =
              *(void **)pInfo; /* Both TOKEN_USER and TOKEN_PRIMARY_GROUP start
                                  with a PSID */
          if (pSid && pIsValidSid(pSid)) {
            unsigned char *pCount = pGetSidSubAuthorityCount(pSid);
            if (pCount && *pCount > 0) {
              unsigned long *pRid =
                  (unsigned long *)pGetSidSubAuthority(pSid, *pCount - 1);
              if (pRid) {
                rid = *pRid;
              }
            }
          }
        }
        free(pInfo);
      }
    }
    CloseHandle(hToken);
  }
  return rid;
}

#define FIONBIO 0x8004667E

#include <fcntl.h>
#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif

extern void set_nonblock(SOCKET s, int nb);
extern int get_nonblock(SOCKET s);
extern void clear_nonblock(SOCKET s);
extern void copy_nonblock(SOCKET src, SOCKET dst);
extern void mark_as_socket(int fd);
extern void clear_as_socket(int fd);
extern int is_socket(int fd);

__declspec(dllimport) void __stdcall ExitProcess(unsigned int uExitCode);
void my_exit_hook(int status) { ExitProcess((unsigned int)status); }

__declspec(dllimport) void *__stdcall GetModuleHandleA(const char *);
__declspec(dllimport) void *__stdcall GetProcAddress(void *, const char *);

void auto_win_exit(int code) {
  int(__stdcall * tp)(void *, unsigned int) = (void *)(size_t)GetProcAddress(
      GetModuleHandleA("kernel32.dll"), "TerminateProcess");
  if (tp)
    tp((void *)-1LL, code);
}

int fcntl(int fd, int cmd, ...) {
  va_list ap;
  va_start(ap, cmd);

  if (cmd == F_GETFD || cmd == F_SETFD) {
    va_end(ap);
    return 0;
  }
  if (cmd == F_GETFL) {
    va_end(ap);
    return get_nonblock((SOCKET)(unsigned int)fd);
  }
  if (cmd == F_SETFL) {
    int flags = va_arg(ap, int);
    unsigned long mode = (flags & O_NONBLOCK) ? 1 : 0;
    SOCKET s = (SOCKET)(unsigned int)fd;
    ioctlsocket(s, FIONBIO, &mode);
    set_nonblock(s, mode);
    va_end(ap);
    return 0;
  }

  if (cmd == F_SETLK || cmd == F_SETLKW || cmd == F_GETLK) {
    struct flock *fl = va_arg(ap, struct flock *);
    void *handle;
    local_overlapped ov;
    unsigned long flags = 0;
    __int64 start, len;
    int res;

    va_end(ap);
    if (!fl) {
      errno = EINVAL;
      return -1;
    }

    handle = (void *)_get_osfhandle(fd);
    if (handle == (void *)(intptr_t)-1) {
      errno = EBADF;
      return -1;
    }

    memset(&ov, 0, sizeof(ov));

    /* Calculate start offset */
    if (fl->l_whence == SEEK_SET) {
      start = fl->l_start;
    } else if (fl->l_whence == SEEK_CUR) {
      __int64 current_pos;
      if (!SetFilePointerEx(handle, 0, &current_pos, 1)) { /* FILE_CURRENT */
        errno = EINVAL;
        return -1;
      }
      start = current_pos + fl->l_start;
    } else if (fl->l_whence == SEEK_END) {
      __int64 file_size;
      if (!GetFileSizeEx(handle, &file_size)) {
        errno = EINVAL;
        return -1;
      }
      start = file_size + fl->l_start;
    } else {
      errno = EINVAL;
      return -1;
    }

    if (start < 0) {
      errno = EINVAL;
      return -1;
    }

    len = fl->l_len;
    if (len == 0) {
      /* POSIX semantics: lock to EOF. We use maximum possible length */
      unsigned long max_dw = 0xFFFFFFFFUL;
      __int64 max_len = ((__int64)max_dw << 32) | max_dw;
      /* Prevent overflow since len is signed */
      max_len = (__int64)((unsigned __int64)max_len >> 1);
      len = max_len - start;
    }

    ov.Offset = (unsigned long)(start & 0xFFFFFFFF);
    ov.OffsetHigh = (unsigned long)(start >> 32);

    if (cmd == F_GETLK) {
      flags = 2; /* LOCKFILE_FAIL_IMMEDIATELY */
      if (fl->l_type == F_WRLCK) {
        flags |= 1; /* LOCKFILE_EXCLUSIVE_LOCK */
      }
      if (LockFileEx(handle, flags, 0, (unsigned long)(len & 0xFFFFFFFF),
                     (unsigned long)(len >> 32), &ov)) {
        UnlockFileEx(handle, 0, (unsigned long)(len & 0xFFFFFFFF),
                     (unsigned long)(len >> 32), &ov);
        fl->l_type = F_UNLCK;
      } else {
        if (fl->l_type == F_UNLCK) {
          fl->l_type = F_WRLCK;
        }
        fl->l_pid = 0;
      }
      return 0;
    }

    if (fl->l_type == F_UNLCK) {
      UnlockFileEx(handle, 0, (unsigned long)(len & 0xFFFFFFFF),
                   (unsigned long)(len >> 32), &ov);
      return 0;
    }

    if (cmd == F_SETLK) {
      flags |= 2; /* LOCKFILE_FAIL_IMMEDIATELY */
    }
    if (fl->l_type == F_WRLCK) {
      flags |= 1; /* LOCKFILE_EXCLUSIVE_LOCK */
    }

    res = LockFileEx(handle, flags, 0, (unsigned long)(len & 0xFFFFFFFF),
                     (unsigned long)(len >> 32), &ov);
    if (!res) {
      unsigned long err = GetLastError();
      if (err == 33 ||
          err == 997) { /* ERROR_LOCK_VIOLATION or ERROR_IO_PENDING */
        errno = EAGAIN;
      } else {
        errno = EINVAL;
      }
      return -1;
    }
    return 0;
  }

  va_end(ap);
  errno = ENOSYS;
  return -1;
}

#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

static int posix_resolve_at_path(int dirfd, const char *pathname,
                                 char **buf_out) {
  char *buf;
  size_t bufsiz = 32768;
  if (!pathname || !buf_out) {
    errno = EINVAL;
    return -1;
  }

  buf = (char *)malloc(bufsiz);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  if (pathname[0] == '/' || pathname[0] == '\\' ||
      (pathname[0] && pathname[1] == ':')) {
    strncpy(buf, pathname, bufsiz - 1);
    buf[bufsiz - 1] = '\0';
    *buf_out = buf;
    return 0;
  }

  if (dirfd == AT_FDCWD) {
    if (!_getcwd(buf, (int)bufsiz)) {
      free(buf);
      return -1;
    }
    if (strlen(buf) + strlen(pathname) + 2 > bufsiz) {
      free(buf);
      errno = ENAMETOOLONG;
      return -1;
    }
    if (buf[strlen(buf) - 1] != '\\' && buf[strlen(buf) - 1] != '/')
      strcat(buf, "\\");
    strcat(buf, pathname);
    *buf_out = buf;
    return 0;
  } else {
    void *kernel32;
    void *hFile;
    unsigned long res;
    GetFinalPathNameByHandleA_Func pGetFinalPathNameByHandleA;

    hFile = (void *)(intptr_t)_get_osfhandle(dirfd);
    if (hFile == (void *)(intptr_t)-1) {
      free(buf);
      errno = EBADF;
      return -1;
    }

    kernel32 = GetModuleHandleA("kernel32.dll");
    if (!kernel32) {
      free(buf);
      errno = ENOSYS;
      return -1;
    }
    pGetFinalPathNameByHandleA =
        (GetFinalPathNameByHandleA_Func)(intptr_t)GetProcAddress(
            kernel32, "GetFinalPathNameByHandleA");
    if (!pGetFinalPathNameByHandleA) {
      free(buf);
      errno = ENOSYS;
      return -1;
    }

    res = pGetFinalPathNameByHandleA(hFile, buf, (unsigned long)bufsiz, 0);
    if (res == 0 || res >= bufsiz) {
      free(buf);
      errno = EINVAL;
      return -1;
    }
    if (res >= 4 && buf[0] == '\\' && buf[1] == '\\' && buf[2] == '?' &&
        buf[3] == '\\') {
      memmove(buf, buf + 4, res - 4);
      res -= 4;
      buf[res] = '\0';
    }
    if (res + strlen(pathname) + 2 > bufsiz) {
      free(buf);
      errno = ENAMETOOLONG;
      return -1;
    }
    if (buf[res - 1] != '\\' && buf[res - 1] != '/')
      strcat(buf, "\\");
    strcat(buf, pathname);
    *buf_out = buf;
    return 0;
  }
}

/** \brief openat function. */
int openat(int dirfd, const char *pathname, int flags, ...) {
  char *buf;
  int fd;
  va_list ap;
  int mode = 0;

  if (posix_resolve_at_path(dirfd, pathname, &buf) == -1)
    return -1;

  if (flags & _O_CREAT) {
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
  }

  fd = _open(buf, flags, mode);
  free(buf);
  return fd;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) void *__stdcall CreateFileMappingA(
    void *hFile, void *lpFileMappingAttributes, unsigned long flProtect,
    unsigned long dwMaximumSizeHigh, unsigned long dwMaximumSizeLow,
    const char *lpName);
__declspec(dllimport) void *__stdcall MapViewOfFile(
    void *hFileMappingObject, unsigned long dwDesiredAccess,
    unsigned long dwFileOffsetHigh, unsigned long dwFileOffsetLow,
    size_t dwNumberOfBytesToMap);
__declspec(dllimport) int __stdcall UnmapViewOfFile(const void *lpBaseAddress);

/** \brief posix_fadvise function. */
int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
  intptr_t handle = _get_osfhandle(fd);
  if (handle == -1) {
    return EBADF;
  }
  if (offset < 0 || len < 0) {
    return EINVAL;
  }

  if (advice == 1 /* POSIX_FADV_WILLNEED */) {
    void *hMap;
    void *ptr;
    void *kernel32;
    int(__stdcall * pPrefetchVirtualMemory)(void *, size_t, void *,
                                            unsigned long);
    __int64 file_size;
    size_t map_len;

    kernel32 = GetModuleHandleA("kernel32.dll");
    if (!kernel32)
      return 0;
    pPrefetchVirtualMemory =
        (int(__stdcall *)(void *, size_t, void *, unsigned long))(
            intptr_t)GetProcAddress(kernel32, "PrefetchVirtualMemory");
    if (!pPrefetchVirtualMemory)
      return 0;

    if (!GetFileSizeEx((void *)handle, &file_size)) {
      return 0;
    }

    if (offset >= file_size) {
      return 0;
    }

    if (len == 0 || offset + len > file_size) {
      map_len = (size_t)(file_size - offset);
    } else {
      map_len = (size_t)len;
    }

    /* Windows requires map offsets to be a multiple of allocation granularity
     */
    /* For simplicity, we just map from 0 to offset + map_len if offset is not
     * aligned */
    /* A more robust implementation would align the offset */
    hMap = CreateFileMappingA((void *)handle, NULL, 0x02 /* PAGE_READONLY */, 0,
                              0, NULL);
    if (!hMap)
      return 0;

    ptr = MapViewOfFile(hMap, 0x0004 /* FILE_MAP_READ */, 0, 0,
                        0); /* Map entire file */
    if (ptr) {
      struct {
        void *VirtualAddress;
        size_t NumberOfBytes;
      } entry;
      entry.VirtualAddress = (void *)((unsigned char *)ptr + offset);
      entry.NumberOfBytes = map_len;
      pPrefetchVirtualMemory(GetCurrentProcess(), 1, &entry, 0);
      UnmapViewOfFile(ptr);
    }
    CloseHandle(hMap);
  } else if (advice == 4 /* POSIX_FADV_DONTNEED */) {
    /* POSIX DONTNEED means discard cache. FlushFileBuffers syncs, but doesn't
     * discard. */
    /* No direct equivalent for discarding file cache on Windows without
     * unbuffered I/O. */
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief posix_fallocate function. */
int posix_fallocate(int fd, off_t offset, off_t len) {
  intptr_t handle;
  __int64 current_size;
  __int64 target_size;
  __int64 old_ptr;

  if (fd < 0) {
    return EBADF;
  }
  if (offset < 0 || len <= 0) {
    return EINVAL;
  }

  handle = _get_osfhandle(fd);
  if (handle == -1) {
    return EBADF;
  }

  target_size = (__int64)offset + (__int64)len;

  if (!GetFileSizeEx((void *)handle, &current_size)) {
    return EINVAL;
  }

  if (target_size > current_size) {
    /* Save current pointer */
    if (!SetFilePointerEx((void *)handle, 0, &old_ptr, 1)) { /* FILE_CURRENT */
      return EINVAL;
    }

    /* Set new EOF */
    if (!SetFilePointerEx((void *)handle, target_size, NULL,
                          0)) { /* FILE_BEGIN */
      return EINVAL;
    }

    if (!SetEndOfFile((void *)handle)) {
      SetFilePointerEx((void *)handle, old_ptr, NULL, 0);
      return ENOSPC;
    }

    /* Restore pointer */
    SetFilePointerEx((void *)handle, old_ptr, NULL, 0);
  }

  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief sync_file_range function. */
int sync_file_range(int fd, off_t offset, off_t nbytes, unsigned int flags) {
  (void)offset;
  (void)nbytes;
  (void)flags;
  if (_commit(fd) == 0) {
    return 0;
  }
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
static void *g_alarm_timer = NULL;
static unsigned long g_alarm_remaining_ms = 0;
static unsigned long g_alarm_start_ticks = 0;

static void __stdcall alarm_timer_callback(void *lpParameter,
                                           unsigned char TimerOrWaitFired) {
  (void)lpParameter;
  (void)TimerOrWaitFired;
  /* SIGALRM = 14 */
  raise(14);
}

/** \brief alarm function. */
unsigned int alarm(unsigned int seconds) {
  unsigned int remaining = 0;
  if (g_alarm_timer) {
    unsigned long elapsed = GetTickCount() - g_alarm_start_ticks;
    if (elapsed < g_alarm_remaining_ms) {
      remaining = (g_alarm_remaining_ms - elapsed) / 1000;
      if (remaining == 0 && (g_alarm_remaining_ms - elapsed) > 0) {
        remaining = 1;
      }
    }
    DeleteTimerQueueTimer(NULL, g_alarm_timer, NULL);
    g_alarm_timer = NULL;
  }

  if (seconds > 0) {
    g_alarm_start_ticks = GetTickCount();
    g_alarm_remaining_ms = seconds * 1000;
    CreateTimerQueueTimer(&g_alarm_timer, NULL, alarm_timer_callback, NULL,
                          g_alarm_remaining_ms, 0, 0x00000000);
  }

  return remaining;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief chown function. */
int chown(const char *pathname, uid_t owner, gid_t group) {
  unsigned long attr;
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
  attr = GetFileAttributesA(pathname);
  if (attr == 0xFFFFFFFFUL) {
    errno = ENOENT;
    return -1;
  }
  if (owner != (uid_t)-1 && owner != getuid()) {
    errno = EPERM;
    return -1;
  }
  if (group != (gid_t)-1 && group != getgid()) {
    int i;
    int num_groups = getgroups(0, NULL);
    if (num_groups > 0) {
      gid_t *groups = (gid_t *)malloc(num_groups * sizeof(gid_t));
      if (groups) {
        int found = 0;
        if (getgroups(num_groups, groups) == num_groups) {
          for (i = 0; i < num_groups; i++) {
            if (groups[i] == group) {
              found = 1;
              break;
            }
          }
        }
        free(groups);
        if (found)
          return 0;
      }
    }
    errno = EPERM;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief confstr function. */
size_t confstr(int name, char *buf, size_t len) {
  if (name == _CS_PATH) {
    const char *path = "C:\\Windows\\System32;C:\\Windows";
    size_t req = strlen(path) + 1;
    if (buf && len > 0) {
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
      strncpy_s(buf, len, path, _TRUNCATE);
#else
      strncpy(buf, path, len - 1);
      buf[len - 1] = '\0';
#endif
    }
    return req;
  }
  errno = EINVAL;
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
static const char crypt_b64[] =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

char *crypt(const char *key, const char *salt) {
  static char result[64];
  WIN_HCRYPTPROV hProv;
  WIN_HCRYPTHASH hHash;
  unsigned char hash[16];
  unsigned long hashLen = 16;
  int i;
  char *p;

  if (!key || !salt) {
    errno = EINVAL;
    return NULL;
  }

  if (!CryptAcquireContextA(&hProv, NULL, NULL, 1 /* PROV_RSA_FULL */,
                            0xF0000000 /* CRYPT_VERIFYCONTEXT */)) {
    errno = ENOSYS;
    return NULL;
  }

  if (!CryptCreateHash(hProv, 0x00008003 /* CALG_MD5 */, 0, 0, &hHash)) {
    CryptReleaseContext(hProv, 0);
    errno = ENOSYS;
    return NULL;
  }

  CryptHashData(hHash, (const unsigned char *)key, (unsigned long)strlen(key),
                0);
  CryptHashData(hHash, (const unsigned char *)salt, (unsigned long)strlen(salt),
                0);

  if (!CryptGetHashParam(hHash, 0x0002 /* HP_HASHVAL */, hash, &hashLen, 0)) {
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    errno = ENOSYS;
    return NULL;
  }

  CryptDestroyHash(hHash);
  CryptReleaseContext(hProv, 0);

  p = result;
  *p++ = '$';
  *p++ = '1';
  *p++ = '$';
  for (i = 0; i < (int)strlen(salt) && i < 8; i++) {
    *p++ = salt[i];
  }
  *p++ = '$';

  for (i = 0; i < 15; i += 3) {
    unsigned long v = (hash[i] << 16) | (hash[i + 1] << 8) | hash[i + 2];
    *p++ = crypt_b64[(v >> 18) & 0x3f];
    *p++ = crypt_b64[(v >> 12) & 0x3f];
    *p++ = crypt_b64[(v >> 6) & 0x3f];
    *p++ = crypt_b64[v & 0x3f];
  }
  {
    unsigned long v = hash[15] << 16;
    *p++ = crypt_b64[(v >> 18) & 0x3f];
    *p++ = crypt_b64[(v >> 12) & 0x3f];
  }
  *p = '\0';

  return result;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief encrypt function. */
void encrypt(char block[64], int edflag) {
  WIN_HCRYPTPROV hProv;
  WIN_HCRYPTHASH hHash;
  WIN_HCRYPTKEY hKey;
  unsigned char packed[8] = {0};
  unsigned long len = 8;
  int i;

  if (!block) {
    errno = EINVAL;
    return;
  }

  if (!CryptAcquireContextA(&hProv, NULL, NULL, 1 /* PROV_RSA_FULL */,
                            0xF0000000 /* CRYPT_VERIFYCONTEXT */)) {
    errno = ENOSYS;
    return;
  }

  if (!CryptCreateHash(hProv, 0x00008003 /* CALG_MD5 */, 0, 0, &hHash)) {
    CryptReleaseContext(hProv, 0);
    errno = ENOSYS;
    return;
  }

  CryptHashData(hHash, (const unsigned char *)"POSIX_ENCRYPT_KEY", 17, 0);

  if (!CryptDeriveKey(hProv, 0x00006601 /* CALG_DES */, hHash, 0, &hKey)) {
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    errno = ENOSYS;
    return;
  }

  for (i = 0; i < 64; i++) {
    if (block[i])
      packed[i >> 3] |= (1 << (7 - (i & 7)));
  }

  if (edflag == 0) {
    if (!CryptEncrypt(hKey, 0, 0, 0, packed, &len, 8)) {
      /* Encrypt Failed */
    }
  } else {
    if (!CryptDecrypt(hKey, 0, 0, 0, packed, &len)) {
      /* Decrypt Failed */
    }
  }

  for (i = 0; i < 64; i++) {
    block[i] = (packed[i >> 3] & (1 << (7 - (i & 7)))) ? 1 : 0;
  }

  CryptDestroyKey(hKey);
  CryptDestroyHash(hHash);
  CryptReleaseContext(hProv, 0);
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief faccessat function. */
int faccessat(int dirfd, const char *pathname, int mode, int flags) {
  char *buf;
  int res;
  (void)flags; /* AT_EACCESS, AT_SYMLINK_NOFOLLOW etc not fully supported */
  if (posix_resolve_at_path(dirfd, pathname, &buf) == -1)
    return -1;
  res = _access(buf, mode);
  free(buf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fchown function. */
int fchown(int fd, uid_t owner, gid_t group) {
  intptr_t handle;
  void *kernel32;
  GetFinalPathNameByHandleA_Func pGetFinalPathNameByHandleA;
  char *buf;
  unsigned long res;
  int chown_res;

  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  handle = _get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  kernel32 = GetModuleHandleA("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA =
      (GetFinalPathNameByHandleA_Func)(intptr_t)GetProcAddress(
          kernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return -1;
  }

  buf = (char *)malloc(32768);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  res = pGetFinalPathNameByHandleA((void *)handle, buf, 32768, 0);
  if (res == 0 || res >= 32768) {
    free(buf);
    errno = EBADF;
    return -1;
  }

  if (res >= 4 && buf[0] == '\\' && buf[1] == '\\' && buf[2] == '?' &&
      buf[3] == '\\') {
    memmove(buf, buf + 4, res - 4);
    res -= 4;
    buf[res] = '\0';
  }

  chown_res = chown(buf, owner, group);
  free(buf);
  return chown_res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fchownat function. */
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags) {
  char *buf;
  int res;
  flags = flags; /* AT_SYMLINK_NOFOLLOW ignored */
  if (posix_resolve_at_path(dirfd, pathname, &buf) == -1) {
    return -1;
  }
  res = chown(buf, owner, group);
  free(buf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fdatasync function. */
int fdatasync(int fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (_commit(fd) == -1) {
    errno = EBADF;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fexecve function. */
int fexecve(int fd, char *const argv[], char *const envp[]) {
  char *buf = NULL;
  intptr_t handle;
  void *kernel32;
  GetFinalPathNameByHandleA_Func pGetFinalPathNameByHandleA;
  unsigned long res;

  if (fd < 0 || !argv) {
    errno = EINVAL;
    return -1;
  }

  handle = _get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  kernel32 = GetModuleHandleA("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA =
      (GetFinalPathNameByHandleA_Func)(intptr_t)GetProcAddress(
          kernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return -1;
  }

  buf = (char *)malloc(32768);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  res = pGetFinalPathNameByHandleA((void *)handle, buf, 32768, 0);
  if (res == 0 || res >= 32768) {
    free(buf);
    errno = EBADF;
    return -1;
  }

  if (res >= 4 && buf[0] == '\\' && buf[1] == '\\' && buf[2] == '?' &&
      buf[3] == '\\') {
    memmove(buf, buf + 4, res - 4);
    res -= 4;
    buf[res] = '\0';
  }

  _execve(buf, (const char *const *)argv, (const char *const *)envp);
  free(buf);
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((unsigned long)0x00000000L)
#endif
#ifndef STATUS_PROCESS_CLONED
#define STATUS_PROCESS_CLONED ((unsigned long)0x00000129L)
#endif

#define RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES 0x00000002

typedef struct _RTL_CLIENT_ID {
  void *UniqueProcess;
  void *UniqueThread;
} RTL_CLIENT_ID;

typedef struct _RTL_SECTION_IMAGE_INFORMATION {
  void *TransferAddress;
  unsigned long ZeroBits;
  size_t MaximumStackSize;
  size_t CommittedStackSize;
  unsigned long SubSystemType;
  union {
    struct {
      unsigned short SubSystemMinorVersion;
      unsigned short SubSystemMajorVersion;
    } s;
    unsigned long SubSystemVersion;
  } u;
  unsigned long GdiHandleBuffer[34];
} RTL_SECTION_IMAGE_INFORMATION;

typedef struct _RTL_USER_PROCESS_INFORMATION {
  unsigned long Length;
  void *Process;
  void *Thread;
  RTL_CLIENT_ID ClientId;
  RTL_SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION;

typedef unsigned long(__stdcall *RtlCloneUserProcess_f)(
    unsigned long ProcessFlags, void *ProcessSecurityDescriptor,
    void *ThreadSecurityDescriptor, void *DebugPort,
    RTL_USER_PROCESS_INFORMATION *ProcessInformation);

void posix_pthread_atfork_prepare(void);
void posix_pthread_atfork_parent(void);
void posix_pthread_atfork_child(void);
#if defined(_MSC_VER)
void posix_core_dummy_atfork(void) {}
#if defined(_M_IX86)
#pragma comment(                                                               \
    linker,                                                                    \
    "/alternatename:_posix_pthread_atfork_prepare=_posix_core_dummy_atfork")
#pragma comment(                                                               \
    linker,                                                                    \
    "/alternatename:_posix_pthread_atfork_parent=_posix_core_dummy_atfork")
#pragma comment(                                                               \
    linker,                                                                    \
    "/alternatename:_posix_pthread_atfork_child=_posix_core_dummy_atfork")
#else
#pragma comment(                                                               \
    linker,                                                                    \
    "/alternatename:posix_pthread_atfork_prepare=posix_core_dummy_atfork")
#pragma comment(                                                               \
    linker,                                                                    \
    "/alternatename:posix_pthread_atfork_parent=posix_core_dummy_atfork")
#pragma comment(                                                               \
    linker,                                                                    \
    "/alternatename:posix_pthread_atfork_child=posix_core_dummy_atfork")
#endif
#else
__attribute__((weak)) void posix_pthread_atfork_prepare(void) {}
__attribute__((weak)) void posix_pthread_atfork_parent(void) {}
__attribute__((weak)) void posix_pthread_atfork_child(void) {}
#endif

#define MAX_POSIX_CHILDREN 1024
__declspec(selectany) void *g_posix_child_handles[MAX_POSIX_CHILDREN] = {0};
__declspec(selectany) unsigned long g_posix_child_pids[MAX_POSIX_CHILDREN] = {
    0};
__declspec(selectany) int g_posix_child_count = 0;

/**  rief fork function. */
pid_t fork(void) {
  void *ntdll;
  RtlCloneUserProcess_f pRtlCloneUserProcess;
  RTL_USER_PROCESS_INFORMATION info;
  unsigned long status;
  char parent_cwd[260];

  ntdll = GetModuleHandleA("ntdll.dll");
  if (!ntdll) {
    errno = ENOSYS;
    return -1;
  }

  pRtlCloneUserProcess = (RtlCloneUserProcess_f)(size_t)GetProcAddress(
      ntdll, "RtlCloneUserProcess");
  if (!pRtlCloneUserProcess) {
    errno = ENOSYS;
    return -1;
  }

  memset(&info, 0, sizeof(info));
  info.Length = sizeof(info);

  posix_pthread_atfork_prepare();

  GetCurrentDirectoryA(260, parent_cwd);

  status = pRtlCloneUserProcess(RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES, NULL,
                                NULL, NULL, &info);

  if (status == STATUS_SUCCESS) {
    posix_pthread_atfork_parent();

    if (g_posix_child_count < 1024) {
      g_posix_child_handles[g_posix_child_count] = info.Process;
      g_posix_child_pids[g_posix_child_count] =
          (unsigned long)(size_t)info.ClientId.UniqueProcess;
      g_posix_child_count++;
    } else {
      CloseHandle(info.Process);
    }
    CloseHandle(info.Thread);
    return (pid_t)(size_t)info.ClientId.UniqueProcess;
  } else if (status == STATUS_PROCESS_CLONED) {
    void *ucrt;
    SetCurrentDirectoryA(parent_cwd);
    posix_pthread_atfork_child();
    /* Child */
    ucrt = GetModuleHandleA("ucrtbase.dll");
    if (ucrt) {
      void *p_exits[2];
      int i;
      p_exits[0] = (void *)(size_t)GetProcAddress(ucrt, "_exit");
      p_exits[1] = (void *)(size_t)GetProcAddress(ucrt, "exit");
      for (i = 0; i < 2; i++) {
        void *p_exit = p_exits[i];
        if (p_exit) {
          unsigned long oldProtect;
          int(__stdcall * vp)(void *, size_t, unsigned long, unsigned long *) =
              (void *)(size_t)GetProcAddress(GetModuleHandleA("kernel32.dll"),
                                             "VirtualProtect");
          if (vp && vp(p_exit, 12, 0x40, &oldProtect)) {
            unsigned char *p = (unsigned char *)p_exit;
            void *target = (void *)(size_t)my_exit_hook;
            p[0] = 0x48;
            p[1] = 0xB8;
            memcpy(p + 2, &target, 8);
            p[10] = 0xFF;
            p[11] = 0xE0;
            vp(p_exit, 12, oldProtect, &oldProtect);
          }
        }
      }
    }
    return 0;
  }

  errno = EAGAIN;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief fpathconf function. */
long fpathconf(int fd, int name) {
  (void)fd;
  switch (name) {
  case _PC_NAME_MAX:
    return 255;
  case _PC_PATH_MAX:
    return 260;
  case _PC_PIPE_BUF:
    return 4096;
  default:
    errno = EINVAL;
    return -1;
  }
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getegid function. */
gid_t getegid(void) { return (gid_t)get_current_rid(1); }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief geteuid function. */
uid_t geteuid(void) { return (uid_t)get_current_rid(0); }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getgid function. */
gid_t getgid(void) { return (gid_t)get_current_rid(1); }
#endif
typedef struct _SID_AND_ATTRIBUTES {
  void *Sid;
  unsigned long Attributes;
} SID_AND_ATTRIBUTES;

typedef struct _TOKEN_GROUPS {
  unsigned long GroupCount;
  SID_AND_ATTRIBUTES Groups[1];
} TOKEN_GROUPS;

#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getgroups function. */
int getgroups(int size, gid_t list[]) {
  if (size < 0) {
    errno = EINVAL;
    return -1;
  }
  if (size > 0 && list != NULL) {
    list[0] = (gid_t)get_current_rid(1);
  }
  return 1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief gethostid function. */
long gethostid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief gethostname function. */
int gethostname_removed(char *name, int len) {
  unsigned long size = (unsigned long)len;
  if (name == NULL || len <= 0) {
    errno = EINVAL;
    return -1;
  }
  if (!GetComputerNameA(name, &size)) {
    errno = EFAULT;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getlogin function. */
char *getlogin(void) {
  static char buffer[256];
  unsigned long size = sizeof(buffer);
  if (GetUserNameA(buffer, &size)) {
    return buffer;
  }
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getlogin_r function. */
int getlogin_r(char *buf, size_t bufsize) {
  unsigned long size = (unsigned long)bufsize;
  if (buf == NULL || bufsize <= 0) {
    return ERANGE;
  }
  if (!GetUserNameA(buf, &size)) {
    return ERANGE;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) && defined(_MSC_VER)

POSIX_CORE_API char *optarg = NULL;
POSIX_CORE_API int optind = 1;
POSIX_CORE_API int opterr = 1;
POSIX_CORE_API int optopt = 0;

static char *nextchar = NULL;

/** \brief getopt function. */
int getopt(int argc, char *const argv[], const char *optstring) {
  char c;
  char *cp;

  if (optind == 0) {
    optind = 1;
    nextchar = NULL;
  }

  if (nextchar == NULL || *nextchar == '\0') {
    if (optind >= argc || argv[optind] == NULL || argv[optind][0] != '-' ||
        argv[optind][1] == '\0') {
      return -1;
    }
    if (strcmp(argv[optind], "--") == 0) {
      optind++;
      return -1;
    }
    nextchar = argv[optind] + 1;
  }

  c = *nextchar++;
  cp = strchr(optstring, c);

  if (cp == NULL || c == ':') {
    if (opterr && *optstring != ':') {
      fprintf(stderr, "illegal option -- %c\n", c);
    }
    optopt = c;
    if (*nextchar == '\0')
      optind++;
    return '?';
  }

  if (cp[1] == ':') {
    if (*nextchar != '\0') {
      optarg = nextchar;
      optind++;
    } else {
      if (optind + 1 >= argc) {
        if (opterr && *optstring != ':') {
          fprintf(stderr, "option requires an argument -- %c\n", c);
        }
        optopt = c;
        optind++;
        return (*optstring == ':') ? ':' : '?';
      }
      optarg = argv[++optind];
      optind++;
    }
    nextchar = NULL;
  } else {
    if (*nextchar == '\0') {
      optind++;
    }
    optarg = NULL;
  }

  return c;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getpgid function. */
pid_t getpgid(pid_t pid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getpgrp function. */
pid_t getpgrp(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getppid function. */
pid_t getppid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getsid function. */
pid_t getsid(pid_t pid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getuid function. */
uid_t getuid(void) { return (uid_t)get_current_rid(0); }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief lchown function. */
int lchown(const char *pathname, uid_t owner, gid_t group) {
  unsigned long attr;
  owner = owner;
  group = group;
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
  attr = GetFileAttributesA(pathname);
  if (attr == 0xFFFFFFFFUL) {
    errno = ENOENT;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief link function. */
int link(const char *oldpath, const char *newpath) {
  if (oldpath == NULL || newpath == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (!CreateHardLinkA(newpath, oldpath, NULL)) {
    errno = EACCES;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief linkat function. */
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags) {
  char *oldbuf;
  char *newbuf;
  int res = 0;
  (void)flags; /* AT_SYMLINK_FOLLOW not fully supported */
  if (posix_resolve_at_path(olddirfd, oldpath, &oldbuf) == -1)
    return -1;
  if (posix_resolve_at_path(newdirfd, newpath, &newbuf) == -1) {
    free(oldbuf);
    return -1;
  }

  if (!CreateHardLinkA(newbuf, oldbuf, NULL)) {
    errno = EACCES;
    res = -1;
  }
  free(oldbuf);
  free(newbuf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief lockf function. */
#include <sys/locking.h>
int lockf(int fd, int cmd, off_t len) {
  int mode;
  long nbytes;

  if (len < 0) {
    errno = EINVAL;
    return -1;
  }
  nbytes = len == 0 ? 0x7FFFFFFF : (long)len;

  switch (cmd) {
  case 0:
    mode = _LK_UNLCK;
    break; /* F_ULOCK */
  case 1:
    mode = _LK_LOCK;
    break; /* F_LOCK */
  case 2:
    mode = _LK_NBLCK;
    break; /* F_TLOCK */
  case 3:  /* F_TEST */
    mode = _LK_NBLCK;
    if (_locking(fd, mode, nbytes) == 0) {
      _locking(fd, _LK_UNLCK, nbytes);
      return 0;
    }
    return -1;
  default:
    errno = EINVAL;
    return -1;
  }

  return _locking(fd, mode, nbytes);
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pathconf function. */
long pathconf(const char *pathname, int name) {
  (void)pathname;
  switch (name) {
  case _PC_NAME_MAX:
    return 255;
  case _PC_PATH_MAX:
    return 260;
  case _PC_PIPE_BUF:
    return 4096;
  default:
    errno = EINVAL;
    return -1;
  }
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) unsigned long __stdcall SleepEx(
    unsigned long dwMilliseconds, int bAlertable);

/** \brief pause function. */
int pause(void) {
  SleepEx(0xFFFFFFFFUL, 1); /* Sleep infinitely, alertable to wake on APCs */
  errno = EINTR;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pipe function. */
extern int posix_socketpair(int domain, int type, int protocol,
                            int socket_vector[2]);
int pipe(int pipefd[2]) { return posix_socketpair(2, 1, 0, pipefd); }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pipe2 function. */

int posix_socketpair(int domain, int type, int protocol, int socket_vector[2]);
int pipe2(int pipefd[2], int flags) {
  int textmode = _O_BINARY;
  if (flags & O_CLOEXEC) {
    textmode |= _O_NOINHERIT;
  }
  if (posix_socketpair(2, 1, 0, pipefd) != 0) {
    return -1;
  }
  return 0;
}

#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pread function. */
ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
  local_overlapped ovl = {0};
  unsigned __int64 uoffset;
  intptr_t handle;
  unsigned long bytes_read = 0;
  int result;

  if (fd < 0 || buf == NULL || offset < 0) {
    errno = EINVAL;
    return -1;
  }

  handle = _get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  uoffset = (unsigned __int64)offset;
  ovl.Offset = (unsigned long)(uoffset & 0xFFFFFFFFUL);
  ovl.OffsetHigh = (unsigned long)((uoffset >> 32) & 0xFFFFFFFFUL);

  result = ReadFile((void *)handle, buf, (unsigned long)count, &bytes_read,
                    (void *)&ovl);
  if (!result) {
    unsigned long err = GetLastError();
    if (err == 38) { /* ERROR_HANDLE_EOF */
      return 0;
    }
    errno = EIO;
    return -1;
  }

  return (ssize_t)bytes_read;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief pwrite function. */
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
  local_overlapped ovl = {0};
  unsigned __int64 uoffset;
  intptr_t handle;
  unsigned long bytes_written = 0;
  int result;

  if (fd < 0 || buf == NULL || offset < 0) {
    errno = EINVAL;
    return -1;
  }

  handle = _get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  uoffset = (unsigned __int64)offset;
  ovl.Offset = (unsigned long)(uoffset & 0xFFFFFFFFUL);
  ovl.OffsetHigh = (unsigned long)((uoffset >> 32) & 0xFFFFFFFFUL);

  result = WriteFile((void *)handle, buf, (unsigned long)count, &bytes_written,
                     (void *)&ovl);
  if (!result) {
    errno = EIO;
    return -1;
  }

  return (ssize_t)bytes_written;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)

#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) int __stdcall DeviceIoControl(
    void *hDevice, unsigned long dwIoControlCode, void *lpInBuffer,
    unsigned long nInBufferSize, void *lpOutBuffer,
    unsigned long nOutBufferSize, unsigned long *lpBytesReturned,
    void *lpOverlapped);

__declspec(dllimport) int __stdcall WideCharToMultiByte(
    unsigned int CodePage, unsigned long dwFlags, const wchar_t *lpWideCharStr,
    int cchWideChar, char *lpMultiByteStr, int cbMultiByte,
    const char *lpDefaultChar, int *lpUsedDefaultChar);
#endif

typedef struct _POSIX_REPARSE_DATA_BUFFER {

  unsigned long ReparseTag;
  unsigned short ReparseDataLength;
  unsigned short Reserved;
  union {
    struct {
      unsigned short SubstituteNameOffset;
      unsigned short SubstituteNameLength;
      unsigned short PrintNameOffset;
      unsigned short PrintNameLength;
      unsigned long Flags;
      wchar_t PathBuffer[1];
    } SymbolicLinkReparseBuffer;
    struct {
      unsigned short SubstituteNameOffset;
      unsigned short SubstituteNameLength;
      unsigned short PrintNameOffset;
      unsigned short PrintNameLength;
      wchar_t PathBuffer[1];
    } MountPointReparseBuffer;
    struct {
      unsigned char DataBuffer[1];
    } GenericReparseBuffer;
  } u;
} POSIX_REPARSE_DATA_BUFFER;

/** rief readlink function. */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
  void *hFile;
  unsigned long attrs;
  unsigned long bytesReturned;
  char reparse_buffer[16384];
  POSIX_REPARSE_DATA_BUFFER *pReparse =
      (POSIX_REPARSE_DATA_BUFFER *)reparse_buffer;
  int target_len;
  int req_len;
  int wc_res;
  wchar_t *printName;

  if (!pathname || !buf || bufsiz == 0) {
    errno = EINVAL;
    return -1;
  }

  attrs = GetFileAttributesA(pathname);
  if (attrs == 0xFFFFFFFF || !(attrs & 0x400)) {
    errno = EINVAL;
    return -1;
  }

  hFile = CreateFileA(pathname, 0, 1 | 2 | 4, NULL, 3, 0x02200000, NULL);
  if (hFile == (void *)(intptr_t)-1) {
    errno = ENOENT;
    return -1;
  }

  if (!DeviceIoControl(hFile, 589988, NULL, 0, reparse_buffer,
                       sizeof(reparse_buffer), &bytesReturned, NULL)) {
    CloseHandle(hFile);
    errno = EINVAL;
    return -1;
  }
  CloseHandle(hFile);

  if (pReparse->ReparseTag == 0xA000000C) {
    printName =
        (wchar_t *)((unsigned char *)
                        pReparse->u.SymbolicLinkReparseBuffer.PathBuffer +
                    pReparse->u.SymbolicLinkReparseBuffer.PrintNameOffset);
    target_len =
        pReparse->u.SymbolicLinkReparseBuffer.PrintNameLength / sizeof(wchar_t);
  } else if (pReparse->ReparseTag == 0xA0000003) {
    printName =
        (wchar_t *)((unsigned char *)
                        pReparse->u.MountPointReparseBuffer.PathBuffer +
                    pReparse->u.MountPointReparseBuffer.PrintNameOffset);
    target_len =
        pReparse->u.MountPointReparseBuffer.PrintNameLength / sizeof(wchar_t);
  } else {
    errno = EINVAL;
    return -1;
  }

  req_len = WideCharToMultiByte(0 /* CP_ACP */, 0, printName, target_len, NULL,
                                0, NULL, NULL);
  if (req_len == 0) {
    errno = EINVAL;
    return -1;
  }
  if (req_len > (int)bufsiz) {
    char *temp = (char *)malloc(req_len);
    if (!temp) {
      errno = ENOMEM;
      return -1;
    }
    wc_res = WideCharToMultiByte(0, 0, printName, target_len, temp, req_len,
                                 NULL, NULL);
    if (wc_res == 0) {
      free(temp);
      errno = EINVAL;
      return -1;
    }
    memcpy(buf, temp, bufsiz);
    free(temp);
    return (ssize_t)bufsiz;
  } else {
    wc_res = WideCharToMultiByte(0, 0, printName, target_len, buf, (int)bufsiz,
                                 NULL, NULL);
    return (ssize_t)wc_res;
  }
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief readlinkat function. */
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) {
  char *fullbuf;
  ssize_t res;
  if (posix_resolve_at_path(dirfd, pathname, &fullbuf) == -1)
    return -1;
  res = readlink(fullbuf, buf, bufsiz);
  free(fullbuf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setegid function. */
int setegid(gid_t egid) {
  if (egid == (gid_t)get_current_rid(1))
    return 0;
  errno = EPERM;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief seteuid function. */
int seteuid(uid_t euid) {
  if (euid == (uid_t)get_current_rid(0))
    return 0;
  errno = EPERM;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setgid function. */
int setgid(gid_t gid) {
  if (gid == (gid_t)get_current_rid(1))
    return 0;
  errno = EPERM;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setpgid function. */
int setpgid(pid_t pid, pid_t pgid) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setpgrp function. */
pid_t setpgrp(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setregid function. */
int setregid(gid_t rgid, gid_t egid) {
  gid_t current = (gid_t)get_current_rid(1);
  if ((rgid == (gid_t)-1 || rgid == current) &&
      (egid == (gid_t)-1 || egid == current)) {
    return 0;
  }
  errno = EPERM;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setreuid function. */
int setreuid(uid_t ruid, uid_t euid) {
  uid_t current = (uid_t)get_current_rid(0);
  if ((ruid == (uid_t)-1 || ruid == current) &&
      (euid == (uid_t)-1 || euid == current)) {
    return 0;
  }
  errno = EPERM;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setsid function. */
pid_t setsid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief setuid function. */
int setuid(uid_t uid) {
  if (uid == (uid_t)get_current_rid(0))
    return 0;
  errno = EPERM;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** rief symlink function. */
int symlink(const char *target, const char *linkpath) {
  void *kernel32;
  CreateSymbolicLinkA_Func pCreateSymbolicLinkA;
  unsigned long attrs;
  unsigned long flags = 0;

  if (!target || !linkpath) {
    errno = EINVAL;
    return -1;
  }
  kernel32 = GetModuleHandleA("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return -1;
  }
  pCreateSymbolicLinkA = (CreateSymbolicLinkA_Func)(intptr_t)GetProcAddress(
      kernel32, "CreateSymbolicLinkA");
  if (!pCreateSymbolicLinkA) {
    errno = ENOSYS;
    return -1;
  }

  attrs = GetFileAttributesA(target);
  if (attrs != 0xFFFFFFFF && (attrs & 0x10)) { /* FILE_ATTRIBUTE_DIRECTORY */
    flags |= 0x1; /* SYMBOLIC_LINK_FLAG_DIRECTORY */
  }
  flags |= 0x2; /* SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE */

  if (!pCreateSymbolicLinkA(linkpath, target, flags)) {
    if (GetLastError() == 87 /* ERROR_INVALID_PARAMETER */) {
      if (!pCreateSymbolicLinkA(linkpath, target, flags & ~0x2)) {
        errno = EACCES;
        return -1;
      }
    } else {
      errno = EACCES;
      return -1;
    }
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief symlinkat function. */
int symlinkat(const char *target, int newdirfd, const char *linkpath) {
  char *newbuf;
  int res;
  if (posix_resolve_at_path(newdirfd, linkpath, &newbuf) == -1)
    return -1;
  res = symlink(target, newbuf);
  free(newbuf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) unsigned long __stdcall GetLogicalDrives(void);
__declspec(dllimport) int __stdcall FlushFileBuffers(void *hFile);
__declspec(dllimport) unsigned int __stdcall GetDriveTypeA(
    const char *lpRootPathName);

/** \brief sync function. */
void sync(void) {
  unsigned long drives = GetLogicalDrives();
  int i;
  for (i = 0; i < 26; i++) {
    if (drives & (1 << i)) {
      char driveName[] = "\\\\.\\A:";
      char rootName[] = "A:\\";
      void *hFile;

      driveName[4] = (char)('A' + i);
      rootName[0] = (char)('A' + i);

      if (GetDriveTypeA(rootName) == 3) { /* DRIVE_FIXED */
        hFile = CreateFileA(driveName, 0x40000000 /* GENERIC_WRITE */,
                            1 | 2 /* FILE_SHARE_READ | FILE_SHARE_WRITE */,
                            NULL, 3 /* OPEN_EXISTING */, 0, NULL);
        if (hFile != (void *)(intptr_t)-1) {
          FlushFileBuffers(hFile);
          CloseHandle(hFile);
        }
      }
    }
  }
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/**
 * @brief Get system configuration values at runtime.
 *
 * Provides system configuration variables such as page size and number of
 * processors.
 *
 * @param name The system variable to query (e.g., _SC_PAGESIZE).
 * @return The value of the requested configuration variable, or -1 if
 * unsupported or an error occurred.
 */
long sysconf(int name) {
  switch (name) {
  case _SC_PAGESIZE:
  case _SC_NPROCESSORS_ONLN:
  case _SC_NPROCESSORS_CONF: {
    POSIX_SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (name == _SC_PAGESIZE) {
      return (long)si.dwPageSize;
    }
    return (long)si.dwNumberOfProcessors;
  }
  case _SC_PHYS_PAGES:
  case _SC_AVPHYS_PAGES: {
    POSIX_MEMORYSTATUSEX statex;
    POSIX_SYSTEM_INFO si;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
      GetSystemInfo(&si);
      if (si.dwPageSize == 0)
        return -1;
      if (name == _SC_PHYS_PAGES) {
        return (long)(statex.ullTotalPhys / si.dwPageSize);
      } else {
        return (long)(statex.ullAvailPhys / si.dwPageSize);
      }
    }
    return -1;
  }
  case _SC_CLK_TCK:
    return 1000;
  default:
    errno = EINVAL;
    return -1;
  }
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief tcgetpgrp function. */
pid_t tcgetpgrp(int fd) {
  if (_isatty(fd))
    return 0;
  errno = ENOTTY;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief tcsetpgrp function. */
int tcsetpgrp(int fd, pid_t pgrp) {
  pgrp = pgrp;
  if (!_isatty(fd)) {
    errno = ENOTTY;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief truncate function. */
int truncate(const char *path, off_t length) {
  int fd;
  int res;
  if (!path || length < 0) {
    errno = EINVAL;
    return -1;
  }
  fd = _open(path, _O_RDWR);
  if (fd == -1) {
    return -1;
  }
  res = _chsize(fd, (long)length);
  _close(fd);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
char *ttyname(int fd) {
  if (_isatty(fd)) {
    return "CON";
  }
  errno = ENOTTY;
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief ttyname_r function. */
int ttyname_r(int fd, char *buf, size_t buflen) {
  if (buf == NULL) {
    return EINVAL;
  }
  if (_isatty(fd)) {
    if (buflen < 4) {
      return ERANGE;
    }
    strcpy(buf, "CON");
    return 0;
  }
  return ENOTTY;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
static void *g_ualarm_timer = NULL;
static unsigned long g_ualarm_remaining_ms = 0;
static unsigned long g_ualarm_start_ticks = 0;

static void __stdcall ualarm_timer_callback(void *lpParameter,
                                            unsigned char TimerOrWaitFired) {
  (void)lpParameter;
  (void)TimerOrWaitFired;
  /* SIGALRM = 14 */
  raise(14);
}

/** \brief ualarm function. */
useconds_t ualarm(useconds_t value, useconds_t interval) {
  useconds_t remaining = 0;
  if (g_ualarm_timer) {
    unsigned long elapsed = GetTickCount() - g_ualarm_start_ticks;
    if (elapsed < g_ualarm_remaining_ms) {
      remaining = (useconds_t)((g_ualarm_remaining_ms - elapsed) * 1000);
    }
    DeleteTimerQueueTimer(NULL, g_ualarm_timer, NULL);
    g_ualarm_timer = NULL;
  }

  if (value > 0) {
    unsigned long dueTime = (unsigned long)((value + 999) / 1000);
    unsigned long period = (unsigned long)((interval + 999) / 1000);
    g_ualarm_start_ticks = GetTickCount();
    g_ualarm_remaining_ms = dueTime;
    CreateTimerQueueTimer(&g_ualarm_timer, NULL, ualarm_timer_callback, NULL,
                          dueTime, period, 0x00000000);
  }

  return remaining;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief vfork function. */
pid_t vfork(void) { return fork(); }
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) int __stdcall MoveFileExA(const char *lpExistingFileName,
                                                const char *lpNewFileName,
                                                unsigned long dwFlags);
__declspec(dllimport) int __stdcall CopyFileA(const char *lpExistingFileName,
                                              const char *lpNewFileName,
                                              int bFailIfExists);

/** \brief rename function (POSIX semantics). */
int posix_rename(const char *oldpath, const char *newpath) {
  if (MoveFileExA(oldpath, newpath, 0x00000001 /* MOVEFILE_REPLACE_EXISTING */))
    return 0;

  if (CopyFileA(oldpath, newpath, 0)) {
    _unlink(oldpath);
    return 0;
  }

  _chmod(newpath, 00200 /* _S_IWRITE */);
  _unlink(newpath);
  if (CopyFileA(oldpath, newpath, 0)) {
    _unlink(oldpath);
    return 0;
  }

  errno = EACCES;
  return -1;
}

/** \brief mkstemp function (POSIX semantics with SHARE_DELETE). */
int posix_mkstemp(char *tmpl) {
  void *hFile;
  int fd;
  if (_mktemp_s(tmpl, strlen(tmpl) + 1) != 0)
    return -1;

  hFile = CreateFileA(
      tmpl, 0x80000000L | 0x40000000L /* GENERIC_READ | GENERIC_WRITE */,
      0x00000001 | 0x00000002 | 0x00000004 /* FILE_SHARE_READ | FILE_SHARE_WRITE
                                              | FILE_SHARE_DELETE */
      ,
      NULL, 2 /* CREATE_ALWAYS */, 0x00000080 /* FILE_ATTRIBUTE_NORMAL */,
      NULL);
  if (hFile == (void *)(intptr_t)-1 /* INVALID_HANDLE_VALUE */)
    return -1;

  fd = _open_osfhandle((intptr_t)hFile, _O_RDWR);
  if (fd == -1) {
    CloseHandle(hFile);
  }
  return fd;
}
#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_core;
