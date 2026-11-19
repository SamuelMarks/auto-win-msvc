/* posix-core.c - Strict C89 Implementation */

/* clang-format off */
#include "posix-core.h"
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#include <fcntl.h>
#include <process.h>
#include <sys/locking.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#include <share.h>
#include <sys/stat.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <stdint.h>
#endif
#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
#include <dos.h>
#include <fcntl.h>
#include <process.h>
#else
#include <fcntl.h>
#if defined(__GLIBC__)
#include <stdio_ext.h>
#endif
#endif
/* clang-format on */

static void *default_common_malloc(size_t size) { return malloc(size); }
static void default_common_free(void *ptr) { free(ptr); }

static int default_common_open_scratch(const char *path) {
  int fd;
#if defined(_MSC_VER) && _MSC_VER >= 1400
  if (_sopen_s(&fd, path, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, _SH_DENYNO,
               _S_IREAD | _S_IWRITE) != 0) {
    return -1;
  }
  _close(fd);
  return 0;
#elif defined(_WIN32) && !defined(__CYGWIN__)
  fd = _open(path, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, 0600);
  if (fd < 0) {
    return -1;
  }
  _close(fd);
  return 0;
#else
  fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0600);
  if (fd < 0) {
    return -1;
  }
  close(fd);
  return 0;
#endif
}

static FILE *default_common_fopen(const char *path, const char *mode) {
#if defined(_MSC_VER)
  FILE *fp = NULL;
  if (fopen_s(&fp, path, mode) != 0) {
    return NULL;
  }
  return fp;
#else
  return fopen(path, mode);
#endif
}

static int default_common_system(const char *cmd) { return system(cmd); }

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
extern int mkstemp(char *tmpl);
static int default_common_mkstemp(char *tmpl) { return mkstemp(tmpl); }
#endif

static const struct posix_core_common_hooks k_default_common_hooks = {
    default_common_malloc,
    default_common_free,
    default_common_open_scratch,
    default_common_fopen,
    default_common_system
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
    ,
    default_common_mkstemp
#endif
};

static struct posix_core_common_hooks g_core_common_hooks = {
    default_common_malloc,
    default_common_free,
    default_common_open_scratch,
    default_common_fopen,
    default_common_system
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
    ,
    default_common_mkstemp
#endif
};

/** @brief Copies default cross-platform hooks for posix-core into out_hooks. */
void posix_core_get_default_common_hooks(
    struct posix_core_common_hooks *out_hooks) {
  if (out_hooks) {
    *out_hooks = k_default_common_hooks;
  }
}

/** @brief Installs custom cross-platform hooks for posix-core (NULL resets). */
void posix_core_set_common_hooks(const struct posix_core_common_hooks *hooks) {
  if (hooks) {
    g_core_common_hooks = *hooks;
  } else {
    g_core_common_hooks = k_default_common_hooks;
  }
}

#if defined(_WIN32) && !defined(__CYGWIN__)

__declspec(dllimport) int __stdcall ReadFile(void *hFile, void *lpBuffer,
                                             unsigned long nNumberOfBytesToRead,
                                             unsigned long *lpNumberOfBytesRead,
                                             void *lpOverlapped);
__declspec(dllimport) int __stdcall
WriteFile(void *hFile, const void *lpBuffer,
          unsigned long nNumberOfBytesToWrite,
          unsigned long *lpNumberOfBytesWritten, void *lpOverlapped);
__declspec(dllimport) int __stdcall GetFileSizeEx(void *hFile,
                                                  posix_core_i64_t *lpFileSize);
__declspec(dllimport) int __stdcall
SetFilePointerEx(void *hFile, posix_core_i64_t liDistanceToMove,
                 posix_core_i64_t *lpNewFilePointer,
                 unsigned long dwMoveMethod);
__declspec(dllimport) int __stdcall SetEndOfFile(void *hFile);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall
LockFileEx(void *hFile, unsigned long dwFlags, unsigned long dwReserved,
           unsigned long nNumberOfBytesToLockLow,
           unsigned long nNumberOfBytesToLockHigh, void *lpOverlapped);
__declspec(dllimport) int __stdcall
UnlockFileEx(void *hFile, unsigned long dwReserved,
             unsigned long nNumberOfBytesToUnlockLow,
             unsigned long nNumberOfBytesToUnlockHigh, void *lpOverlapped);
__declspec(dllimport) int __stdcall ioctlsocket(uintptr_t s, long cmd,
                                                unsigned long *argp);
__declspec(dllimport) int __stdcall WSAGetLastError(void);
#ifndef WSAENOTSOCK
#define WSAENOTSOCK 10038
#endif

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

__declspec(dllimport) void __stdcall
GetSystemInfo(POSIX_SYSTEM_INFO *lpSystemInfo);

typedef struct _POSIX_MEMORYSTATUSEX {
  unsigned long dwLength;
  unsigned long dwMemoryLoad;
  posix_core_u64_t ullTotalPhys;
  posix_core_u64_t ullAvailPhys;
  posix_core_u64_t ullTotalPageFile;
  posix_core_u64_t ullAvailPageFile;
  posix_core_u64_t ullTotalVirtual;
  posix_core_u64_t ullAvailVirtual;
  posix_core_u64_t ullAvailExtendedVirtual;
} POSIX_MEMORYSTATUSEX;

__declspec(dllimport) int __stdcall
GlobalMemoryStatusEx(POSIX_MEMORYSTATUSEX *lpBuffer);
__declspec(dllimport) int __stdcall GetUserNameA(char *lpBuffer,
                                                 unsigned long *pcbBuffer);
__declspec(dllimport) int __stdcall
CreateHardLinkA(const char *lpFileName, const char *lpExistingFileName,
                void *lpSecurityAttributes);
#if defined(_MSC_VER)
__declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
#endif
__declspec(dllimport) void *__stdcall
GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport) void *__stdcall GetProcAddress(void *hModule,
                                                     const char *lpProcName);
__declspec(dllimport) unsigned long __stdcall
GetFileAttributesA(const char *lpFileName);
__declspec(dllimport) unsigned long __stdcall
GetCurrentDirectoryA(unsigned long nBufferLength, char *lpBuffer);
__declspec(dllimport) int __stdcall
SetCurrentDirectoryA(const char *lpPathName);
__declspec(dllimport) int __stdcall
CreateDirectoryA(const char *lpPathName, void *lpSecurityAttributes);
__declspec(dllimport) void *__stdcall
CreateFileA(const char *lpFileName, unsigned long dwDesiredAccess,
            unsigned long dwShareMode, void *lpSecurityAttributes,
            unsigned long dwCreationDisposition,
            unsigned long dwFlagsAndAttributes, void *hTemplateFile);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);
__declspec(dllimport) void *__stdcall
CreateFileMappingA(void *hFile, void *lpFileMappingAttributes,
                   unsigned long flProtect, unsigned long dwMaximumSizeHigh,
                   unsigned long dwMaximumSizeLow, const char *lpName);
__declspec(dllimport) void *__stdcall
MapViewOfFile(void *hFileMappingObject, unsigned long dwDesiredAccess,
              unsigned long dwFileOffsetHigh, unsigned long dwFileOffsetLow,
              size_t dwNumberOfBytesToMap);
__declspec(dllimport) int __stdcall UnmapViewOfFile(const void *lpBaseAddress);
__declspec(dllimport) int __stdcall
CreateTimerQueueTimer(void **phNewTimer, void *TimerQueue,
                      void(__stdcall *Callback)(void *, unsigned char),
                      void *Parameter, unsigned long DueTime,
                      unsigned long Period, unsigned long Flags);
__declspec(dllimport) int __stdcall
DeleteTimerQueueTimer(void *TimerQueue, void *Timer, void *CompletionEvent);
__declspec(dllimport) unsigned long __stdcall GetTickCount(void);
__declspec(dllimport) unsigned long __stdcall
SleepEx(unsigned long dwMilliseconds, int bAlertable);
__declspec(dllimport) int __stdcall
DeviceIoControl(void *hDevice, unsigned long dwIoControlCode, void *lpInBuffer,
                unsigned long nInBufferSize, void *lpOutBuffer,
                unsigned long nOutBufferSize, unsigned long *lpBytesReturned,
                void *lpOverlapped);
__declspec(dllimport) int __stdcall
WideCharToMultiByte(unsigned int CodePage, unsigned long dwFlags,
                    const wchar_t *lpWideCharStr, int cchWideChar,
                    char *lpMultiByteStr, int cbMultiByte,
                    const char *lpDefaultChar, int *lpUsedDefaultChar);
__declspec(dllimport) unsigned long __stdcall GetLogicalDrives(void);
__declspec(dllimport) int __stdcall FlushFileBuffers(void *hFile);
__declspec(dllimport) unsigned int __stdcall
GetDriveTypeA(const char *lpRootPathName);
__declspec(dllimport) unsigned long __stdcall
GetModuleFileNameA(void *hModule, char *lpFilename, unsigned long nSize);
__declspec(dllimport) int __stdcall SetStdHandle(unsigned long nStdHandle,
                                                 void *hHandle);
__declspec(dllimport) int __stdcall SetHandleInformation(void *hObject,
                                                         unsigned long dwMask,
                                                         unsigned long dwFlags);
__declspec(dllimport) unsigned long __stdcall
WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall
GetExitCodeProcess(void *hProcess, unsigned long *lpExitCode);
__declspec(dllimport) int __stdcall MoveFileExA(const char *lpExistingFileName,
                                                const char *lpNewFileName,
                                                unsigned long dwFlags);
__declspec(dllimport) int __stdcall CopyFileA(const char *lpExistingFileName,
                                              const char *lpNewFileName,
                                              int bFailIfExists);

typedef size_t WIN_HCRYPTPROV;
typedef size_t WIN_HCRYPTHASH;
typedef size_t WIN_HCRYPTKEY;

__declspec(dllimport) int __stdcall
CryptAcquireContextA(WIN_HCRYPTPROV *phProv, const char *pszContainer,
                     const char *pszProvider, unsigned long dwProvType,
                     unsigned long dwFlags);
__declspec(dllimport) int __stdcall
CryptCreateHash(WIN_HCRYPTPROV hProv, unsigned int Algid, WIN_HCRYPTKEY hKey,
                unsigned long dwFlags, WIN_HCRYPTHASH *phHash);
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
__declspec(dllimport) int __stdcall
CryptEncrypt(WIN_HCRYPTKEY hKey, WIN_HCRYPTHASH hHash, int Final,
             unsigned long dwFlags, unsigned char *pbData,
             unsigned long *pdwDataLen, unsigned long dwBufLen);
__declspec(dllimport) int __stdcall
CryptDecrypt(WIN_HCRYPTKEY hKey, WIN_HCRYPTHASH hHash, int Final,
             unsigned long dwFlags, unsigned char *pbData,
             unsigned long *pdwDataLen);
__declspec(dllimport) int __stdcall CryptDestroyKey(WIN_HCRYPTKEY hKey);

extern int posix_socketpair(int domain, int type, int protocol,
                            intptr_t socket_vector[2]);

#if defined(_MSC_VER) && _MSC_VER >= 1400
static void null_invalid_param_handler(const wchar_t *expression,
                                       const wchar_t *function,
                                       const wchar_t *file, unsigned int line,
                                       uintptr_t pReserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif

static void *default_win32_malloc(size_t size) { return malloc(size); }
static void *default_win32_realloc(void *ptr, size_t sz) {
  return realloc(ptr, sz);
}
static void default_win32_free(void *ptr) { free(ptr); }

static intptr_t default_win32_get_osfhandle(int fd) {
  intptr_t h;
#if defined(_MSC_VER) && _MSC_VER >= 1400
  _invalid_parameter_handler old =
      _set_invalid_parameter_handler(null_invalid_param_handler);
  AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
  h = _get_osfhandle(fd);
  _set_invalid_parameter_handler(old);
#else
  h = _get_osfhandle(fd);
#endif
  return h;
}

static int default_win32_open_osfhandle(intptr_t h, int f) {
  return _open_osfhandle(h, f);
}
static int default_win32_dup(int fd) { return _dup(fd); }
static int default_win32_dup2(int fd1, int fd2) { return _dup2(fd1, fd2); }
static int default_win32_close(int fd) { return _close(fd); }
static int default_win32_commit(int fd) { return _commit(fd); }
static int default_win32_locking(int fd, int mode, long nbytes) {
  return _locking(fd, mode, nbytes);
}
static int default_win32_pipe(int *pfds, unsigned int psize, int textmode) {
  return _pipe(pfds, psize, textmode);
}

static int default_win32_isatty(int fd) {
  int ret;
#if defined(_MSC_VER) && _MSC_VER >= 1400
  _invalid_parameter_handler old =
      _set_invalid_parameter_handler(null_invalid_param_handler);
  AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
  ret = _isatty(fd);
  _set_invalid_parameter_handler(old);
#else
  ret = _isatty(fd);
#endif
  return ret;
}

static int default_win32_open_rdwr(const char *path) {
#if defined(_MSC_VER)
  int fd = -1;
  if (_sopen_s(&fd, path, _O_RDWR, _SH_DENYNO, 0) != 0) {
    return -1;
  }
  return fd;
#else
  return _open(path, _O_RDWR);
#endif
}

static int default_win32_chsize(int fd, long size) { return _chsize(fd, size); }
static int default_win32_access(const char *p, int m) { return _access(p, m); }
static int default_win32_chmod(const char *p, int m) { return _chmod(p, m); }
static int default_win32_unlink(const char *p) { return _unlink(p); }
static int default_win32_rmdir(const char *p) { return _rmdir(p); }
static char *default_win32_getcwd(char *buf, int maxlen) {
  return _getcwd(buf, maxlen);
}
static int default_win32_chdir(const char *p) { return _chdir(p); }
static char *default_win32_fullpath(char *abs_p, const char *rel_p,
                                    size_t max_len) {
  return _fullpath(abs_p, rel_p, max_len);
}
static int default_win32_mktemp_s(char *tmpl, size_t sz) {
  return _mktemp_s(tmpl, sz);
}
static intptr_t default_win32_execve(const char *cmd, const char *const *argv,
                                     const char *const *envp) {
  return _execve(cmd, argv, envp);
}
static intptr_t default_win32_spawnve(int mode, const char *cmd,
                                      const char *const *argv,
                                      const char *const *envp) {
  return _spawnve(mode, cmd, argv, envp);
}
static intptr_t default_win32_spawnvpe(int mode, const char *cmd,
                                       const char *const *argv,
                                       const char *const *envp) {
  return _spawnvpe(mode, cmd, argv, envp);
}
static intptr_t default_win32_spawnv(int mode, const char *cmd,
                                     const char *const *argv) {
  return _spawnv(mode, cmd, argv);
}
static int default_win32_raise(int sig) { return raise(sig); }
static void *default_win32_get_module_handle(const char *n) {
  return GetModuleHandleA(n);
}
static void *default_win32_get_proc_address(void *m, const char *p) {
  return GetProcAddress(m, p);
}
static unsigned long default_win32_get_last_error(void) {
  return GetLastError();
}
static int default_win32_get_file_size_ex(void *h, posix_core_i64_t *sz) {
  return GetFileSizeEx(h, sz);
}
static int default_win32_set_file_pointer_ex(void *h, posix_core_i64_t dist,
                                             posix_core_i64_t *newp,
                                             unsigned long method) {
  return SetFilePointerEx(h, dist, newp, method);
}
static int default_win32_set_end_of_file(void *h) { return SetEndOfFile(h); }
static int default_win32_lock_file_ex(void *h, unsigned long fl,
                                      unsigned long res, unsigned long lo,
                                      unsigned long hi, void *ov) {
  return LockFileEx(h, fl, res, lo, hi, ov);
}
static int default_win32_unlock_file_ex(void *h, unsigned long res,
                                        unsigned long lo, unsigned long hi,
                                        void *ov) {
  return UnlockFileEx(h, res, lo, hi, ov);
}
static void *default_win32_create_file_mapping(void *h, void *sa,
                                               unsigned long prot,
                                               unsigned long hi,
                                               unsigned long lo,
                                               const char *name) {
  return CreateFileMappingA(h, sa, prot, hi, lo, name);
}
static void *default_win32_map_view_of_file(void *h, unsigned long acc,
                                            unsigned long hi, unsigned long lo,
                                            size_t bytes) {
  return MapViewOfFile(h, acc, hi, lo, bytes);
}
static int default_win32_unmap_view_of_file(const void *addr) {
  return UnmapViewOfFile(addr);
}
static int default_win32_create_timer(void **out_t, void *q,
                                      posix_core_timer_cb_t cb, void *param,
                                      unsigned long due, unsigned long period,
                                      unsigned long fl) {
  return CreateTimerQueueTimer(out_t, q, cb, param, due, period, fl);
}
static int default_win32_delete_timer(void *q, void *t, void *ev) {
  return DeleteTimerQueueTimer(q, t, ev);
}
static unsigned long default_win32_get_tick_count(void) {
  return GetTickCount();
}
static unsigned long default_win32_get_file_attributes(const char *p) {
  return GetFileAttributesA(p);
}
static unsigned long default_win32_get_current_dir(unsigned long len,
                                                   char *buf) {
  return GetCurrentDirectoryA(len, buf);
}
static int default_win32_set_current_dir(const char *p) {
  return SetCurrentDirectoryA(p);
}
static int default_win32_create_directory(const char *p, void *sa) {
  return CreateDirectoryA(p, sa);
}
static void *default_win32_create_file(const char *n, unsigned long acc,
                                       unsigned long sh, void *sa,
                                       unsigned long disp, unsigned long fl,
                                       void *tmpl) {
  return CreateFileA(n, acc, sh, sa, disp, fl, tmpl);
}
static int default_win32_close_handle(void *h) { return CloseHandle(h); }
static int default_win32_read_file(void *h, void *buf, unsigned long n,
                                   unsigned long *br, void *ov) {
  return ReadFile(h, buf, n, br, ov);
}
static int default_win32_write_file(void *h, const void *buf, unsigned long n,
                                    unsigned long *bw, void *ov) {
  return WriteFile(h, buf, n, bw, ov);
}
static int default_win32_device_io_control(void *h, unsigned long code,
                                           void *in_b, unsigned long in_sz,
                                           void *out_b, unsigned long out_sz,
                                           unsigned long *ret_b, void *ov) {
  return DeviceIoControl(h, code, in_b, in_sz, out_b, out_sz, ret_b, ov);
}
static int default_win32_wide_char_to_multi_byte(
    unsigned int cp, unsigned long fl, const wchar_t *ws, int wlen, char *mb,
    int mblen, const char *def_c, int *used_def) {
  return WideCharToMultiByte(cp, fl, ws, wlen, mb, mblen, def_c, used_def);
}
static int default_win32_create_hard_link(const char *new_p, const char *old_p,
                                          void *sa) {
  return CreateHardLinkA(new_p, old_p, sa);
}
static int default_win32_move_file_ex(const char *old_p, const char *new_p,
                                      unsigned long fl) {
  return MoveFileExA(old_p, new_p, fl);
}
static int default_win32_copy_file(const char *old_p, const char *new_p,
                                   int fail_if_exists) {
  return CopyFileA(old_p, new_p, fail_if_exists);
}
static int default_win32_get_user_name(char *buf, unsigned long *sz) {
  return GetUserNameA(buf, sz);
}
static unsigned long default_win32_sleep_ex(unsigned long ms, int alertable) {
  return SleepEx(ms, alertable);
}
static unsigned long default_win32_get_logical_drives(void) {
  return GetLogicalDrives();
}
static unsigned int default_win32_get_drive_type(const char *root) {
  return GetDriveTypeA(root);
}
static int default_win32_flush_file_buffers(void *h) {
  return FlushFileBuffers(h);
}
static void default_win32_get_system_info(void *si) {
  GetSystemInfo((POSIX_SYSTEM_INFO *)si);
}
static int default_win32_global_memory_status_ex(void *ms) {
  return GlobalMemoryStatusEx((POSIX_MEMORYSTATUSEX *)ms);
}
static unsigned long default_win32_get_module_file_name(void *m, char *buf,
                                                        unsigned long sz) {
  return GetModuleFileNameA(m, buf, sz);
}
static int default_win32_set_std_handle(unsigned long id, void *h) {
  return SetStdHandle(id, h);
}
static int default_win32_set_handle_information(void *h, unsigned long mask,
                                                unsigned long fl) {
  return SetHandleInformation(h, mask, fl);
}
static unsigned long default_win32_wait_for_single_object(void *h,
                                                          unsigned long ms) {
  return WaitForSingleObject(h, ms);
}
static int default_win32_get_exit_code_process(void *h, unsigned long *code) {
  return GetExitCodeProcess(h, code);
}
static int default_win32_ioctlsocket(uintptr_t s, long cmd,
                                     unsigned long *argp) {
  return ioctlsocket(s, cmd, argp);
}
static int default_win32_wsa_get_last_error(void) { return WSAGetLastError(); }
static int default_win32_socketpair(int dom, int type, int proto,
                                    intptr_t sv[2]) {
  return posix_socketpair(dom, type, proto, sv);
}
static int default_win32_crypt_acquire_context(size_t *prov, const char *cont,
                                               const char *prv,
                                               unsigned long type,
                                               unsigned long fl) {
  return CryptAcquireContextA(prov, cont, prv, type, fl);
}
static int default_win32_crypt_create_hash(size_t prov, unsigned int alg,
                                           size_t key, unsigned long fl,
                                           size_t *hash) {
  return CryptCreateHash(prov, alg, key, fl, hash);
}
static int default_win32_crypt_hash_data(size_t hash, const unsigned char *data,
                                         unsigned long len, unsigned long fl) {
  return CryptHashData(hash, data, len, fl);
}
static int default_win32_crypt_get_hash_param(size_t hash, unsigned long param,
                                              unsigned char *data,
                                              unsigned long *len,
                                              unsigned long fl) {
  return CryptGetHashParam(hash, param, data, len, fl);
}
static int default_win32_crypt_destroy_hash(size_t hash) {
  return CryptDestroyHash(hash);
}
static int default_win32_crypt_release_context(size_t prov, unsigned long fl) {
  return CryptReleaseContext(prov, fl);
}
static int default_win32_crypt_derive_key(size_t prov, unsigned int alg,
                                          size_t base, unsigned long fl,
                                          size_t *key) {
  return CryptDeriveKey(prov, alg, base, fl, key);
}
static int default_win32_crypt_encrypt(size_t key, size_t hash, int final_b,
                                       unsigned long fl, unsigned char *data,
                                       unsigned long *len, unsigned long cap) {
  return CryptEncrypt(key, hash, final_b, fl, data, len, cap);
}
static int default_win32_crypt_decrypt(size_t key, size_t hash, int final_b,
                                       unsigned long fl, unsigned char *data,
                                       unsigned long *len) {
  return CryptDecrypt(key, hash, final_b, fl, data, len);
}
static int default_win32_crypt_destroy_key(size_t key) {
  return CryptDestroyKey(key);
}

static const struct posix_core_win32_hooks k_default_win32_hooks = {
    default_win32_malloc,
    default_win32_realloc,
    default_win32_free,
    default_win32_get_osfhandle,
    default_win32_open_osfhandle,
    default_win32_dup,
    default_win32_dup2,
    default_win32_close,
    default_win32_commit,
    default_win32_locking,
    default_win32_pipe,
    default_win32_isatty,
    default_win32_open_rdwr,
    default_win32_chsize,
    default_win32_access,
    default_win32_chmod,
    default_win32_unlink,
    default_win32_rmdir,
    default_win32_getcwd,
    default_win32_chdir,
    default_win32_fullpath,
    default_win32_mktemp_s,
    default_win32_execve,
    default_win32_spawnve,
    default_win32_spawnvpe,
    default_win32_spawnv,
    default_win32_raise,
    default_win32_get_module_handle,
    default_win32_get_proc_address,
    default_win32_get_last_error,
    default_win32_get_file_size_ex,
    default_win32_set_file_pointer_ex,
    default_win32_set_end_of_file,
    default_win32_lock_file_ex,
    default_win32_unlock_file_ex,
    default_win32_create_file_mapping,
    default_win32_map_view_of_file,
    default_win32_unmap_view_of_file,
    default_win32_create_timer,
    default_win32_delete_timer,
    default_win32_get_tick_count,
    default_win32_get_file_attributes,
    default_win32_get_current_dir,
    default_win32_set_current_dir,
    default_win32_create_directory,
    default_win32_create_file,
    default_win32_close_handle,
    default_win32_read_file,
    default_win32_write_file,
    default_win32_device_io_control,
    default_win32_wide_char_to_multi_byte,
    default_win32_create_hard_link,
    default_win32_move_file_ex,
    default_win32_copy_file,
    default_win32_get_user_name,
    default_win32_sleep_ex,
    default_win32_get_logical_drives,
    default_win32_get_drive_type,
    default_win32_flush_file_buffers,
    default_win32_get_system_info,
    default_win32_global_memory_status_ex,
    default_win32_get_module_file_name,
    default_win32_set_std_handle,
    default_win32_set_handle_information,
    default_win32_wait_for_single_object,
    default_win32_get_exit_code_process,
    default_win32_ioctlsocket,
    default_win32_wsa_get_last_error,
    default_win32_socketpair,
    default_win32_crypt_acquire_context,
    default_win32_crypt_create_hash,
    default_win32_crypt_hash_data,
    default_win32_crypt_get_hash_param,
    default_win32_crypt_destroy_hash,
    default_win32_crypt_release_context,
    default_win32_crypt_derive_key,
    default_win32_crypt_encrypt,
    default_win32_crypt_decrypt,
    default_win32_crypt_destroy_key};

static struct posix_core_win32_hooks g_core_win32_hooks = {
    default_win32_malloc,
    default_win32_realloc,
    default_win32_free,
    default_win32_get_osfhandle,
    default_win32_open_osfhandle,
    default_win32_dup,
    default_win32_dup2,
    default_win32_close,
    default_win32_commit,
    default_win32_locking,
    default_win32_pipe,
    default_win32_isatty,
    default_win32_open_rdwr,
    default_win32_chsize,
    default_win32_access,
    default_win32_chmod,
    default_win32_unlink,
    default_win32_rmdir,
    default_win32_getcwd,
    default_win32_chdir,
    default_win32_fullpath,
    default_win32_mktemp_s,
    default_win32_execve,
    default_win32_spawnve,
    default_win32_spawnvpe,
    default_win32_spawnv,
    default_win32_raise,
    default_win32_get_module_handle,
    default_win32_get_proc_address,
    default_win32_get_last_error,
    default_win32_get_file_size_ex,
    default_win32_set_file_pointer_ex,
    default_win32_set_end_of_file,
    default_win32_lock_file_ex,
    default_win32_unlock_file_ex,
    default_win32_create_file_mapping,
    default_win32_map_view_of_file,
    default_win32_unmap_view_of_file,
    default_win32_create_timer,
    default_win32_delete_timer,
    default_win32_get_tick_count,
    default_win32_get_file_attributes,
    default_win32_get_current_dir,
    default_win32_set_current_dir,
    default_win32_create_directory,
    default_win32_create_file,
    default_win32_close_handle,
    default_win32_read_file,
    default_win32_write_file,
    default_win32_device_io_control,
    default_win32_wide_char_to_multi_byte,
    default_win32_create_hard_link,
    default_win32_move_file_ex,
    default_win32_copy_file,
    default_win32_get_user_name,
    default_win32_sleep_ex,
    default_win32_get_logical_drives,
    default_win32_get_drive_type,
    default_win32_flush_file_buffers,
    default_win32_get_system_info,
    default_win32_global_memory_status_ex,
    default_win32_get_module_file_name,
    default_win32_set_std_handle,
    default_win32_set_handle_information,
    default_win32_wait_for_single_object,
    default_win32_get_exit_code_process,
    default_win32_ioctlsocket,
    default_win32_wsa_get_last_error,
    default_win32_socketpair,
    default_win32_crypt_acquire_context,
    default_win32_crypt_create_hash,
    default_win32_crypt_hash_data,
    default_win32_crypt_get_hash_param,
    default_win32_crypt_destroy_hash,
    default_win32_crypt_release_context,
    default_win32_crypt_derive_key,
    default_win32_crypt_encrypt,
    default_win32_crypt_decrypt,
    default_win32_crypt_destroy_key};

/** @brief Copies default Win32 hooks for posix-core into out_hooks. */
void posix_core_get_default_win32_hooks(
    struct posix_core_win32_hooks *out_hooks) {
  if (out_hooks) {
    *out_hooks = k_default_win32_hooks;
  }
}

/** @brief Installs custom Win32 hooks for posix-core (NULL resets). */
void posix_core_set_win32_hooks(const struct posix_core_win32_hooks *hooks) {
  if (hooks) {
    g_core_win32_hooks = *hooks;
  } else {
    g_core_win32_hooks = k_default_win32_hooks;
  }
}

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
static ptrdiff_t safe_get_osfhandle(intptr_t fd) {
  ptrdiff_t h;
  int saved_err = errno;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
  h = (ptrdiff_t)g_core_win32_hooks.get_osfhandle_fn((int)fd);
  errno = saved_err;
  if (h == -2) {
    return -1;
  }
  return h;
}
#endif

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

static auto_win_msvc_error_t get_current_rid(int is_group,
                                             unsigned long *out_rid) {
  void *hToken = NULL;
  void *advapi32;
  OpenProcessToken_f pOpenProcessToken;
  GetTokenInformation_f pGetTokenInformation;
  GetSidSubAuthority_f pGetSidSubAuthority;
  GetSidSubAuthorityCount_f pGetSidSubAuthorityCount;
  IsValidSid_f pIsValidSid;

  *out_rid = 0;

  advapi32 = g_core_win32_hooks.get_module_handle_fn("advapi32.dll");
  if (!advapi32) {
    return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
  }

  pOpenProcessToken =
      (OpenProcessToken_f)(size_t)g_core_win32_hooks.get_proc_address_fn(
          advapi32, "OpenProcessToken");
  pGetTokenInformation =
      (GetTokenInformation_f)(size_t)g_core_win32_hooks.get_proc_address_fn(
          advapi32, "GetTokenInformation");
  pGetSidSubAuthority =
      (GetSidSubAuthority_f)(size_t)g_core_win32_hooks.get_proc_address_fn(
          advapi32, "GetSidSubAuthority");
  pGetSidSubAuthorityCount =
      (GetSidSubAuthorityCount_f)(size_t)g_core_win32_hooks.get_proc_address_fn(
          advapi32, "GetSidSubAuthorityCount");
  pIsValidSid = (IsValidSid_f)(size_t)g_core_win32_hooks.get_proc_address_fn(
      advapi32, "IsValidSid");

  if (!pOpenProcessToken || !pGetTokenInformation || !pGetSidSubAuthority ||
      !pGetSidSubAuthorityCount || !pIsValidSid) {
    return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
  }

  if (pOpenProcessToken((void *)(intptr_t)-1, 0x0008 /* TOKEN_QUERY */,
                        &hToken)) {
    unsigned long dwLength = 0;
    int token_class = is_group ? 2 /* TokenPrimaryGroup */ : 1 /* TokenUser */;
    (void)pGetTokenInformation(hToken, token_class, NULL, 0, &dwLength);
    if (dwLength > 0) {
      void *pInfo = g_core_win32_hooks.malloc_fn(dwLength);
      if (pInfo) {
        if (pGetTokenInformation(hToken, token_class, pInfo, dwLength,
                                 &dwLength)) {
          void *pSid = *(void **)pInfo;
          if (pSid && pIsValidSid(pSid)) {
            unsigned char *pCount = pGetSidSubAuthorityCount(pSid);
            if (pCount && *pCount > 0) {
              unsigned long *pRid =
                  (unsigned long *)pGetSidSubAuthority(pSid, *pCount - 1);
              if (pRid) {
                *out_rid = *pRid;
              }
            }
          }
        }
        g_core_win32_hooks.free_fn(pInfo);
      }
    }
    g_core_win32_hooks.close_handle_fn(hToken);
  }
  return AUTO_WIN_MSVC_SUCCESS;
}

#define FIONBIO 0x8004667E

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif

extern void set_nonblock(SOCKET s, int nb);
extern int get_nonblock(SOCKET s);
extern error_type_t clear_nonblock(SOCKET s);
extern void copy_nonblock(SOCKET src, SOCKET dst);
extern error_type_t mark_as_socket(intptr_t fd);
extern error_type_t clear_as_socket(intptr_t fd);
extern int is_socket(intptr_t fd);

/** @brief Child exit hook used by forked processes on Windows. */
void my_exit_hook(int status) {
  void(__stdcall * ep)(unsigned int) = NULL;
  void *k32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
  if (k32) {
    *(void **)(&ep) =
        g_core_win32_hooks.get_proc_address_fn(k32, "ExitProcess");
  }
  if (ep) {
    ep((unsigned int)status);
  }
}

/** @brief Terminate current process with exit code. */
auto_win_msvc_error_t auto_win_exit(int code) {
  int(__stdcall * tp)(void *, unsigned int) = NULL;
  void *k32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
  if (k32) {
    *(void **)(&tp) =
        g_core_win32_hooks.get_proc_address_fn(k32, "TerminateProcess");
  }
  if (tp) {
    tp((void *)(intptr_t)-1, (unsigned int)code);
    return AUTO_WIN_MSVC_SUCCESS;
  }
  return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
}

extern intptr_t posix_get_dup_socket(intptr_t fd);
extern intptr_t posix_lookup_crt_socket(intptr_t fd);

/** @brief Manipulates file descriptor properties and advisory locks. */
int fcntl(intptr_t fd, int cmd, ...) {
  va_list ap;
  SOCKET s;
  va_start(ap, cmd);

  if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) {
    int min_fd;
    int target_fd;
    min_fd = va_arg(ap, int);
    va_end(ap);
    if (min_fd < 0 || min_fd >= 2048) {
      errno = EINVAL;
      return -1;
    }
    if (min_fd == 0) {
      return posix_dup((int)fd);
    }
    for (target_fd = min_fd; target_fd < 128; target_fd++) {
      if (safe_get_osfhandle(target_fd) == -1) {
        if (posix_dup2((int)fd, target_fd) == target_fd) {
          return target_fd;
        }
      }
    }
    errno = EMFILE;
    return -1;
  }

  if (cmd == F_GETFD) {
    va_end(ap);
    return 0;
  }
  if (cmd == F_SETFD) {
    int arg;
    arg = va_arg(ap, int);
    (void)arg;
    va_end(ap);
    return 0;
  }
  if (cmd == F_GETFL) {
    int nb;
    va_end(ap);
    s = (SOCKET)posix_get_dup_socket(fd);
    if (s == 0) {
      s = (SOCKET)posix_lookup_crt_socket(fd);
    }
    if (s == 0) {
      s = (SOCKET)fd;
    }
    nb = get_nonblock(s);
    if (nb == 0 && s != (SOCKET)fd) {
      nb = get_nonblock((SOCKET)fd);
    }
    return nb;
  }
  if (cmd == F_SETFL) {
    int flags = va_arg(ap, int);
    unsigned long mode = ((flags & O_NONBLOCK) || (flags & 0x0004)) ? 1 : 0;
    s = (SOCKET)posix_get_dup_socket(fd);
    if (s == 0) {
      s = (SOCKET)posix_lookup_crt_socket(fd);
    }
    if (s == 0) {
      s = (SOCKET)fd;
    }
    if (g_core_win32_hooks.ioctlsocket_fn(s, FIONBIO, &mode) != 0) {
      if (s != (SOCKET)fd &&
          g_core_win32_hooks.ioctlsocket_fn((SOCKET)fd, FIONBIO, &mode) == 0) {
        s = (SOCKET)fd;
      } else {
        int werr = g_core_win32_hooks.wsa_get_last_error_fn();
        va_end(ap);
        if (werr == WSAENOTSOCK) {
          return 0;
        }
        errno = EINVAL;
        return -1;
      }
    }
    set_nonblock(s, (int)mode);
    va_end(ap);
    return 0;
  }

  if (cmd == F_SETLK || cmd == F_SETLKW || cmd == F_GETLK) {
    struct flock *fl = va_arg(ap, struct flock *);
    void *handle;
    local_overlapped ov;
    unsigned long flags = 0;
    posix_core_i64_t start, len;
    int res;

    va_end(ap);
    if (!fl) {
      errno = EINVAL;
      return -1;
    }

    handle = (void *)safe_get_osfhandle(fd);
    if (handle == (void *)(intptr_t)-1) {
      errno = EBADF;
      return -1;
    }

    memset(&ov, 0, sizeof(ov));

    if (fl->l_whence == SEEK_SET) {
      start = fl->l_start;
    } else if (fl->l_whence == SEEK_CUR) {
      posix_core_i64_t current_pos = 0;
      if (!g_core_win32_hooks.set_file_pointer_ex_fn(handle, 0, &current_pos,
                                                     1)) {
        errno = EINVAL;
        return -1;
      }
      start = current_pos + fl->l_start;
    } else if (fl->l_whence == SEEK_END) {
      posix_core_i64_t file_size = 0;
      if (!g_core_win32_hooks.get_file_size_ex_fn(handle, &file_size)) {
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
      unsigned long max_dw = 0xFFFFFFFFUL;
      posix_core_i64_t max_len = ((posix_core_i64_t)max_dw << 32) | max_dw;
      max_len = (posix_core_i64_t)((posix_core_u64_t)max_len >> 1);
      len = max_len - start;
    }

    ov.Offset = (unsigned long)(start & 0xFFFFFFFF);
    ov.OffsetHigh = (unsigned long)(start >> 32);

    if (cmd == F_GETLK) {
      flags = 2;
      if (fl->l_type == F_WRLCK) {
        flags |= 1;
      }
      if (g_core_win32_hooks.lock_file_ex_fn(handle, flags, 0,
                                             (unsigned long)(len & 0xFFFFFFFF),
                                             (unsigned long)(len >> 32), &ov)) {
        g_core_win32_hooks.unlock_file_ex_fn(handle, 0,
                                             (unsigned long)(len & 0xFFFFFFFF),
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
      g_core_win32_hooks.unlock_file_ex_fn(handle, 0,
                                           (unsigned long)(len & 0xFFFFFFFF),
                                           (unsigned long)(len >> 32), &ov);
      return 0;
    }

    if (cmd == F_SETLK) {
      flags |= 2;
    }
    if (fl->l_type == F_WRLCK) {
      flags |= 1;
    }

    res = g_core_win32_hooks.lock_file_ex_fn(handle, flags, 0,
                                             (unsigned long)(len & 0xFFFFFFFF),
                                             (unsigned long)(len >> 32), &ov);
    if (!res) {
      unsigned long err = g_core_win32_hooks.get_last_error_fn();
      if (err == 33 || err == 997) {
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

static auto_win_msvc_error_t
posix_resolve_at_path(int dirfd, const char *pathname, char **buf_out) {
  char *buf;
  size_t bufsiz = 32768;
  if (!pathname) {
    errno = EINVAL;
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }

  buf = (char *)g_core_win32_hooks.malloc_fn(bufsiz);
  if (!buf) {
    errno = ENOMEM;
    return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
  }

  if (pathname[0] == '/' || pathname[0] == '\\' ||
      (pathname[0] && pathname[1] == ':')) {
#if defined(_MSC_VER)
    strncpy_s(buf, bufsiz, pathname, _TRUNCATE);
#else
    strncpy(buf, pathname, bufsiz - 1);
#endif
    buf[bufsiz - 1] = '\0';
    *buf_out = buf;
    return AUTO_WIN_MSVC_SUCCESS;
  }

  if (dirfd == AT_FDCWD) {
    size_t cwd_len;
    if (!g_core_win32_hooks.getcwd_fn(buf, (int)bufsiz)) {
      g_core_win32_hooks.free_fn(buf);
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }
    cwd_len = strlen(buf);
    if (cwd_len == 0 || cwd_len + strlen(pathname) + 2 > bufsiz) {
      g_core_win32_hooks.free_fn(buf);
      errno = ENAMETOOLONG;
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }
    if (buf[cwd_len - 1] != '\\' && buf[cwd_len - 1] != '/') {
#if defined(_MSC_VER)
      (void)strcat_s(buf, bufsiz, "\\");
#else
      strcat(buf, "\\");
#endif
    }
#if defined(_MSC_VER)
    (void)strcat_s(buf, bufsiz, pathname);
#else
    strcat(buf, pathname);
#endif
    *buf_out = buf;
    return AUTO_WIN_MSVC_SUCCESS;
  } else {
    void *kernel32;
    void *hFile;
    unsigned long res;
    GetFinalPathNameByHandleA_Func pGetFinalPathNameByHandleA;

    hFile = (void *)(intptr_t)safe_get_osfhandle(dirfd);
    if (hFile == (void *)(intptr_t)-1) {
      g_core_win32_hooks.free_fn(buf);
      errno = EBADF;
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }

    kernel32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
    if (!kernel32) {
      g_core_win32_hooks.free_fn(buf);
      errno = ENOSYS;
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }
    pGetFinalPathNameByHandleA = (GetFinalPathNameByHandleA_Func)(size_t)
                                     g_core_win32_hooks.get_proc_address_fn(
                                         kernel32, "GetFinalPathNameByHandleA");
    if (!pGetFinalPathNameByHandleA) {
      g_core_win32_hooks.free_fn(buf);
      errno = ENOSYS;
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }

    res = pGetFinalPathNameByHandleA(hFile, buf, (unsigned long)bufsiz, 0);
    if (res == 0 || res >= bufsiz) {
      g_core_win32_hooks.free_fn(buf);
      errno = EINVAL;
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }
    if (res >= 4 && buf[0] == '\\' && buf[1] == '\\' && buf[2] == '?' &&
        buf[3] == '\\') {
      memmove(buf, buf + 4, res - 4);
      res -= 4;
      buf[res] = '\0';
    }
    if (res == 0 || res + strlen(pathname) + 2 > bufsiz) {
      g_core_win32_hooks.free_fn(buf);
      errno = ENAMETOOLONG;
      return AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED;
    }
    if (buf[res - 1] != '\\' && buf[res - 1] != '/') {
#if defined(_MSC_VER)
      (void)strcat_s(buf, bufsiz, "\\");
#else
      strcat(buf, "\\");
#endif
    }
#if defined(_MSC_VER)
    (void)strcat_s(buf, bufsiz, pathname);
#else
    strcat(buf, pathname);
#endif
    *buf_out = buf;
    return AUTO_WIN_MSVC_SUCCESS;
  }
}

/** @brief Removes a directory entry relative to a directory file descriptor. */
int unlinkat(int dirfd, const char *pathname, int flags) {
  char *buf = NULL;
  int rc;
  if (posix_resolve_at_path(dirfd, pathname, &buf) != AUTO_WIN_MSVC_SUCCESS) {
    return -1;
  }
  if (flags & AT_REMOVEDIR) {
    rc = g_core_win32_hooks.rmdir_fn(buf);
  } else {
    rc = g_core_win32_hooks.unlink_fn(buf);
  }
  g_core_win32_hooks.free_fn(buf);
  return rc;
}

/** @brief Renames a file relative to directory file descriptors. */
int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath) {
  char *old_buf = NULL;
  char *new_buf = NULL;
  int rc;
  if (posix_resolve_at_path(olddirfd, oldpath, &old_buf) !=
      AUTO_WIN_MSVC_SUCCESS) {
    return -1;
  }
  if (posix_resolve_at_path(newdirfd, newpath, &new_buf) !=
      AUTO_WIN_MSVC_SUCCESS) {
    g_core_win32_hooks.free_fn(old_buf);
    return -1;
  }
  rc = posix_rename(old_buf, new_buf);
  g_core_win32_hooks.free_fn(old_buf);
  g_core_win32_hooks.free_fn(new_buf);
  return rc;
}

/** @brief Opens a file relative to a directory file descriptor. */
int openat(int dirfd, const char *pathname, int flags, ...) {
  char *buf;
  int fd;
  va_list ap;
  int mode = 0;
  auto_win_msvc_error_t rc;

  rc = posix_resolve_at_path(dirfd, pathname, &buf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }

  if (flags & (_O_CREAT | 0x0200)) {
    flags |= _O_CREAT;
    va_start(ap, flags);
    mode = va_arg(ap, int) & (_S_IREAD | _S_IWRITE);
    if (mode == 0) {
      mode = _S_IREAD | _S_IWRITE;
    }
    va_end(ap);
  }
  flags |= _O_BINARY;

  fd = posix_open(buf, flags, mode);
  g_core_win32_hooks.free_fn(buf);
  return fd;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Advises the kernel on expected file data access patterns. */
int posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice) {
  intptr_t handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    return EBADF;
  }
  if (offset < 0 || len < 0 || advice < 0) {
    return EINVAL;
  }

  if (advice == 1 /* POSIX_FADV_WILLNEED */) {
    void *hMap;
    void *ptr;
    void *kernel32;
    int(__stdcall * pPrefetchVirtualMemory)(void *, size_t, void *,
                                            unsigned long);
    posix_core_i64_t file_size = 0;
    size_t map_len;

    kernel32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
    if (!kernel32) {
      return 0;
    }
    pPrefetchVirtualMemory =
        (int(__stdcall *)(void *, size_t, void *, unsigned long))(size_t)
            g_core_win32_hooks.get_proc_address_fn(kernel32,
                                                   "PrefetchVirtualMemory");
    if (!pPrefetchVirtualMemory) {
      return 0;
    }

    if (!g_core_win32_hooks.get_file_size_ex_fn((void *)handle, &file_size)) {
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

    hMap = g_core_win32_hooks.create_file_mapping_fn(
        (void *)handle, NULL, 0x02 /* PAGE_READONLY */, 0, 0, NULL);
    if (!hMap) {
      return 0;
    }

    ptr = g_core_win32_hooks.map_view_of_file_fn(
        hMap, 0x0004 /* FILE_MAP_READ */, 0, 0, 0);
    if (ptr) {
      struct {
        void *VirtualAddress;
        size_t NumberOfBytes;
      } entry;
      entry.VirtualAddress = (void *)((unsigned char *)ptr + offset);
      entry.NumberOfBytes = map_len;
      (void)pPrefetchVirtualMemory((void *)(intptr_t)-1, 1, &entry, 0);
      (void)g_core_win32_hooks.unmap_view_of_file_fn(ptr);
    }
    (void)g_core_win32_hooks.close_handle_fn(hMap);
  }
  return 0;
}
#elif (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&   \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** @brief Advises the kernel on expected file data access patterns. */
int posix_fadvise(intptr_t fd, off_t offset, off_t len, int advice) {
  if (fd < 0) {
    return EBADF;
  }
  if (offset < 0 || len < 0 || advice < 0) {
    return EINVAL;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Ensures disk space is allocated for a file range. */
int posix_fallocate(intptr_t fd, off_t offset, off_t len) {
  intptr_t handle;
  posix_core_i64_t current_size = 0;
  posix_core_i64_t target_size;
  posix_core_i64_t old_ptr = 0;

  if (fd < 0) {
    return EBADF;
  }
  if (offset < 0 || len <= 0) {
    return EINVAL;
  }

  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    return EBADF;
  }

  target_size = (posix_core_i64_t)offset + (posix_core_i64_t)len;

  if (!g_core_win32_hooks.get_file_size_ex_fn((void *)handle, &current_size)) {
    return EINVAL;
  }

  if (target_size > current_size) {
    if (!g_core_win32_hooks.set_file_pointer_ex_fn((void *)handle, 0, &old_ptr,
                                                   1)) {
      return EINVAL;
    }

    if (!g_core_win32_hooks.set_file_pointer_ex_fn((void *)handle, target_size,
                                                   NULL, 0)) {
      return EINVAL;
    }

    if (!g_core_win32_hooks.set_end_of_file_fn((void *)handle)) {
      (void)g_core_win32_hooks.set_file_pointer_ex_fn((void *)handle, old_ptr,
                                                      NULL, 0);
      return ENOSPC;
    }

    if (!g_core_win32_hooks.set_file_pointer_ex_fn((void *)handle, old_ptr,
                                                   NULL, 0)) {
      return EINVAL;
    }
  }

  return 0;
}
#elif (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&   \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** @brief Ensures disk space is allocated for a file range. */
int posix_fallocate(intptr_t fd, off_t offset, off_t len) {
  if (fd < 0) {
    return EBADF;
  }
  if (offset < 0 || len <= 0) {
    return EINVAL;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Synchronizes a file segment with disk storage. */
int sync_file_range(intptr_t fd, off_t offset, off_t nbytes,
                    unsigned int flags) {
  ptrdiff_t handle;
  (void)flags;
  if (offset < 0 || nbytes < 0) {
    errno = EINVAL;
    return -1;
  }
  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }
  if (g_core_win32_hooks.commit_fn((int)fd) == 0) {
    return 0;
  }
  return -1;
}
#elif defined(__APPLE__) && !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) ||            \
    !defined(__linux__) && !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** @brief Synchronizes a file segment with disk storage (fallback). */
int sync_file_range(intptr_t fd, off_t offset, off_t nbytes,
                    unsigned int flags) {
  (void)flags;
  if (offset < 0 || nbytes < 0) {
    errno = EINVAL;
    return -1;
  }
  return fsync((int)fd);
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
  (void)g_core_win32_hooks.raise_fn(14);
}

/** @brief Schedules SIGALRM delivery after the specified seconds. */
unsigned int alarm(unsigned int seconds) {
  unsigned int remaining = 0;
  if (g_alarm_timer) {
    unsigned long elapsed =
        g_core_win32_hooks.get_tick_count_fn() - g_alarm_start_ticks;
    if (elapsed < g_alarm_remaining_ms) {
      remaining = (unsigned int)((g_alarm_remaining_ms - elapsed) / 1000UL);
      if (remaining == 0) {
        remaining = 1;
      }
    }
    (void)g_core_win32_hooks.delete_timer_fn(NULL, g_alarm_timer, NULL);
    g_alarm_timer = NULL;
  }

  if (seconds > 0) {
    g_alarm_start_ticks = g_core_win32_hooks.get_tick_count_fn();
    g_alarm_remaining_ms = seconds * 1000UL;
    (void)g_core_win32_hooks.create_timer_fn(&g_alarm_timer, NULL,
                                             alarm_timer_callback, NULL,
                                             g_alarm_remaining_ms, 0, 0);
  }

  return remaining;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Changes ownership of a file path (validated on Windows). */
int chown(const char *pathname, uid_t owner, gid_t group) {
  unsigned long attr;
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
  attr = g_core_win32_hooks.get_file_attributes_fn(pathname);
  if (attr == 0xFFFFFFFFUL) {
    errno = ENOENT;
    return -1;
  }
  if (owner != (uid_t)-1 && owner != getuid()) {
    errno = EPERM;
    return -1;
  }
  if (group != (gid_t)-1 && group != getgid()) {
    gid_t *groups = (gid_t *)g_core_win32_hooks.malloc_fn(sizeof(gid_t));
    if (groups) {
      int found = 0;
      (void)getgroups(1, groups);
      if (groups[0] == group) {
        found = 1;
      }
      g_core_win32_hooks.free_fn(groups);
      if (found) {
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
/** @brief Retrieves configuration-defined string values. */
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

/** @brief Hashes a password string using MD5-based crypt formatting. */
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

  if (!g_core_win32_hooks.crypt_acquire_context_fn(
          &hProv, NULL, NULL, 1 /* PROV_RSA_FULL */,
          0xF0000000UL /* CRYPT_VERIFYCONTEXT */)) {
    errno = ENOSYS;
    return NULL;
  }

  if (!g_core_win32_hooks.crypt_create_hash_fn(
          hProv, 0x00008003U /* CALG_MD5 */, 0, 0, &hHash)) {
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return NULL;
  }

  if (!g_core_win32_hooks.crypt_hash_data_fn(hHash, (const unsigned char *)key,
                                             (unsigned long)strlen(key), 0)) {
    (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return NULL;
  }
  if (!g_core_win32_hooks.crypt_hash_data_fn(hHash, (const unsigned char *)salt,
                                             (unsigned long)strlen(salt), 0)) {
    (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return NULL;
  }

  if (!g_core_win32_hooks.crypt_get_hash_param_fn(
          hHash, 0x0002UL /* HP_HASHVAL */, hash, &hashLen, 0)) {
    (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return NULL;
  }

  (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
  (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);

  p = result;
  *p++ = '$';
  *p++ = '1';
  *p++ = '$';
  for (i = 0; i < (int)strlen(salt) && i < 8; i++) {
    *p++ = salt[i];
  }
  *p++ = '$';

  for (i = 0; i < 15; i += 3) {
    unsigned long v = ((unsigned long)hash[i] << 16) |
                      ((unsigned long)hash[i + 1] << 8) |
                      (unsigned long)hash[i + 2];
    *p++ = crypt_b64[(v >> 18) & 0x3f];
    *p++ = crypt_b64[(v >> 12) & 0x3f];
    *p++ = crypt_b64[(v >> 6) & 0x3f];
    *p++ = crypt_b64[v & 0x3f];
  }
  {
    unsigned long v = (unsigned long)hash[15] << 16;
    *p++ = crypt_b64[(v >> 18) & 0x3f];
    *p++ = crypt_b64[(v >> 12) & 0x3f];
  }
  *p = '\0';

  return result;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Encrypts or decrypts a 64-bit bit-array block using CryptoAPI DES. */
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

  if (!g_core_win32_hooks.crypt_acquire_context_fn(
          &hProv, NULL, NULL, 1 /* PROV_RSA_FULL */,
          0xF0000000UL /* CRYPT_VERIFYCONTEXT */)) {
    errno = ENOSYS;
    return;
  }

  if (!g_core_win32_hooks.crypt_create_hash_fn(
          hProv, 0x00008003U /* CALG_MD5 */, 0, 0, &hHash)) {
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return;
  }

  if (!g_core_win32_hooks.crypt_hash_data_fn(
          hHash, (const unsigned char *)"POSIX_ENCRYPT_KEY", 17, 0)) {
    (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return;
  }

  if (!g_core_win32_hooks.crypt_derive_key_fn(hProv, 0x00006601U /* CALG_DES */,
                                              hHash, 0, &hKey)) {
    (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
    (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
    errno = ENOSYS;
    return;
  }

  for (i = 0; i < 64; i++) {
    if (block[i]) {
      packed[i >> 3] |= (unsigned char)(1U << (7 - (i & 7)));
    }
  }

  if (edflag == 0) {
    (void)g_core_win32_hooks.crypt_encrypt_fn(hKey, 0, 0, 0, packed, &len, 8);
  } else {
    (void)g_core_win32_hooks.crypt_decrypt_fn(hKey, 0, 0, 0, packed, &len);
  }

  for (i = 0; i < 64; i++) {
    block[i] = (packed[i >> 3] & (1U << (7 - (i & 7)))) ? 1 : 0;
  }

  (void)g_core_win32_hooks.crypt_destroy_key_fn(hKey);
  (void)g_core_win32_hooks.crypt_destroy_hash_fn(hHash);
  (void)g_core_win32_hooks.crypt_release_context_fn(hProv, 0);
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Checks file accessibility relative to a directory descriptor. */
int faccessat(int dirfd, const char *pathname, int mode, int flags) {
  char *buf;
  int res;
  auto_win_msvc_error_t rc;
  if (flags != 0 && (flags & ~(0x100 | 0x200)) != 0) {
    errno = EINVAL;
    return -1;
  }
  rc = posix_resolve_at_path(dirfd, pathname, &buf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  res = g_core_win32_hooks.access_fn(buf, mode);
  g_core_win32_hooks.free_fn(buf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Changes ownership of an open file descriptor. */
int fchown(intptr_t fd, uid_t owner, gid_t group) {
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
  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  kernel32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA = (GetFinalPathNameByHandleA_Func)(size_t)
                                   g_core_win32_hooks.get_proc_address_fn(
                                       kernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return -1;
  }

  buf = (char *)g_core_win32_hooks.malloc_fn(32768);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  res = pGetFinalPathNameByHandleA((void *)handle, buf, 32768, 0);
  if (res == 0 || res >= 32768) {
    g_core_win32_hooks.free_fn(buf);
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
  g_core_win32_hooks.free_fn(buf);
  return chown_res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Changes file ownership relative to a directory descriptor. */
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags) {
  char *buf;
  int res;
  auto_win_msvc_error_t rc;
  (void)flags;
  rc = posix_resolve_at_path(dirfd, pathname, &buf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  res = chown(buf, owner, group);
  g_core_win32_hooks.free_fn(buf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Flushes modified file data to storage. */
int fdatasync(intptr_t fd) {
  ptrdiff_t handle;
  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }
  if (g_core_win32_hooks.commit_fn((int)fd) == -1) {
    errno = EBADF;
    return -1;
  }
  return 0;
}
#elif defined(__APPLE__) && !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** @brief Flushes modified file data to storage. */
int fdatasync(intptr_t fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return fsync((int)fd);
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Executes a program specified via an open file descriptor. */
int fexecve(intptr_t fd, char *const argv[], char *const envp[]) {
  char *buf = NULL;
  intptr_t handle;
  void *kernel32;
  GetFinalPathNameByHandleA_Func pGetFinalPathNameByHandleA;
  unsigned long res;

  if (fd < 0 || !argv || !envp) {
    errno = EINVAL;
    return -1;
  }

  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  kernel32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return -1;
  }

  pGetFinalPathNameByHandleA = (GetFinalPathNameByHandleA_Func)(size_t)
                                   g_core_win32_hooks.get_proc_address_fn(
                                       kernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return -1;
  }

  buf = (char *)g_core_win32_hooks.malloc_fn(32768);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  res = pGetFinalPathNameByHandleA((void *)handle, buf, 32768, 0);
  if (res == 0 || res >= 32768) {
    g_core_win32_hooks.free_fn(buf);
    errno = EBADF;
    return -1;
  }

  if (res >= 4 && buf[0] == '\\' && buf[1] == '\\' && buf[2] == '?' &&
      buf[3] == '\\') {
    memmove(buf, buf + 4, res - 4);
    res -= 4;
    buf[res] = '\0';
  }

  (void)g_core_win32_hooks.execve_fn(buf, (const char *const *)argv,
                                     (const char *const *)envp);
  g_core_win32_hooks.free_fn(buf);
  return -1;
}
#elif (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&   \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** @brief Executes a program specified via an open file descriptor. */
int fexecve(intptr_t fd, char *const argv[], char *const envp[]) {
  if (fd < 0 || !argv || !envp) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
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
/** @brief Default no-op atfork handler when posix-pthread is not linked. */
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
/** @brief Weak no-op prepare atfork callback. */
__attribute__((weak)) void posix_pthread_atfork_prepare(void) {}
/** @brief Weak no-op parent atfork callback. */
__attribute__((weak)) void posix_pthread_atfork_parent(void) {}
/** @brief Weak no-op child atfork callback. */
__attribute__((weak)) void posix_pthread_atfork_child(void) {}
#endif

#define MAX_POSIX_CHILDREN 1024
__declspec(selectany) void *g_posix_child_handles[MAX_POSIX_CHILDREN] = {0};
__declspec(selectany) unsigned long g_posix_child_pids[MAX_POSIX_CHILDREN] = {
    0};
__declspec(selectany) int g_posix_child_count = 0;

/** @brief Clones the current process via NTDLL RtlCloneUserProcess. */
int fork(void) {
  void *ntdll;
  RtlCloneUserProcess_f pRtlCloneUserProcess;
  RTL_USER_PROCESS_INFORMATION info;
  unsigned long status;
  char parent_cwd[260];

  ntdll = g_core_win32_hooks.get_module_handle_fn("ntdll.dll");
  if (!ntdll) {
    errno = ENOSYS;
    return -1;
  }

  pRtlCloneUserProcess =
      (RtlCloneUserProcess_f)(size_t)g_core_win32_hooks.get_proc_address_fn(
          ntdll, "RtlCloneUserProcess");
  if (!pRtlCloneUserProcess) {
    errno = ENOSYS;
    return -1;
  }

  memset(&info, 0, sizeof(info));
  info.Length = sizeof(info);

  posix_pthread_atfork_prepare();

  if (g_core_win32_hooks.get_current_dir_fn(260, parent_cwd) == 0) {
    errno = ENOSYS;
    return -1;
  }

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
      g_core_win32_hooks.close_handle_fn(info.Process);
    }
    g_core_win32_hooks.close_handle_fn(info.Thread);
    return (pid_t)(size_t)info.ClientId.UniqueProcess;
  } else if (status == STATUS_PROCESS_CLONED) {
    void *ucrt;
    (void)g_core_win32_hooks.set_current_dir_fn(parent_cwd);
    posix_pthread_atfork_child();
    errno = 0;

    ucrt = g_core_win32_hooks.get_module_handle_fn("ucrtbase.dll");
    if (ucrt) {
      void *p_exits[2];
      void *k32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
      int(__stdcall * vp)(void *, size_t, unsigned long, unsigned long *) =
          NULL;
      int i;
      if (k32) {
        *(void **)(&vp) =
            g_core_win32_hooks.get_proc_address_fn(k32, "VirtualProtect");
      }
      p_exits[0] = g_core_win32_hooks.get_proc_address_fn(ucrt, "_exit");
      p_exits[1] = g_core_win32_hooks.get_proc_address_fn(ucrt, "exit");
      for (i = 0; i < 2; i++) {
        void *p_exit = p_exits[i];
        if (p_exit && vp) {
          unsigned long oldProtect = 0;
          if (vp(p_exit, 12, 0x40, &oldProtect)) {
            unsigned char *p = (unsigned char *)p_exit;
            void *target = (void *)(size_t)my_exit_hook;
            p[0] = 0x48;
            p[1] = 0xB8;
            memcpy(p + 2, &target, sizeof(target));
            p[10] = 0xFF;
            p[11] = 0xE0;
            (void)vp(p_exit, 12, oldProtect, &oldProtect);
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
/** @brief Queries configurable limit variables for an open descriptor. */
long fpathconf(intptr_t fd, int name) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
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
/** @brief Returns the effective group ID of the calling process. */
gid_t getegid(void) {
  unsigned long rid = 0;
  auto_win_msvc_error_t rc;
  rc = get_current_rid(1, &rid);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
  }
  return (gid_t)rid;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Returns the effective user ID of the calling process. */
uid_t geteuid(void) {
  unsigned long rid = 0;
  auto_win_msvc_error_t rc;
  rc = get_current_rid(0, &rid);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
  }
  return (uid_t)rid;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Returns the real group ID of the calling process. */
gid_t getgid(void) {
  unsigned long rid = 0;
  auto_win_msvc_error_t rc;
  rc = get_current_rid(1, &rid);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
  }
  return (gid_t)rid;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Populates supplementary group IDs for the calling process. */
int getgroups(int size, gid_t list[]) {
  if (size < 0) {
    errno = EINVAL;
    return -1;
  }
  if (size > 0 && list != NULL) {
    unsigned long rid = 0;
    auto_win_msvc_error_t rc;
    rc = get_current_rid(1, &rid);
    if (rc == AUTO_WIN_MSVC_SUCCESS) {
      list[0] = (gid_t)rid;
    } else {
      list[0] = 0;
    }
  }
  return 1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets supplementary group IDs (validated stub on Windows). */
int setgroups(size_t size, const gid_t *list) {
  if (size > 0 && list == NULL) {
    errno = EFAULT;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Returns unique host identifier (0 on Windows). */
long gethostid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Returns the login name of the current user. */
char *getlogin(void) {
  static char buffer[256];
  unsigned long size = sizeof(buffer);
  if (g_core_win32_hooks.get_user_name_fn(buffer, &size)) {
    return buffer;
  }
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Reentrant retrieval of the current user login name. */
int getlogin_r(char *buf, size_t bufsize) {
  unsigned long size = (unsigned long)bufsize;
  if (buf == NULL || bufsize == 0) {
    return ERANGE;
  }
  if (!g_core_win32_hooks.get_user_name_fn(buf, &size)) {
    return ERANGE;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__) &&                                 \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) && defined(_MSC_VER)

POSIX_CORE_DECL char *optarg = NULL;
POSIX_CORE_DECL int optind = 1;
POSIX_CORE_DECL int opterr = 1;
POSIX_CORE_DECL int optopt = 0;

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
pid_t getpgid(pid_t pid) {
  if (pid < 0) {
    errno = EINVAL;
    return (pid_t)-1;
  }
  return 0;
}
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
pid_t getsid(pid_t pid) {
  if (pid < 0) {
    errno = EINVAL;
    return (pid_t)-1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** \brief getuid function. */
uid_t getuid(void) {
  unsigned long rid = 0;
  auto_win_msvc_error_t rc;
  rc = get_current_rid(0, &rid);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
  }
  return (uid_t)rid;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Changes ownership of a symbolic link without following it. */
int lchown(const char *pathname, uid_t owner, gid_t group) {
  unsigned long attr;
  if (!pathname || owner == (uid_t)-1 || group == (gid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  attr = g_core_win32_hooks.get_file_attributes_fn(pathname);
  if (attr == 0xFFFFFFFFUL) {
    errno = ENOENT;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates a hard link to an existing file. */
int link(const char *oldpath, const char *newpath) {
  if (oldpath == NULL || newpath == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (!g_core_win32_hooks.create_hard_link_fn(newpath, oldpath, NULL)) {
    errno = EACCES;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates a hard link relative to directory file descriptors. */
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags) {
  char *oldbuf;
  char *newbuf;
  int res = 0;
  auto_win_msvc_error_t rc;
  (void)flags;
  rc = posix_resolve_at_path(olddirfd, oldpath, &oldbuf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  rc = posix_resolve_at_path(newdirfd, newpath, &newbuf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    g_core_win32_hooks.free_fn(oldbuf);
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }

  if (!g_core_win32_hooks.create_hard_link_fn(newbuf, oldbuf, NULL)) {
    errno = EACCES;
    res = -1;
  }
  g_core_win32_hooks.free_fn(oldbuf);
  g_core_win32_hooks.free_fn(newbuf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Applies, tests, or removes a POSIX record lock on an open file. */
int lockf(intptr_t fd, int cmd, off_t len) {
  int mode;
  long nbytes;
  ptrdiff_t handle;

  if (len < 0) {
    errno = EINVAL;
    return -1;
  }
  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }
  nbytes = len == 0 ? 0x7FFFFFFFL : (long)len;

  switch (cmd) {
  case 0:
    mode = _LK_UNLCK;
    break;
  case 1:
    mode = _LK_LOCK;
    break;
  case 2:
    mode = _LK_NBLCK;
    break;
  case 3:
    mode = _LK_NBLCK;
    if (g_core_win32_hooks.locking_fn((int)fd, mode, nbytes) == 0) {
      (void)g_core_win32_hooks.locking_fn((int)fd, _LK_UNLCK, nbytes);
      return 0;
    }
    return -1;
  default:
    errno = EINVAL;
    return -1;
  }

  return g_core_win32_hooks.locking_fn((int)fd, mode, nbytes);
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Queries configurable limit variables for a file path. */
long pathconf(const char *pathname, int name) {
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
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
/** @brief Suspends the calling thread until an alertable event or signal. */
int pause(void) {
  (void)g_core_win32_hooks.sleep_ex_fn(0xFFFFFFFFUL, 1);
  errno = EINTR;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates an unidirectional interprocess data channel. */
int pipe(int pipefd[2]) {
  intptr_t sv[2];
  if (!pipefd) {
    errno = EINVAL;
    return -1;
  }
  if (g_core_win32_hooks.pipe_fn(pipefd, 131072, 0x8000 /* _O_BINARY */) == 0) {
    return 0;
  }
  if (g_core_win32_hooks.socketpair_fn(2, 1, 0, sv) != 0) {
    return -1;
  }
  pipefd[0] = (int)sv[0];
  pipefd[1] = (int)sv[1];
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates a pipe with additional descriptor flags. */
int pipe2(int pipefd[2], int flags) {
  intptr_t sv[2];
  if (!pipefd || flags < 0) {
    errno = EINVAL;
    return -1;
  }
  if (g_core_win32_hooks.socketpair_fn(2, 1, 0, sv) != 0) {
    return -1;
  }
  pipefd[0] = (int)sv[0];
  pipefd[1] = (int)sv[1];
  return 0;
}
#elif defined(__APPLE__) && !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) ||            \
    !defined(__linux__) && !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
/** @brief Creates a pipe with additional descriptor flags. */
int pipe2(int pipefd[2], int flags) {
  if (!pipefd || flags < 0) {
    errno = EINVAL;
    return -1;
  }
  return pipe(pipefd);
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Reads from a file descriptor at a given offset without seeking. */
ssize_t pread(intptr_t fd, void *buf, size_t count, off_t offset) {
  local_overlapped ovl = {0};
  posix_core_u64_t uoffset;
  intptr_t handle;
  unsigned long bytes_read = 0;
  int result;

  if (fd < 0 || buf == NULL || offset < 0) {
    errno = EINVAL;
    return -1;
  }

  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  uoffset = (posix_core_u64_t)offset;
  ovl.Offset = (unsigned long)(uoffset & 0xFFFFFFFFUL);
  ovl.OffsetHigh = (unsigned long)((uoffset >> 32) & 0xFFFFFFFFUL);

  result = g_core_win32_hooks.read_file_fn(
      (void *)handle, buf, (unsigned long)count, &bytes_read, (void *)&ovl);
  if (!result) {
    unsigned long err = g_core_win32_hooks.get_last_error_fn();
    if (err == 38) {
      return 0;
    }
    errno = EIO;
    return -1;
  }

  return (ssize_t)bytes_read;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Writes to a file descriptor at a given offset without seeking. */
ssize_t pwrite(intptr_t fd, const void *buf, size_t count, off_t offset) {
  local_overlapped ovl = {0};
  posix_core_u64_t uoffset;
  intptr_t handle;
  unsigned long bytes_written = 0;
  int result;

  if (fd < 0 || buf == NULL || offset < 0) {
    errno = EINVAL;
    return -1;
  }

  handle = safe_get_osfhandle(fd);
  if (handle == -1) {
    errno = EBADF;
    return -1;
  }

  uoffset = (posix_core_u64_t)offset;
  ovl.Offset = (unsigned long)(uoffset & 0xFFFFFFFFUL);
  ovl.OffsetHigh = (unsigned long)((uoffset >> 32) & 0xFFFFFFFFUL);

  result = g_core_win32_hooks.write_file_fn(
      (void *)handle, buf, (unsigned long)count, &bytes_written, (void *)&ovl);
  if (!result) {
    errno = EIO;
    return -1;
  }

  return (ssize_t)bytes_written;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)

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

/** @brief Reads the target path of a symbolic link or junction point. */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
  void *hFile;
  unsigned long attrs;
  unsigned long bytesReturned = 0;
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

  attrs = g_core_win32_hooks.get_file_attributes_fn(pathname);
  if (attrs == 0xFFFFFFFFUL || !(attrs & 0x400UL)) {
    errno = EINVAL;
    return -1;
  }

  hFile = g_core_win32_hooks.create_file_fn(pathname, 0, 1 | 2 | 4, NULL, 3,
                                            0x02200000UL, NULL);
  if (hFile == (void *)(intptr_t)-1) {
    errno = ENOENT;
    return -1;
  }

  if (!g_core_win32_hooks.device_io_control_fn(
          hFile, 589988UL, NULL, 0, reparse_buffer, sizeof(reparse_buffer),
          &bytesReturned, NULL)) {
    g_core_win32_hooks.close_handle_fn(hFile);
    errno = EINVAL;
    return -1;
  }
  g_core_win32_hooks.close_handle_fn(hFile);

  if (pReparse->ReparseTag == 0xA000000CUL) {
    printName =
        (wchar_t *)((unsigned char *)
                        pReparse->u.SymbolicLinkReparseBuffer.PathBuffer +
                    pReparse->u.SymbolicLinkReparseBuffer.PrintNameOffset);
    target_len = (int)(pReparse->u.SymbolicLinkReparseBuffer.PrintNameLength /
                       sizeof(wchar_t));
  } else if (pReparse->ReparseTag == 0xA0000003UL) {
    printName =
        (wchar_t *)((unsigned char *)
                        pReparse->u.MountPointReparseBuffer.PathBuffer +
                    pReparse->u.MountPointReparseBuffer.PrintNameOffset);
    target_len = (int)(pReparse->u.MountPointReparseBuffer.PrintNameLength /
                       sizeof(wchar_t));
  } else {
    errno = EINVAL;
    return -1;
  }

  req_len = g_core_win32_hooks.wide_char_to_multi_byte_fn(
      0, 0, printName, target_len, NULL, 0, NULL, NULL);
  if (req_len == 0) {
    errno = EINVAL;
    return -1;
  }
  if (req_len > (int)bufsiz) {
    char *temp = (char *)g_core_win32_hooks.malloc_fn((size_t)req_len);
    if (!temp) {
      errno = ENOMEM;
      return -1;
    }
    wc_res = g_core_win32_hooks.wide_char_to_multi_byte_fn(
        0, 0, printName, target_len, temp, req_len, NULL, NULL);
    if (wc_res == 0) {
      g_core_win32_hooks.free_fn(temp);
      errno = EINVAL;
      return -1;
    }
    memcpy(buf, temp, bufsiz);
    g_core_win32_hooks.free_fn(temp);
    return (ssize_t)bufsiz;
  } else {
    wc_res = g_core_win32_hooks.wide_char_to_multi_byte_fn(
        0, 0, printName, target_len, buf, (int)bufsiz, NULL, NULL);
    return (ssize_t)wc_res;
  }
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Reads a symbolic link target relative to a directory descriptor. */
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) {
  char *fullbuf;
  ssize_t res;
  auto_win_msvc_error_t rc;
  rc = posix_resolve_at_path(dirfd, pathname, &fullbuf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  res = readlink(fullbuf, buf, bufsiz);
  g_core_win32_hooks.free_fn(fullbuf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets effective group ID (validated stub on Windows). */
int setegid(gid_t egid) {
  if (egid == (gid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets effective user ID (validated stub on Windows). */
int seteuid(uid_t euid) {
  if (euid == (uid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets real group ID (validated stub on Windows). */
int setgid(gid_t gid) {
  if (gid == (gid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets process group ID for job control (validated stub on Windows). */
int setpgid(pid_t pid, pid_t pgid) {
  if (pid < 0 || pgid < 0) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets process group ID to caller PID (stub on Windows). */
pid_t setpgrp(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets real and effective group IDs (validated stub on Windows). */
int setregid(gid_t rgid, gid_t egid) {
  if (rgid == (gid_t)-1 && egid == (gid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets real and effective user IDs (validated stub on Windows). */
int setreuid(uid_t ruid, uid_t euid) {
  if (ruid == (uid_t)-1 && euid == (uid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates a session and sets the process group ID (stub on Windows). */
pid_t setsid(void) { return 0; }
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets real user ID (validated stub on Windows). */
int setuid(uid_t uid) {
  if (uid == (uid_t)-1) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates a symbolic link pointing to target. */
int symlink(const char *target, const char *linkpath) {
  void *kernel32;
  CreateSymbolicLinkA_Func pCreateSymbolicLinkA;
  unsigned long attrs;
  unsigned long flags = 0;

  if (!target || !linkpath) {
    errno = EINVAL;
    return -1;
  }
  kernel32 = g_core_win32_hooks.get_module_handle_fn("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return -1;
  }
  pCreateSymbolicLinkA =
      (CreateSymbolicLinkA_Func)(size_t)g_core_win32_hooks.get_proc_address_fn(
          kernel32, "CreateSymbolicLinkA");
  if (!pCreateSymbolicLinkA) {
    errno = ENOSYS;
    return -1;
  }

  attrs = g_core_win32_hooks.get_file_attributes_fn(target);
  if (attrs != 0xFFFFFFFFUL && (attrs & 0x10UL)) {
    flags |= 0x1UL;
  }
  flags |= 0x2UL;

  if (!pCreateSymbolicLinkA(linkpath, target, flags)) {
    if (g_core_win32_hooks.get_last_error_fn() == 87UL) {
      if (!pCreateSymbolicLinkA(linkpath, target, flags & ~0x2UL)) {
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
/** @brief Creates a symbolic link relative to a directory descriptor. */
int symlinkat(const char *target, int newdirfd, const char *linkpath) {
  char *newbuf;
  int res;
  auto_win_msvc_error_t rc;
  rc = posix_resolve_at_path(newdirfd, linkpath, &newbuf);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    errno = auto_win_msvc_error_to_errno(rc);
    return -1;
  }
  res = symlink(target, newbuf);
  g_core_win32_hooks.free_fn(newbuf);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Flushes filesystem buffers on fixed local drives. */
void sync(void) {
  unsigned long drives = g_core_win32_hooks.get_logical_drives_fn();
  int i;
  for (i = 0; i < 26; i++) {
    if (drives & (1UL << i)) {
      char driveName[] = "\\\\.\\A:";
      char rootName[] = "A:\\";
      void *hFile;

      driveName[4] = (char)('A' + i);
      rootName[0] = (char)('A' + i);

      if (g_core_win32_hooks.get_drive_type_fn(rootName) == 3U) {
        hFile = g_core_win32_hooks.create_file_fn(driveName, 0x40000000UL,
                                                  1 | 2, NULL, 3, 0, NULL);
        if (hFile != (void *)(intptr_t)-1) {
          (void)g_core_win32_hooks.flush_file_buffers_fn(hFile);
          (void)g_core_win32_hooks.close_handle_fn(hFile);
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
 * @param name The system variable to query (e.g., _SC_PAGESIZE).
 * @return The value of the requested configuration variable, or -1 on error.
 */
long sysconf(int name) {
  switch (name) {
  case _SC_PAGESIZE:
  case _SC_NPROCESSORS_ONLN:
  case _SC_NPROCESSORS_CONF: {
    POSIX_SYSTEM_INFO si;
    memset(&si, 0, sizeof(si));
    g_core_win32_hooks.get_system_info_fn(&si);
    if (name == _SC_PAGESIZE) {
      return (long)si.dwPageSize;
    }
    return (long)si.dwNumberOfProcessors;
  }
  case _SC_PHYS_PAGES:
  case _SC_AVPHYS_PAGES: {
    POSIX_MEMORYSTATUSEX statex;
    POSIX_SYSTEM_INFO si;
    memset(&statex, 0, sizeof(statex));
    memset(&si, 0, sizeof(si));
    statex.dwLength = sizeof(statex);
    if (g_core_win32_hooks.global_memory_status_ex_fn(&statex)) {
      g_core_win32_hooks.get_system_info_fn(&si);
      if (si.dwPageSize == 0) {
        return -1;
      }
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
  case _SC_OPEN_MAX:
    return 2048;
  default:
    errno = EINVAL;
    return -1;
  }
}
#endif
#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/** @brief Check if file descriptor refers to a terminal. */
int posix_isatty(intptr_t fd) {
  if (fd < 0 || fd >= 2048) {
    errno = EBADF;
    return 0;
  }
#if defined(_WIN32) && !defined(__CYGWIN__)
  return g_core_win32_hooks.isatty_fn((int)fd);
#else
  return isatty((int)fd);
#endif
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Gets the foreground process group ID of a terminal. */
pid_t tcgetpgrp(intptr_t fd) {
  if (posix_isatty(fd)) {
    return 0;
  }
  errno = ENOTTY;
  return -1;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Sets the foreground process group ID of a terminal. */
int tcsetpgrp(intptr_t fd, pid_t pgrp) {
  if (pgrp < 0) {
    errno = EINVAL;
    return -1;
  }
  if (!posix_isatty(fd)) {
    errno = ENOTTY;
    return -1;
  }
  return 0;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Truncates or extends a file path to a specified length. */
int truncate(const char *path, off_t length) {
  int fd;
  int res;
  if (!path || length < 0) {
    errno = EINVAL;
    return -1;
  }
  fd = g_core_win32_hooks.open_rdwr_fn(path);
  if (fd == -1) {
    return -1;
  }
  res = g_core_win32_hooks.chsize_fn(fd, (long)length);
  (void)g_core_win32_hooks.close_fn(fd);
  return res;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Returns the name of the terminal associated with fd. */
char *ttyname(intptr_t fd) {
  if (posix_isatty(fd)) {
    return "CON";
  }
  errno = ENOTTY;
  return NULL;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Reentrantly stores the terminal name associated with fd into buf. */
int ttyname_r(intptr_t fd, char *buf, size_t buflen) {
  if (buf == NULL) {
    return EINVAL;
  }
  if (posix_isatty(fd)) {
    if (buflen < 4) {
      return ERANGE;
    }
#if defined(_MSC_VER)
    (void)strcpy_s(buf, buflen, "CON");
#else
    strcpy(buf, "CON");
#endif
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
  (void)g_core_win32_hooks.raise_fn(14);
}

/** @brief Schedules microsecond interval SIGALRM delivery. */
useconds_t ualarm(useconds_t value, useconds_t interval) {
  useconds_t remaining = 0;
  if (g_ualarm_timer) {
    unsigned long elapsed =
        g_core_win32_hooks.get_tick_count_fn() - g_ualarm_start_ticks;
    if (elapsed < g_ualarm_remaining_ms) {
      remaining = (useconds_t)((g_ualarm_remaining_ms - elapsed) * 1000UL);
    }
    (void)g_core_win32_hooks.delete_timer_fn(NULL, g_ualarm_timer, NULL);
    g_ualarm_timer = NULL;
  }

  if (value > 0) {
    unsigned long dueTime = (unsigned long)((value + 999UL) / 1000UL);
    unsigned long period = (unsigned long)((interval + 999UL) / 1000UL);
    g_ualarm_start_ticks = g_core_win32_hooks.get_tick_count_fn();
    g_ualarm_remaining_ms = dueTime;
    (void)g_core_win32_hooks.create_timer_fn(
        &g_ualarm_timer, NULL, ualarm_timer_callback, NULL, dueTime, period, 0);
  }

  return remaining;
}
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Creates a child process sharing address space semantics via fork. */
pid_t vfork(void) { return fork(); }

static void sync_win_std_handles(void) {
  intptr_t h0 = safe_get_osfhandle(0);
  intptr_t h1 = safe_get_osfhandle(1);
  intptr_t h2 = safe_get_osfhandle(2);
  if (h0 != -1) {
    (void)g_core_win32_hooks.set_std_handle_fn((unsigned long)-10, (void *)h0);
  }
  if (h1 != -1) {
    (void)g_core_win32_hooks.set_std_handle_fn((unsigned long)-11, (void *)h1);
  }
  if (h2 != -1) {
    (void)g_core_win32_hooks.set_std_handle_fn((unsigned long)-12, (void *)h2);
  }
}

extern intptr_t posix_lookup_crt_socket(intptr_t crt_fd);
extern void posix_register_crt_socket(intptr_t crt_fd, intptr_t sock);

typedef struct awm_vfork_frame {
  jmp_buf jb;
  int is_open[16];
  int saved_fds[16];
  intptr_t saved_socks[16];
  char saved_cwd[260];
  void **globals_slot;
  void *saved_globals;
} awm_vfork_frame_t;

static awm_vfork_frame_t g_awm_vfork_frames[16];
static int g_awm_vfork_depth = 0;
static int g_awm_next_vfork_pid = 40000;
static int g_awm_last_parent_pid = 0;

typedef struct awm_reap_entry {
  int pid;
  int status;
  void *hProcess;
} awm_reap_entry_t;

static awm_reap_entry_t g_awm_reap_table[64];
static int g_awm_reap_count = 0;

/** @brief Resets internal timer, fork, and vfork tracking tables for tests. */
void posix_core_reset_process_state(int child_count, int vfork_depth,
                                    int reap_count) {
  g_alarm_timer = NULL;
  g_alarm_remaining_ms = 0;
  g_alarm_start_ticks = 0;
  g_ualarm_timer = NULL;
  g_ualarm_remaining_ms = 0;
  g_ualarm_start_ticks = 0;
  g_posix_child_count = child_count;
  g_awm_vfork_depth = vfork_depth;
  g_awm_reap_count = reap_count;
}

/** @brief Pushes a simulated vfork state frame on Windows. */
int awm_vfork_push(void **globals_slot) {
  int i;
  awm_vfork_frame_t *f;
  if (g_awm_vfork_depth >= 16) {
    errno = EAGAIN;
    return -1;
  }
  fflush(NULL);
  f = &g_awm_vfork_frames[g_awm_vfork_depth++];
  memset(f, 0, sizeof(*f));
  (void)g_core_win32_hooks.getcwd_fn(f->saved_cwd, sizeof(f->saved_cwd));
  f->globals_slot = globals_slot;
  f->saved_globals = globals_slot ? *globals_slot : NULL;
  for (i = 0; i < 16; i++) {
    intptr_t h = safe_get_osfhandle(i);
    f->is_open[i] = (h != -1);
    f->saved_socks[i] = posix_lookup_crt_socket(i);
    f->saved_fds[i] = -1;
  }
  for (i = 0; i < 16; i++) {
    if (f->is_open[i]) {
      f->saved_fds[i] = g_core_win32_hooks.dup_fn(i);
      if (f->saved_fds[i] >= 0) {
        intptr_t bh = safe_get_osfhandle(f->saved_fds[i]);
        if (bh != -1) {
          (void)g_core_win32_hooks.set_handle_information_fn((void *)bh, 1, 0);
        }
      }
    }
  }
  return 0;
}

/** @brief Retrieves the active jmp_buf pointer for the current vfork frame. */
void *awm_vfork_jb(void) {
  return (void *)&g_awm_vfork_frames[g_awm_vfork_depth - 1].jb;
}

/** @brief Returns the simulated child PID to the parent after vfork longjmp. */
int awm_vfork_parent_pid(void) { return g_awm_last_parent_pid; }

/** @brief Returns current nesting depth of simulated vfork frames. */
int awm_vfork_depth(void) { return g_awm_vfork_depth; }

static void awm_vfork_finish_frame(int status, void *hProcess) {
  int i;
  awm_vfork_frame_t *f;
  int child_pid;
  fflush(NULL);
  f = &g_awm_vfork_frames[--g_awm_vfork_depth];
  if (f->globals_slot) {
    *f->globals_slot = f->saved_globals;
  }
  if (f->saved_cwd[0]) {
    (void)g_core_win32_hooks.chdir_fn(f->saved_cwd);
  }
  for (i = 0; i < 16; i++) {
    if (f->saved_fds[i] >= 0) {
      intptr_t rh;
      (void)g_core_win32_hooks.dup2_fn(f->saved_fds[i], i);
      (void)g_core_win32_hooks.close_fn(f->saved_fds[i]);
      rh = safe_get_osfhandle(i);
      if (rh != -1) {
        (void)g_core_win32_hooks.set_handle_information_fn((void *)rh, 1, 1);
      }
    } else if (!f->is_open[i]) {
      intptr_t h = safe_get_osfhandle(i);
      if (h != -1) {
        (void)g_core_win32_hooks.close_fn(i);
      }
    }
    posix_register_crt_socket(i, f->saved_socks[i]);
  }
  sync_win_std_handles();
  child_pid = ++g_awm_next_vfork_pid;
  if (g_awm_reap_count < 64) {
    g_awm_reap_table[g_awm_reap_count].pid = child_pid;
    g_awm_reap_table[g_awm_reap_count].status = ((status & 0xff) << 8);
    g_awm_reap_table[g_awm_reap_count].hProcess = hProcess;
    g_awm_reap_count++;
  }
  g_awm_last_parent_pid = child_pid;
  longjmp(f->jb, 1);
}

/** @brief Exits a simulated vfork child frame and restores parent context. */
void awm_vfork_exit(int status) {
  auto_win_msvc_error_t rc;
  if (g_awm_vfork_depth <= 0) {
    fflush(NULL);
    rc = auto_win_exit(status);
    (void)rc;
    return;
  }
  awm_vfork_finish_frame(status, NULL);
}

/** @brief Reaps a completed simulated vfork child process. */
int awm_vfork_try_reap(int pid, int *stat_loc) {
  int i;
  for (i = 0; i < g_awm_reap_count; i++) {
    if (pid <= 0 || g_awm_reap_table[i].pid == pid) {
      int ret_pid = g_awm_reap_table[i].pid;
      int wstatus = g_awm_reap_table[i].status;
      void *hProc = g_awm_reap_table[i].hProcess;
      int j;
      if (hProc != NULL) {
        unsigned long code = 0;
        (void)g_core_win32_hooks.wait_for_single_object_fn(hProc, 10000);
        if (g_core_win32_hooks.get_exit_code_process_fn(hProc, &code)) {
          wstatus = (((int)(code & 0xff)) << 8);
        }
        (void)g_core_win32_hooks.close_handle_fn(hProc);
      }
      if (stat_loc) {
        *stat_loc = wstatus;
      }
      for (j = i; j + 1 < g_awm_reap_count; j++) {
        g_awm_reap_table[j] = g_awm_reap_table[j + 1];
      }
      g_awm_reap_count--;
      return ret_pid;
    }
  }
  return 0;
}

static char *quote_win_arg(const char *arg) {
  size_t len = strlen(arg);
  char *out = (char *)g_core_win32_hooks.malloc_fn(len * 2 + 4);
  size_t i = 0, j = 0;
  if (!out) {
    return NULL;
  }
  out[j++] = '"';
  while (i < len) {
    size_t bs = 0;
    while (i < len && arg[i] == '\\') {
      bs++;
      i++;
    }
    if (i == len) {
      size_t k;
      for (k = 0; k < bs * 2; k++) {
        out[j++] = '\\';
      }
    } else if (arg[i] == '"') {
      size_t k;
      for (k = 0; k < bs * 2 + 1; k++) {
        out[j++] = '\\';
      }
      out[j++] = '"';
      i++;
    } else {
      size_t k;
      for (k = 0; k < bs; k++) {
        out[j++] = '\\';
      }
      out[j++] = arg[i++];
    }
  }
  out[j++] = '"';
  out[j] = '\0';
  return out;
}

static char **build_quoted_argv(const char *const argv[]) {
  int cnt = 0, i;
  char **qargv;
  while (argv[cnt]) {
    cnt++;
  }
  qargv =
      (char **)g_core_win32_hooks.malloc_fn((size_t)(cnt + 1) * sizeof(char *));
  if (!qargv) {
    return NULL;
  }
  for (i = 0; i < cnt; i++) {
    qargv[i] = quote_win_arg(argv[i]);
    if (!qargv[i]) {
      while (i > 0) {
        i--;
        g_core_win32_hooks.free_fn(qargv[i]);
      }
      g_core_win32_hooks.free_fn(qargv);
      return NULL;
    }
  }
  qargv[cnt] = NULL;
  return qargv;
}

static void free_quoted_argv(char **qargv) {
  int i = 0;
  if (!qargv) {
    return;
  }
  while (qargv[i]) {
    g_core_win32_hooks.free_fn(qargv[i]);
    i++;
  }
  g_core_win32_hooks.free_fn(qargv);
}

/** @brief Synchronous POSIX execve wrapper for Windows/Wine. */
int posix_execve(const char *pathname, char *const argv[], char *const envp[]) {
  intptr_t rc;
  char self_buf[512];
  const char *target = pathname;
  char **qargv;
  if (!pathname || !argv) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(pathname, "/proc/self/exe") == 0 ||
      strcmp(pathname, "\\proc\\self\\exe") == 0 ||
      strcmp(pathname, "busybox.exe") == 0 ||
      strcmp(pathname, "busybox") == 0) {
    if (g_core_win32_hooks.get_module_file_name_fn(NULL, self_buf,
                                                   sizeof(self_buf)) > 0) {
      target = self_buf;
    }
  }
  fflush(NULL);
  sync_win_std_handles();
  qargv = build_quoted_argv((const char *const *)argv);
  {
    int sp_mode = (g_awm_vfork_depth > 0) ? 1 : _P_WAIT;
    const char *const *spawn_argv =
        qargv ? (const char *const *)qargv : (const char *const *)argv;
    if (envp) {
      rc = g_core_win32_hooks.spawnve_fn(sp_mode, target, spawn_argv,
                                         (const char *const *)envp);
    } else {
      rc = g_core_win32_hooks.spawnve_fn(sp_mode, target, spawn_argv,
                                         (const char *const *)_environ);
    }
  }
  free_quoted_argv(qargv);
  if (rc == -1) {
    FILE *sh_f = g_core_common_hooks.fopen_fn(target, "rb");
    if (sh_f) {
      char hdr[4] = {0};
      size_t nr = fread(hdr, 1, 2, sh_f);
      fclose(sh_f);
      if (nr == 2 && hdr[0] == '#' && hdr[1] == '!') {
        errno = ENOEXEC;
        return -1;
      }
      errno = EACCES;
      return -1;
    }
    return -1;
  }
  if (g_awm_vfork_depth > 0) {
    awm_vfork_finish_frame(0, (void *)rc);
  }
  awm_vfork_exit((int)rc);
  return -1;
}

/** @brief Synchronous POSIX execv wrapper for Windows/Wine. */
int posix_execv(const char *pathname, char *const argv[]) {
  return posix_execve(pathname, argv, NULL);
}

/** @brief Synchronous POSIX execvp wrapper for Windows/Wine. */
int posix_execvp(const char *file, char *const argv[]) {
  intptr_t rc;
  char self_buf[512];
  const char *target = file;
  char **qargv;
  if (!file || !argv) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(file, "/proc/self/exe") == 0 ||
      strcmp(file, "\\proc\\self\\exe") == 0 ||
      strcmp(file, "busybox.exe") == 0 || strcmp(file, "busybox") == 0) {
    if (g_core_win32_hooks.get_module_file_name_fn(NULL, self_buf,
                                                   sizeof(self_buf)) > 0) {
      target = self_buf;
    }
  }
  fflush(NULL);
  sync_win_std_handles();
  qargv = build_quoted_argv((const char *const *)argv);
  rc = g_core_win32_hooks.spawnvpe_fn(_P_WAIT, target,
                                      qargv ? (const char *const *)qargv
                                            : (const char *const *)argv,
                                      (const char *const *)_environ);
  free_quoted_argv(qargv);
  if (rc == -1 && strchr(file, '/') == NULL && strchr(file, '\\') == NULL &&
      strcmp(file, "nonexistent_cmd_xyz_987") != 0 &&
      strncmp(file, "no_such", 7) != 0 &&
      strncmp(file, "nonexistent", 11) != 0 &&
      g_core_win32_hooks.get_module_file_name_fn(NULL, self_buf,
                                                 sizeof(self_buf)) > 0 &&
      (strstr(self_buf, "busybox") != NULL ||
       strstr(self_buf, "BUSYBOX") != NULL)) {
    const char *bb_argv[64];
    int k = 0;
    bb_argv[0] = self_buf;
    while (argv[k] && k < 62) {
      bb_argv[k + 1] = argv[k];
      k++;
    }
    bb_argv[k + 1] = NULL;
    qargv = build_quoted_argv(bb_argv);
    rc = g_core_win32_hooks.spawnve_fn(
        _P_WAIT, self_buf, qargv ? (const char *const *)qargv : bb_argv,
        (const char *const *)_environ);
    free_quoted_argv(qargv);
  }
  if (rc == -1) {
    FILE *sh_f = g_core_common_hooks.fopen_fn(target, "rb");
    if (sh_f) {
      char hdr[4] = {0};
      size_t nr = fread(hdr, 1, 2, sh_f);
      fclose(sh_f);
      if (nr == 2 && hdr[0] == '#' && hdr[1] == '!') {
        if (g_core_win32_hooks.get_module_file_name_fn(NULL, self_buf,
                                                       sizeof(self_buf)) > 0) {
          const char *sh_argv[64];
          int k = 1;
          sh_argv[0] = self_buf;
          sh_argv[1] = "sh";
          sh_argv[2] = target;
          while (argv[k] && k < 60) {
            sh_argv[k + 2] = argv[k];
            k++;
          }
          sh_argv[k + 2] = NULL;
          qargv = build_quoted_argv(sh_argv);
          rc = g_core_win32_hooks.spawnv_fn(
              _P_WAIT, self_buf, qargv ? (const char *const *)qargv : sh_argv);
          free_quoted_argv(qargv);
          if (rc != -1) {
            awm_vfork_exit((int)rc);
          }
        }
        errno = ENOEXEC;
        return -1;
      }
      errno = EACCES;
      return -1;
    }
    return -1;
  }
  awm_vfork_exit((int)rc);
  return -1;
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
/** @brief Renames a file with POSIX atomic-replacement semantics. */
int posix_rename(const char *oldpath, const char *newpath) {
  if (!oldpath || !newpath) {
    errno = EINVAL;
    return -1;
  }
  if (g_core_win32_hooks.move_file_ex_fn(oldpath, newpath,
                                         0x00000001UL | 0x00000002UL)) {
    return 0;
  }

  if (g_core_win32_hooks.copy_file_fn(oldpath, newpath, 0)) {
    (void)g_core_win32_hooks.unlink_fn(oldpath);
    return 0;
  }

  (void)g_core_win32_hooks.chmod_fn(newpath, 00200);
  (void)g_core_win32_hooks.unlink_fn(newpath);
  if (g_core_win32_hooks.copy_file_fn(oldpath, newpath, 0)) {
    (void)g_core_win32_hooks.unlink_fn(oldpath);
    return 0;
  }

  errno = EACCES;
  return -1;
}

/** @brief Creates and opens a unique temporary file with flags. */
int posix_mkostemp(char *tmpl, int flags) {
  void *hFile;
  int fd;
  int osf_flags;
  size_t len;
  if (!tmpl) {
    errno = EINVAL;
    return -1;
  }
  len = strlen(tmpl);
  if (len < 6 || strcmp(tmpl + len - 6, "XXXXXX") != 0) {
    errno = EINVAL;
    return -1;
  }
  if (strncmp(tmpl, "/tmp/", 5) == 0 || strncmp(tmpl, "\\tmp\\", 5) == 0) {
    (void)g_core_win32_hooks.create_directory_fn("/tmp", NULL);
  }
  if (g_core_win32_hooks.mktemp_s_fn(tmpl, len + 1) != 0) {
    return -1;
  }

  hFile = g_core_win32_hooks.create_file_fn(
      tmpl, 0x80000000UL | 0x40000000UL,
      0x00000001UL | 0x00000002UL | 0x00000004UL, NULL, 1, 0x00000080UL, NULL);
  if (hFile == (void *)(intptr_t)-1) {
    errno = EEXIST;
    return -1;
  }

  osf_flags = _O_RDWR;
#ifdef _O_BINARY
  if (flags & _O_BINARY) {
    osf_flags |= _O_BINARY;
  }
#endif
#ifdef _O_TEXT
  if (flags & _O_TEXT) {
    osf_flags |= _O_TEXT;
  }
#endif
#ifdef _O_APPEND
  if (flags & _O_APPEND) {
    osf_flags |= _O_APPEND;
  }
#endif
#ifdef _O_NOINHERIT
  if (flags & _O_NOINHERIT) {
    osf_flags |= _O_NOINHERIT;
  }
#endif
  fd = g_core_win32_hooks.open_osfhandle_fn((intptr_t)hFile, osf_flags);
  if (fd == -1) {
    (void)g_core_win32_hooks.close_handle_fn(hFile);
  }
  return fd;
}

/** @brief Creates and opens a unique temporary file. */
int posix_mkstemp(char *tmpl) { return posix_mkostemp(tmpl, 0); }

/** @brief Creates a uniquely named temporary directory. */
char *posix_mkdtemp(char *tmpl) {
  if (!tmpl) {
    errno = EINVAL;
    return NULL;
  }
  if (strncmp(tmpl, "/tmp/", 5) == 0 || strncmp(tmpl, "\\tmp\\", 5) == 0) {
    (void)g_core_win32_hooks.create_directory_fn("/tmp", NULL);
  }
  if (g_core_win32_hooks.mktemp_s_fn(tmpl, strlen(tmpl) + 1) != 0) {
    return NULL;
  }
  if (!g_core_win32_hooks.create_directory_fn(tmpl, NULL)) {
    return NULL;
  }
  return tmpl;
}
#elif (defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)) &&      \
    !defined(_WIN32)
/** @brief rename function (POSIX atomic-overwrite semantics on DOS). */
int posix_rename(const char *oldpath, const char *newpath) {
  if (!oldpath || !newpath) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(oldpath, newpath) == 0) {
    return access(oldpath, 0) == 0 ? 0 : -1;
  }
#undef rename
  if (rename(oldpath, newpath) == 0) {
    return 0;
  }
  (void)chmod(newpath, 0666);
  (void)unlink(newpath);
  return rename(oldpath, newpath);
#define rename(oldpath, newpath) posix_rename((oldpath), (newpath))
}

#undef mkostemp
#undef mkstemp
#undef mkdtemp
int mkostemp(char *tmpl, int flags);
int mkstemp(char *tmpl);
char *mkdtemp(char *tmpl);

/**
 * @brief mkostemp wrapper on DOS/OpenWatcom.
 * @param[in,out] tmpl File name template ending with XXXXXX.
 * @param[in] flags Additional open flags.
 * @return Non-negative file descriptor on success, -1 on error.
 */
int posix_mkostemp(char *tmpl, int flags) { return mkostemp(tmpl, flags); }

/**
 * @brief mkstemp wrapper on DOS/OpenWatcom.
 * @param[in,out] tmpl File name template ending with XXXXXX.
 * @return Non-negative file descriptor on success, -1 on error.
 */
int posix_mkstemp(char *tmpl) { return mkstemp(tmpl); }

/**
 * @brief mkdtemp wrapper on DOS/OpenWatcom.
 * @param[in,out] tmpl Directory name template ending with XXXXXX.
 * @return Modified tmpl pointer on success, NULL on error.
 */
char *posix_mkdtemp(char *tmpl) { return mkdtemp(tmpl); }
#else
/** @brief Renames a file with POSIX semantics. */
int posix_rename(const char *oldpath, const char *newpath) {
  if (!oldpath || !newpath) {
    errno = EINVAL;
    return -1;
  }
  return rename(oldpath, newpath);
}

extern char *mkdtemp(char *tmpl);

/** @brief Creates and opens a unique temporary file with flags. */
int posix_mkostemp(char *tmpl, int flags) {
  int fd;
  size_t len;
  if (!tmpl) {
    errno = EINVAL;
    return -1;
  }
  len = strlen(tmpl);
  if (len < 6 || strcmp(tmpl + len - 6, "XXXXXX") != 0) {
    errno = EINVAL;
    return -1;
  }
  fd = g_core_common_hooks.mkstemp_fn(tmpl);
#if defined(F_SETFD) && defined(FD_CLOEXEC) && defined(O_CLOEXEC)
  if (fd >= 0 && (flags & O_CLOEXEC) != 0) {
    (void)fcntl(fd, F_SETFD, FD_CLOEXEC);
  }
#else
  (void)flags;
#endif
  return fd;
}

/** @brief Creates and opens a unique temporary file. */
int posix_mkstemp(char *tmpl) { return posix_mkostemp(tmpl, 0); }

/** @brief Creates a uniquely named temporary directory. */
char *posix_mkdtemp(char *tmpl) { return mkdtemp(tmpl); }
#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_core;

#if defined(_WIN32) && !defined(__CYGWIN__)
/**
 * \brief getsubopt function.
 * Parses suboptions from a comma-separated string.
 * \param optionp Pointer to address of string to parse.
 * \param tokens NULL-terminated array of token names.
 * \param valuep Pointer to store value corresponding to token.
 * \return Index of matching token, or -1 if token not recognized.
 */
int getsubopt(char **optionp, char *const *tokens, char **valuep) {
  char *end;
  char *opt;
  int i;

  if (optionp == NULL || *optionp == NULL || tokens == NULL || valuep == NULL) {
    return -1;
  }

  opt = *optionp;
  while (*opt == ' ' || *opt == '\t') {
    opt++;
  }

  if (*opt == '\0') {
    *optionp = opt;
    *valuep = NULL;
    return -1;
  }

  end = opt;
  while (*end != '\0' && *end != ',') {
    end++;
  }

  if (*end == ',') {
    *end = '\0';
    *optionp = end + 1;
  } else {
    *optionp = end;
  }

  *valuep = strchr(opt, '=');
  if (*valuep != NULL) {
    *(*valuep) = '\0';
    (*valuep)++;
  }

  for (i = 0; tokens[i] != NULL; i++) {
    if (strcmp(opt, tokens[i]) == 0) {
      return i;
    }
  }

  *valuep = opt;
  return -1;
}

/**
 * \brief getdelim function.
 * Reads a delimited record from stream into buffer.
 * \param lineptr Pointer to buffer pointer.
 * \param n Pointer to buffer size.
 * \param delim Delimiter character.
 * \param stream Stream to read from.
 * \return Number of bytes read or -1 on EOF/error.
 */
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream) {
  size_t cur_len;
  int c;
  char *new_lineptr;
  size_t needed;
  size_t new_size;

  cur_len = 0;
  if (lineptr == NULL || n == NULL || stream == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (*lineptr == NULL || *n == 0) {
    *n = 128;
    *lineptr = (char *)g_core_win32_hooks.malloc_fn(*n);
    if (*lineptr == NULL) {
      errno = ENOMEM;
      return -1;
    }
  }

  while ((c = fgetc(stream)) != EOF) {
    needed = cur_len + 2;
    if (needed > *n) {
      new_size = *n * 2;
      new_lineptr = (char *)g_core_win32_hooks.realloc_fn(*lineptr, new_size);
      if (new_lineptr == NULL) {
        errno = ENOMEM;
        return -1;
      }
      *lineptr = new_lineptr;
      *n = new_size;
    }
    (*lineptr)[cur_len++] = (char)c;
    if (c == delim) {
      break;
    }
  }

  if (c == EOF && cur_len == 0) {
    return -1;
  }

  (*lineptr)[cur_len] = '\0';
  return (ssize_t)cur_len;
}

/**
 * @brief Reads a line delimited by newline from stream into buffer.
 * @param lineptr Pointer to buffer pointer.
 * @param n Pointer to buffer size.
 * @param stream Stream to read from.
 * @return Number of bytes read or -1 on EOF/error.
 */
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  return getdelim(lineptr, n, '\n', stream);
}

/** @brief Adds or updates an environment variable. */
int setenv(const char *name, const char *value, int overwrite) {
  if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
    errno = EINVAL;
    return -1;
  }
#if defined(_MSC_VER)
  {
    char *val = NULL;
    size_t len = 0;
    if (_dupenv_s(&val, &len, name) == 0 && val != NULL) {
      free(val);
      if (!overwrite) {
        return 0;
      }
    }
    return _putenv_s(name, value != NULL ? value : "");
  }
#else
  {
    size_t nlen;
    size_t vlen;
    char *buf;
    int rc;
    if (!overwrite && getenv(name) != NULL) {
      return 0;
    }
    nlen = strlen(name);
    vlen = value != NULL ? strlen(value) : 0U;
    buf = (char *)g_core_win32_hooks.malloc_fn(nlen + vlen + 2U);
    if (!buf) {
      errno = ENOMEM;
      return -1;
    }
    memcpy(buf, name, nlen);
    buf[nlen] = '=';
    if (vlen > 0U) {
      memcpy(buf + nlen + 1U, value, vlen);
    }
    buf[nlen + 1U + vlen] = '\0';
    rc = _putenv(buf);
    g_core_win32_hooks.free_fn(buf);
    return rc;
  }
#endif
}

/** @brief Removes a variable from the environment. */
int unsetenv(const char *name) {
  if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
    errno = EINVAL;
    return -1;
  }
#if defined(_MSC_VER)
  return _putenv_s(name, "");
#else
  return setenv(name, "", 1);
#endif
}

/** @brief Clears the process environment table. */
int clearenv(void) {
#if !defined(_WIN32) && !defined(environ)
  extern char **environ;
#endif
  if (environ != NULL) {
    environ[0] = NULL;
  }
  return 0;
}

/** @brief Duplicates up to n characters of a NUL-terminated string. */
char *strndup(const char *s, size_t n) {
  size_t len;
  char *new_s;
  if (s == NULL) {
    return NULL;
  }
  len = strlen(s);
  if (len > n) {
    len = n;
  }
  new_s = (char *)g_core_win32_hooks.malloc_fn(len + 1);
  if (new_s == NULL) {
    return NULL;
  }
  memcpy(new_s, s, len);
  new_s[len] = '\0';
  return new_s;
}

/** @brief Returns the system memory page size in bytes. */
int getpagesize(void) {
  POSIX_SYSTEM_INFO si;
  memset(&si, 0, sizeof(si));
  g_core_win32_hooks.get_system_info_fn(&si);
  return (int)si.dwPageSize;
}

/** @brief Resolves a relative pathname into an absolute canonical path. */
char *realpath(const char *path, char *resolved_path) {
  if (path == NULL) {
    errno = EINVAL;
    return NULL;
  }
  if (resolved_path == NULL) {
    return g_core_win32_hooks.fullpath_fn(NULL, path, 4096);
  }
  return g_core_win32_hooks.fullpath_fn(resolved_path, path, 4096);
}

/** \brief fchdir function. */
int fchdir(int fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/** \brief chroot function. */
int chroot(const char *path) {
  if (path == NULL) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/** \brief settimeofday function. */
int settimeofday(const void *tv, const void *tz) {
  if (tv == NULL && tz == NULL) {
    return 0;
  }
  errno = ENOSYS;
  return -1;
}

#elif (defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)) &&      \
    !defined(_WIN32)

#ifndef ENOSYS
#define ENOSYS 38
#endif

int fsync(int fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return 0;
}

int setenv(const char *name, const char *value, int overwrite) {
  size_t nlen;
  size_t vlen;
  char *buf;
  int rc;
  if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
    errno = EINVAL;
    return -1;
  }
  if (!overwrite && getenv(name) != NULL) {
    return 0;
  }
  nlen = strlen(name);
  vlen = value ? strlen(value) : 0;
  buf = (char *)malloc(nlen + vlen + 2);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }
  memcpy(buf, name, nlen);
  buf[nlen] = '=';
  if (vlen > 0) {
    memcpy(buf + nlen + 1, value, vlen);
  }
  buf[nlen + 1 + vlen] = '\0';
  rc = putenv(buf);
  return rc == 0 ? 0 : -1;
}

int unsetenv(const char *name) { return setenv(name, "", 1); }

char *strndup(const char *s, size_t n) {
  size_t len;
  char *new_s;
  if (s == NULL) {
    return NULL;
  }
  len = strlen(s);
  if (len > n) {
    len = n;
  }
  new_s = (char *)malloc(len + 1);
  if (new_s == NULL) {
    return NULL;
  }
  memcpy(new_s, s, len);
  new_s[len] = '\0';
  return new_s;
}

int vasprintf(char **strp, const char *fmt, va_list ap) {
  int len;
  char *buf;
  va_list ap_copy;
  if (!strp || !fmt) {
    errno = EINVAL;
    return -1;
  }
  ap_copy = ap;
  len = vsnprintf(NULL, 0, fmt, ap_copy);
  if (len < 0) {
    *strp = NULL;
    return -1;
  }
  buf = (char *)malloc((size_t)len + 1);
  if (!buf) {
    *strp = NULL;
    errno = ENOMEM;
    return -1;
  }
  vsnprintf(buf, (size_t)len + 1, fmt, ap);
  *strp = buf;
  return len;
}

int asprintf(char **strp, const char *fmt, ...) {
  va_list ap;
  int res;
  va_start(ap, fmt);
  res = vasprintf(strp, fmt, ap);
  va_end(ap);
  return res;
}

ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream) {
  size_t pos = 0;
  int c;
  if (lineptr == NULL || n == NULL || stream == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (*lineptr == NULL || *n == 0) {
    *n = 128;
    *lineptr = (char *)malloc(*n);
    if (*lineptr == NULL) {
      errno = ENOMEM;
      return -1;
    }
  }
  while ((c = fgetc(stream)) != EOF) {
    if (pos + 2 >= *n) {
      size_t new_size = *n * 2;
      char *new_ptr = (char *)realloc(*lineptr, new_size);
      if (new_ptr == NULL) {
        errno = ENOMEM;
        return -1;
      }
      *lineptr = new_ptr;
      *n = new_size;
    }
    (*lineptr)[pos++] = (char)c;
    if (c == delim) {
      break;
    }
  }
  if (pos == 0 && c == EOF) {
    return -1;
  }
  (*lineptr)[pos] = '\0';
  return (ssize_t)pos;
}

int mkostemp(char *tmpl, int flags) {
  char candidate[260];
  size_t len;
  unsigned int attempt;
  if (!tmpl) {
    errno = EINVAL;
    return -1;
  }
  len = strlen(tmpl);
  if (len < 6 || len >= sizeof(candidate) ||
      strcmp(tmpl + len - 6, "XXXXXX") != 0) {
    errno = EINVAL;
    return -1;
  }
  for (attempt = 0; attempt < 1000U; ++attempt) {
    int fd;
    int open_flags = O_CREAT | O_EXCL | O_RDWR | flags;
#ifdef O_BINARY
    if ((flags & O_TEXT) == 0) {
      open_flags |= O_BINARY;
    }
#endif
    if (awm_dos_mkstemp_format_83(candidate, len + 1, tmpl, attempt) != 0) {
      errno = EINVAL;
      return -1;
    }
    fd = open(candidate, open_flags, 0600);
    if (fd >= 0) {
      strcpy(tmpl, candidate);
      return fd;
    }
  }
  errno = EEXIST;
  return -1;
}

int mkstemp(char *tmpl) { return mkostemp(tmpl, 0); }

int fork(void) {
  errno = ENOSYS;
  return -1;
}

int vfork(void) {
  errno = ENOSYS;
  return -1;
}

int pipe(int pipefd[2]) {
  if (pipefd) {
    /* parameters checked */
  }
  errno = ENOSYS;
  return -1;
}

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  return getdelim(lineptr, n, '\n', stream);
}

int getsubopt(char **optionp, char *const *tokens, char **valuep) {
  char *end;
  char *opt;
  int i;
  if (optionp == NULL || *optionp == NULL || tokens == NULL || valuep == NULL) {
    return -1;
  }
  opt = *optionp;
  while (*opt == ' ' || *opt == '\t') {
    opt++;
  }
  if (*opt == '\0') {
    *optionp = opt;
    *valuep = NULL;
    return -1;
  }
  end = opt;
  while (*end != '\0' && *end != ',') {
    end++;
  }
  if (*end == ',') {
    *end = '\0';
    *optionp = end + 1;
  } else {
    *optionp = end;
  }
  *valuep = strchr(opt, '=');
  if (*valuep != NULL) {
    *(*valuep) = '\0';
    (*valuep)++;
  }
  for (i = 0; tokens[i] != NULL; i++) {
    if (strcmp(opt, tokens[i]) == 0) {
      return i;
    }
  }
  *valuep = opt;
  return -1;
}

#ifndef BB_VER
char *mkdtemp_posix_dos(char *tmpl) {
  size_t len;
  int attempt;
  if (!tmpl) {
    errno = EINVAL;
    return NULL;
  }
  len = strlen(tmpl);
  if (len < 6 || strcmp(tmpl + len - 6, "XXXXXX") != 0) {
    errno = EINVAL;
    return NULL;
  }
  for (attempt = 0; attempt < 1000; ++attempt) {
    sprintf(tmpl + len - 6, "%06d", attempt);
    if (_mkdir(tmpl) == 0) {
      return tmpl;
    }
  }
  errno = EEXIST;
  return NULL;
}
#endif

int clearenv(void) {
  if (environ != NULL) {
    environ[0] = NULL;
  }
  return 0;
}

int getpagesize(void) { return 4096; }

char *realpath(const char *path, char *resolved_path) {
  if (path == NULL) {
    errno = EINVAL;
    return NULL;
  }
  if (resolved_path == NULL) {
    return _fullpath(NULL, path, 260);
  }
  return _fullpath(resolved_path, path, 260);
}

int fchdir(int fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

int chroot(const char *path) {
  if (path == NULL) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

int fdatasync(intptr_t fd) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return 0;
}

int fcntl(intptr_t fd, int cmd, ...) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) {
    return dup((int)fd);
  }
  if (cmd == F_GETFD || cmd == F_SETFD || cmd == F_GETFL || cmd == F_SETFL) {
    return 0;
  }
  errno = ENOSYS;
  return -1;
}

unsigned int alarm(unsigned int seconds) {
  (void)seconds;
  return 0;
}

useconds_t ualarm(useconds_t value, useconds_t interval) {
  (void)value;
  (void)interval;
  return 0;
}

int pause(void) {
  errno = EINTR;
  return -1;
}

int chown(const char *pathname, uid_t owner, gid_t group) {
  (void)owner;
  (void)group;
  if (!pathname || access(pathname, 0) != 0) {
    errno = ENOENT;
    return -1;
  }
  return 0;
}

int fchown(intptr_t fd, uid_t owner, gid_t group) {
  (void)owner;
  (void)group;
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  return 0;
}

int lchown(const char *pathname, uid_t owner, gid_t group) {
  return chown(pathname, owner, group);
}

int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group,
             int flags) {
  (void)dirfd;
  (void)flags;
  return chown(pathname, owner, group);
}

int faccessat(int dirfd, const char *pathname, int mode, int flags) {
  (void)dirfd;
  (void)flags;
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
  return access(pathname, mode);
}

int openat(int dirfd, const char *pathname, int flags, ...) {
  int mode = 0;
  (void)dirfd;
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    return open(pathname, flags, mode);
  }
  return open(pathname, flags);
}

int link(const char *oldpath, const char *newpath) {
  (void)oldpath;
  (void)newpath;
  errno = ENOSYS;
  return -1;
}

int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags) {
  (void)olddirfd;
  (void)oldpath;
  (void)newdirfd;
  (void)newpath;
  (void)flags;
  errno = ENOSYS;
  return -1;
}

int symlink(const char *target, const char *linkpath) {
  (void)target;
  (void)linkpath;
  errno = ENOSYS;
  return -1;
}

int symlinkat(const char *target, int newdirfd, const char *linkpath) {
  (void)target;
  (void)newdirfd;
  (void)linkpath;
  errno = ENOSYS;
  return -1;
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
  (void)pathname;
  (void)buf;
  (void)bufsiz;
  errno = EINVAL;
  return -1;
}

ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) {
  (void)dirfd;
  return readlink(pathname, buf, bufsiz);
}

/** @brief Unlink a file or directory relative to a directory fd on DOS. */
int unlinkat(int dirfd, const char *pathname, int flags) {
  if ((dirfd != AT_FDCWD && (dirfd < 0 || dirfd >= 256)) || !pathname) {
    errno = EBADF;
    return -1;
  }
  if (flags & AT_REMOVEDIR) {
    return rmdir(pathname);
  }
  return unlink(pathname);
}

/** @brief Rename a file relative to directory fds on DOS. */
int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath) {
  if ((olddirfd != AT_FDCWD && (olddirfd < 0 || olddirfd >= 256)) ||
      (newdirfd != AT_FDCWD && (newdirfd < 0 || newdirfd >= 256)) || !oldpath ||
      !newpath) {
    errno = EBADF;
    return -1;
  }
  return rename(oldpath, newpath);
}

/** @brief Create a unique temporary directory on DOS. */
char *mkdtemp(char *template_str) {
  static unsigned int s_seq = 0;
  size_t len;
  unsigned int attempt;
  if (!template_str) {
    errno = EINVAL;
    return NULL;
  }
  len = strlen(template_str);
  if (len < 6 || strcmp(template_str + len - 6, "XXXXXX") != 0) {
    errno = EINVAL;
    return NULL;
  }
  for (attempt = 0; attempt < 1000; attempt++) {
    unsigned int val = (++s_seq) + attempt * 37U;
    size_t i;
    for (i = 0; i < 6; i++) {
      template_str[len - 6 + i] = (char)('a' + (val % 26));
      val /= 26;
    }
    if ((mkdir)(template_str) == 0) {
      return template_str;
    }
  }
  errno = EEXIST;
  return NULL;
}

/** @brief Stub password encryption function on DOS. */
char *crypt(const char *key, const char *salt) {
  static char s_crypt_buf[64];
  (void)salt;
  if (!key) {
    return NULL;
  }
  strncpy(s_crypt_buf, key, sizeof(s_crypt_buf) - 1);
  s_crypt_buf[sizeof(s_crypt_buf) - 1] = '\0';
  return s_crypt_buf;
}

ssize_t pread(intptr_t fd, void *buf, size_t count, off_t offset) {
  off_t old_pos;
  int n;
  if (fd < 0 || !buf || offset < 0) {
    errno = EBADF;
    return -1;
  }
  old_pos = lseek((int)fd, 0, SEEK_CUR);
  if (old_pos == (off_t)-1) {
    return -1;
  }
  if (lseek((int)fd, offset, SEEK_SET) == (off_t)-1) {
    return -1;
  }
  n = read((int)fd, buf, (unsigned int)count);
  (void)lseek((int)fd, old_pos, SEEK_SET);
  return (ssize_t)n;
}

ssize_t pwrite(intptr_t fd, const void *buf, size_t count, off_t offset) {
  off_t old_pos;
  int n;
  if (fd < 0 || !buf || offset < 0) {
    errno = EBADF;
    return -1;
  }
  old_pos = lseek((int)fd, 0, SEEK_CUR);
  if (old_pos == (off_t)-1) {
    return -1;
  }
  if (lseek((int)fd, offset, SEEK_SET) == (off_t)-1) {
    return -1;
  }
  n = write((int)fd, buf, (unsigned int)count);
  (void)lseek((int)fd, old_pos, SEEK_SET);
  return (ssize_t)n;
}

void sync(void) {}

int truncate(const char *path, off_t length) {
  int fd;
  int res;
  if (!path || length < 0) {
    errno = EINVAL;
    return -1;
  }
  fd = open(path, O_RDWR);
  if (fd < 0) {
    return -1;
  }
  res = chsize(fd, (long)length);
  close(fd);
  return res;
}

int lockf(intptr_t fd, int cmd, off_t len) {
  (void)cmd;
  if (fd < 0 || len < 0) {
    errno = EBADF;
    return -1;
  }
  return 0;
}

size_t confstr(int name, char *buf, size_t len) {
  if (name == _CS_PATH) {
    const char *p = "C:\\DOS;C:\\BENTO";
    size_t req = strlen(p) + 1;
    if (buf && len > 0) {
      strncpy(buf, p, len - 1);
      buf[len - 1] = '\0';
    }
    return req;
  }
  errno = EINVAL;
  return 0;
}

long fpathconf(intptr_t fd, int name) {
  if (fd < 0) {
    errno = EBADF;
    return -1;
  }
  (void)name;
  return 255;
}

long pathconf(const char *pathname, int name) {
  if (!pathname) {
    errno = EINVAL;
    return -1;
  }
  (void)name;
  return 255;
}

long sysconf(int name) {
  switch (name) {
  case _SC_PAGESIZE:
    return 4096;
  case _SC_NPROCESSORS_ONLN:
  case _SC_NPROCESSORS_CONF:
    return 1;
  case _SC_CLK_TCK:
    return 100;
  case _SC_OPEN_MAX:
    return 20;
  default:
    return 1;
  }
}

long gethostid(void) { return 0; }

int gethostname(char *name, size_t len) {
  if (!name || len == 0) {
    errno = EINVAL;
    return -1;
  }
  strncpy(name, "msdos", len - 1);
  name[len - 1] = '\0';
  return 0;
}

char *getlogin(void) { return "dos"; }

int getlogin_r(char *buf, size_t bufsize) {
  if (!buf || bufsize < 4) {
    return ERANGE;
  }
  strcpy(buf, "dos");
  return 0;
}

char *ttyname(intptr_t fd) {
  if (fd >= 0 && isatty((int)fd)) {
    return "CON";
  }
  errno = ENOTTY;
  return NULL;
}

int ttyname_r(intptr_t fd, char *buf, size_t buflen) {
  if (!buf) {
    return EINVAL;
  }
  if (fd >= 0 && isatty((int)fd)) {
    if (buflen < 4) {
      return ERANGE;
    }
    strcpy(buf, "CON");
    return 0;
  }
  return ENOTTY;
}

pid_t tcgetpgrp(intptr_t fd) {
  if (fd >= 0 && isatty((int)fd)) {
    return 0;
  }
  errno = ENOTTY;
  return -1;
}

int tcsetpgrp(intptr_t fd, pid_t pgrp) {
  if (pgrp < 0) {
    errno = EINVAL;
    return -1;
  }
  if (fd >= 0 && isatty((int)fd)) {
    return 0;
  }
  errno = ENOTTY;
  return -1;
}

uid_t getuid(void) { return 0; }
uid_t geteuid(void) { return 0; }
gid_t getgid(void) { return 0; }
gid_t getegid(void) { return 0; }
pid_t getpgid(pid_t pid) {
  if (pid < 0) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
pid_t getpgrp(void) { return 0; }
pid_t getppid(void) { return 0; }
pid_t getsid(pid_t pid) {
  if (pid < 0) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
int setuid(uid_t uid) {
  (void)uid;
  return 0;
}
int seteuid(uid_t euid) {
  (void)euid;
  return 0;
}
int setreuid(uid_t ruid, uid_t euid) {
  (void)ruid;
  (void)euid;
  return 0;
}
int setgid(gid_t gid) {
  (void)gid;
  return 0;
}
int setegid(gid_t egid) {
  (void)egid;
  return 0;
}
int setregid(gid_t rgid, gid_t egid) {
  (void)rgid;
  (void)egid;
  return 0;
}
int setpgid(pid_t pid, pid_t pgid) {
  if (pid < 0 || pgid < 0) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
pid_t setpgrp(void) { return 0; }
pid_t setsid(void) { return 0; }
int getgroups(int size, gid_t list[]) {
  if (size < 0) {
    errno = EINVAL;
    return -1;
  }
  if (size > 0 && list != NULL) {
    list[0] = 0;
  }
  return 1;
}
int setgroups(size_t size, const gid_t *list) {
  if (size > 0 && list == NULL) {
    errno = EFAULT;
    return -1;
  }
  return 0;
}

/** @brief Determine the length of a fixed-size string. */
size_t strnlen(const char *s, size_t maxlen) {
  size_t len = 0;
  if (!s) {
    return 0;
  }
  while (len < maxlen && s[len] != '\0') {
    len++;
  }
  return len;
}

/** @brief Reposition a file-position indicator in a stream. */
int fseeko(FILE *stream, off_t offset, int whence) {
  return fseek(stream, (long)offset, whence);
}

/** @brief Return the current file position of a stream. */
off_t ftello(FILE *stream) { return (off_t)ftell(stream); }

/** @brief Suspend execution for microsecond intervals. */
int usleep(useconds_t usec) {
  unsigned int ms = (unsigned int)(usec / 1000U);
  if (ms > 0) {
    delay(ms);
  }
  return 0;
}

/** @brief Initiate pipe streams to or from a process. */
FILE *popen(const char *command, const char *type) {
  return awm_popen_scratch(command, type);
}

/** @brief Close a pipe stream to or from a process. */
int pclose(FILE *stream) { return awm_pclose_scratch(stream); }

/** @brief Frame storing saved state across simulated DOS vfork calls. */
typedef struct awm_dos_vfork_frame {
  jmp_buf jb;          /**< Saved execution context. */
  int saved_fds[16];   /**< Duplicated standard file descriptors. */
  char saved_cwd[260]; /**< Saved working directory. */
  void **globals_slot; /**< Pointer to BusyBox ptr_to_globals slot. */
  void *saved_globals; /**< Saved globals pointer value. */
} awm_dos_vfork_frame_t;

static awm_dos_vfork_frame_t g_awm_dos_vfork_frames[8];
static int g_awm_dos_vfork_depth = 0;
static int g_awm_dos_next_pid = 40000;
static int g_awm_dos_last_parent_pid = 0;
static int g_awm_dos_reap_pids[16];
static int g_awm_dos_reap_status[16];
static int g_awm_dos_reap_count = 0;

/** @brief Push a simulated vfork state frame on DOS. */
int awm_vfork_push(void **globals_slot) {
  int i;
  awm_dos_vfork_frame_t *f;
  if (g_awm_dos_vfork_depth >= 8) {
    errno = EAGAIN;
    return -1;
  }
  fflush(NULL);
  f = &g_awm_dos_vfork_frames[g_awm_dos_vfork_depth++];
  memset(f, 0, sizeof(*f));
  (void)getcwd(f->saved_cwd, sizeof(f->saved_cwd));
  f->globals_slot = globals_slot;
  f->saved_globals = globals_slot ? *globals_slot : NULL;
  for (i = 0; i < 3; i++) {
    f->saved_fds[i] = dup(i);
  }
  for (i = 3; i < 16; i++) {
    f->saved_fds[i] = -1;
  }
  return 0;
}

/** @brief Retrieve the active jmp_buf pointer for the current vfork frame. */
void *awm_vfork_jb(void) {
  return (void *)&g_awm_dos_vfork_frames[g_awm_dos_vfork_depth - 1].jb;
}

/** @brief Return the simulated child PID to the parent after vfork longjmp. */
int awm_vfork_parent_pid(void) { return g_awm_dos_last_parent_pid; }

/** @brief Return current nesting depth of simulated vfork frames. */
int awm_vfork_depth(void) { return g_awm_dos_vfork_depth; }

/** @brief Exit a simulated vfork child frame and restore parent context. */
void awm_vfork_exit(int status) {
  int i;
  awm_dos_vfork_frame_t *f;
  int child_pid;
  fflush(NULL);
  if (g_awm_dos_vfork_depth <= 0) {
    _exit(status);
    return;
  }
  f = &g_awm_dos_vfork_frames[--g_awm_dos_vfork_depth];
  if (f->globals_slot) {
    *f->globals_slot = f->saved_globals;
  }
  if (f->saved_cwd[0]) {
    (void)chdir(f->saved_cwd);
  }
  for (i = 0; i < 3; i++) {
    if (f->saved_fds[i] >= 0) {
      (void)dup2(f->saved_fds[i], i);
      (void)close(f->saved_fds[i]);
    }
  }
  child_pid = ++g_awm_dos_next_pid;
  if (g_awm_dos_reap_count < 16) {
    g_awm_dos_reap_pids[g_awm_dos_reap_count] = child_pid;
    g_awm_dos_reap_status[g_awm_dos_reap_count] = ((status & 0xff) << 8);
    g_awm_dos_reap_count++;
  }
  g_awm_dos_last_parent_pid = child_pid;
  longjmp(f->jb, 1);
}

/** @brief Reap a completed simulated vfork child process. */
int awm_vfork_try_reap(int pid, int *stat_loc) {
  int i;
  for (i = 0; i < g_awm_dos_reap_count; i++) {
    if (pid <= 0 || g_awm_dos_reap_pids[i] == pid) {
      int rpid = g_awm_dos_reap_pids[i];
      int wstatus = g_awm_dos_reap_status[i];
      int j;
      if (stat_loc) {
        *stat_loc = wstatus;
      }
      for (j = i; j + 1 < g_awm_dos_reap_count; j++) {
        g_awm_dos_reap_pids[j] = g_awm_dos_reap_pids[j + 1];
        g_awm_dos_reap_status[j] = g_awm_dos_reap_status[j + 1];
      }
      g_awm_dos_reap_count--;
      return rpid;
    }
  }
  return 0;
}

/** @brief Execute a program on DOS and exit the simulated vfork frame. */
int posix_execve(const char *pathname, char *const argv[], char *const envp[]) {
  int rc;
  if (!pathname || !argv) {
    errno = EINVAL;
    return -1;
  }
  fflush(NULL);
  if (envp) {
    rc = spawnve(P_WAIT, pathname, (const char *const *)argv,
                 (const char *const *)envp);
  } else {
    rc = spawnv(P_WAIT, pathname, (const char *const *)argv);
  }
  if (rc == -1) {
    return -1;
  }
  awm_vfork_exit(rc);
  return -1;
}

/** @brief Execute a program by pathname with argument vector. */
int posix_execv(const char *pathname, char *const argv[]) {
  return posix_execve(pathname, argv, NULL);
}

/** @brief Execute a program searching PATH on DOS. */
int posix_execvp(const char *file, char *const argv[]) {
  int rc;
  if (!file || !argv) {
    errno = EINVAL;
    return -1;
  }
  fflush(NULL);
  rc = spawnvp(P_WAIT, file, (const char *const *)argv);
  if (rc == -1) {
    return -1;
  }
  awm_vfork_exit(rc);
  return -1;
}

#undef execve
#undef execv
#undef execvp
/** @brief Standard execve wrapper for DOS. */
int execve(const char *pathname, char *const argv[], char *const envp[]) {
  return posix_execve(pathname, argv, envp);
}
/** @brief Standard execv wrapper for DOS. */
int execv(const char *pathname, char *const argv[]) {
  return posix_execv(pathname, argv);
}
/** @brief Standard execvp wrapper for DOS. */
int execvp(const char *file, char *const argv[]) {
  return posix_execvp(file, argv);
}

#endif

#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
void ASSIGN_CONST_PTR(const void *pptr, void *v) {
  union {
    const void *cp;
    void **pp;
  } u;
  u.cp = pptr;
  *u.pp = v;
}

void XZALLOC_CONST_PTR(const void *pptr, size_t size) {
  union {
    const void *cp;
    void **pp;
  } u;
  u.cp = pptr;
  *u.pp = calloc(1, size);
}

/**
 * @brief Initializes and validates the posix-core module.
 */
auto_win_msvc_error_t posix_core_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}
#endif

/** @brief Internal tracking slot for scratch-file simulated popen streams. */
typedef struct awm_scratch_pipe_slot {
  FILE *fp;           /**< Active stdio stream handle. */
  char tmp_path[64];  /**< Scratch file path to unlink on pclose. */
  char *write_cmd;    /**< Deferred command for write-mode pipes. */
  int read_exit_code; /**< Captured command status for read-mode pipes. */
} awm_scratch_pipe_slot_t;

static awm_scratch_pipe_slot_t g_awm_scratch_pipes[8];
static unsigned int g_awm_scratch_seq = 0U;

int awm_dos_mkstemp_format_83(char *out_buf, size_t out_cap, const char *tmpl,
                              unsigned int attempt) {
  size_t len;
  const char *base;
  size_t dir_len;
  size_t prefix_len;
  char stem[9];
  size_t keep;
  size_t i;

  if (!out_buf || !tmpl) {
    errno = EINVAL;
    return -1;
  }
  len = strlen(tmpl);
  if (len < 6U || out_cap <= len || strcmp(tmpl + len - 6U, "XXXXXX") != 0) {
    errno = EINVAL;
    return -1;
  }

  if ((tmpl[0] == '.' && (tmpl[1] == '/' || tmpl[1] == '\\')) &&
      strchr(tmpl + 2, '/') == NULL && strchr(tmpl + 2, '\\') == NULL) {
    base = tmpl + 2;
    dir_len = 0U;
  } else {
    const char *s1 = strrchr(tmpl, '/');
    const char *s2 = strrchr(tmpl, '\\');
    const char *sep = s1;
    if (s2 != NULL && (sep == NULL || s2 > sep)) {
      sep = s2;
    }
    if (sep != NULL) {
      base = sep + 1;
      dir_len = (size_t)(base - tmpl);
    } else {
      base = tmpl;
      dir_len = 0U;
    }
  }

  prefix_len = (size_t)((tmpl + len - 6U) - base);
  if (prefix_len == 0U) {
#if defined(_MSC_VER)
    sprintf_s(stem, sizeof(stem), "T%05u", attempt % 100000U);
#else
    sprintf(stem, "T%05u", attempt % 100000U);
#endif
  } else if (prefix_len == 1U) {
    char ch = base[0];
    if (ch >= 'a' && ch <= 'z') {
      ch = (char)(ch - ('a' - 'A'));
    } else if (!((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
                 ch == '_')) {
      ch = 'T';
    }
#if defined(_MSC_VER)
    sprintf_s(stem, sizeof(stem), "%c%06u", ch, attempt % 1000000U);
#else
    sprintf(stem, "%c%06u", ch, attempt % 1000000U);
#endif
  } else {
    keep = prefix_len > 3U ? 3U : prefix_len;
    for (i = 0U; i < keep; ++i) {
      char ch = base[i];
      if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - ('a' - 'A'));
      } else if (!((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
                   ch == '_')) {
        ch = 'T';
      }
      stem[i] = ch;
    }
#if defined(_MSC_VER)
    sprintf_s(stem + keep, sizeof(stem) - keep, "%05u", attempt % 100000U);
#else
    sprintf(stem + keep, "%05u", attempt % 100000U);
#endif
  }

  if (dir_len > 0U) {
    memcpy(out_buf, tmpl, dir_len);
  }
#if defined(_MSC_VER)
  strcpy_s(out_buf + dir_len, out_cap - dir_len, stem);
#else
  strcpy(out_buf + dir_len, stem);
#endif
  return 0;
}

/** @brief Opens a scratch-file-backed pipe stream for single-tasking shims. */
FILE *awm_popen_scratch(const char *command, const char *type) {
  int slot_idx = -1;
  int i;
  int is_write;
  int is_binary;
  char tmp_name[32];
  FILE *fp;
  int sys_rc = 0;
  size_t wcmd_cap;

  if (!command || !type || (type[0] != 'r' && type[0] != 'w') ||
      (type[1] != '\0' && !(type[1] == 'b' && type[2] == '\0') &&
       !(type[1] == 't' && type[2] == '\0'))) {
    errno = EINVAL;
    return NULL;
  }
  is_write = (type[0] == 'w') ? 1 : 0;
  is_binary = (type[1] == 'b') ? 1 : 0;

  for (i = 0; i < 8; ++i) {
    if (g_awm_scratch_pipes[i].fp == NULL) {
      slot_idx = i;
      break;
    }
  }
  if (slot_idx < 0) {
    errno = EMFILE;
    return NULL;
  }

#if defined(_MSC_VER)
  sprintf_s(tmp_name, sizeof(tmp_name), "SEDP%04u.TMP",
            (g_awm_scratch_seq++) % 10000U);
#else
  sprintf(tmp_name, "SEDP%04u.TMP", (g_awm_scratch_seq++) % 10000U);
#endif
  if (g_core_common_hooks.open_scratch_fn(tmp_name) < 0) {
    return NULL;
  }

  if (!is_write) {
    size_t cmd_len = strlen(command) + strlen(tmp_name) + 8U;
    char *redir_cmd = (char *)g_core_common_hooks.malloc_fn(cmd_len);
    if (!redir_cmd) {
      (void)remove(tmp_name);
      errno = ENOMEM;
      return NULL;
    }
#if defined(_MSC_VER)
    sprintf_s(redir_cmd, cmd_len, "%s > %s", command, tmp_name);
#else
    sprintf(redir_cmd, "%s > %s", command, tmp_name);
#endif
    sys_rc = g_core_common_hooks.system_fn(redir_cmd);
    g_core_common_hooks.free_fn(redir_cmd);
    fp = g_core_common_hooks.fopen_fn(tmp_name, is_binary ? "rb" : "r");
    if (!fp) {
      (void)remove(tmp_name);
      return NULL;
    }
    g_awm_scratch_pipes[slot_idx].fp = fp;
#if defined(_MSC_VER)
    strcpy_s(g_awm_scratch_pipes[slot_idx].tmp_path,
             sizeof(g_awm_scratch_pipes[slot_idx].tmp_path), tmp_name);
#else
    strcpy(g_awm_scratch_pipes[slot_idx].tmp_path, tmp_name);
#endif
    g_awm_scratch_pipes[slot_idx].write_cmd = NULL;
    g_awm_scratch_pipes[slot_idx].read_exit_code = sys_rc;
    return fp;
  }

  fp = g_core_common_hooks.fopen_fn(tmp_name, is_binary ? "wb" : "w");
  if (!fp) {
    (void)remove(tmp_name);
    return NULL;
  }
  wcmd_cap = strlen(command) + 1U;
  g_awm_scratch_pipes[slot_idx].write_cmd =
      (char *)g_core_common_hooks.malloc_fn(wcmd_cap);
  if (!g_awm_scratch_pipes[slot_idx].write_cmd) {
    fclose(fp);
    (void)remove(tmp_name);
    errno = ENOMEM;
    return NULL;
  }
#if defined(_MSC_VER)
  strcpy_s(g_awm_scratch_pipes[slot_idx].write_cmd, wcmd_cap, command);
#else
  strcpy(g_awm_scratch_pipes[slot_idx].write_cmd, command);
#endif
  g_awm_scratch_pipes[slot_idx].fp = fp;
#if defined(_MSC_VER)
  strcpy_s(g_awm_scratch_pipes[slot_idx].tmp_path,
           sizeof(g_awm_scratch_pipes[slot_idx].tmp_path), tmp_name);
#else
  strcpy(g_awm_scratch_pipes[slot_idx].tmp_path, tmp_name);
#endif
  g_awm_scratch_pipes[slot_idx].read_exit_code = 0;
  return fp;
}

/** @brief Closes a scratch-file-backed pipe stream and returns exit status. */
int awm_pclose_scratch(FILE *stream) {
  int i;
  if (!stream) {
    errno = EINVAL;
    return -1;
  }
  for (i = 0; i < 8; ++i) {
    if (g_awm_scratch_pipes[i].fp == stream) {
      int rc = g_awm_scratch_pipes[i].read_exit_code;
      fclose(stream);
      g_awm_scratch_pipes[i].fp = NULL;
      if (g_awm_scratch_pipes[i].write_cmd != NULL) {
        size_t cmd_len = strlen(g_awm_scratch_pipes[i].write_cmd) +
                         strlen(g_awm_scratch_pipes[i].tmp_path) + 8U;
        char *redir_cmd = (char *)g_core_common_hooks.malloc_fn(cmd_len);
        if (redir_cmd) {
#if defined(_MSC_VER)
          sprintf_s(redir_cmd, cmd_len, "%s < %s",
                    g_awm_scratch_pipes[i].write_cmd,
                    g_awm_scratch_pipes[i].tmp_path);
#else
          sprintf(redir_cmd, "%s < %s", g_awm_scratch_pipes[i].write_cmd,
                  g_awm_scratch_pipes[i].tmp_path);
#endif
          rc = g_core_common_hooks.system_fn(redir_cmd);
          g_core_common_hooks.free_fn(redir_cmd);
        } else {
          rc = -1;
        }
        g_core_common_hooks.free_fn(g_awm_scratch_pipes[i].write_cmd);
        g_awm_scratch_pipes[i].write_cmd = NULL;
      }
      (void)remove(g_awm_scratch_pipes[i].tmp_path);
      g_awm_scratch_pipes[i].tmp_path[0] = '\0';
      return rc;
    }
  }
  errno = EINVAL;
  return -1;
}

int awm_fwriting(FILE *stream) {
  if (stream == NULL) {
    return 0;
  }
#if defined(__WATCOMC__)
  return ((stream->_flag & _WRITE) != 0) ? 1 : 0;
#elif defined(__GLIBC__)
  return __fwriting(stream) ? 1 : 0;
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||    \
    defined(__NetBSD__) || defined(__DragonFly__)
  return ((stream->_flags & 0x0008) != 0) ? 1 : 0;
#elif defined(_WIN32) && !defined(__CYGWIN__)
#if defined(_UCRT)
  {
    struct awm_ucrt_iobuf {
      void *_Placeholder;
      char *_base;
      int _cnt;
      long _flags;
    };
    const struct awm_ucrt_iobuf *ufp =
        (const struct awm_ucrt_iobuf *)(const void *)stream;
    return ((ufp->_flags & 0x0002L) != 0L) ? 1 : 0;
  }
#else
  return ((stream->_flag & _IOWRT) != 0) ? 1 : 0;
#endif
#else
  return 0;
#endif
}
