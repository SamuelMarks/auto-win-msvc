#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file posix-libproc.c
 * @brief Implementation of posix-libproc polyfills and process enumeration.
 */

/* clang-format off */
#include "posix-libproc.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#endif
#endif
/* clang-format on */

static void *default_libproc_malloc(size_t sz) { return malloc(sz); }
static void *(*g_libproc_malloc)(size_t) = default_libproc_malloc;

#if !defined(_WIN32) && !defined(_WIN64)
static void *default_libproc_calloc(size_t n, size_t sz) {
  return calloc(n, sz);
}
static void *(*g_libproc_calloc)(size_t, size_t) = default_libproc_calloc;
#endif

/**
 * @brief Sets custom allocator hooks for OOM testing in posix-libproc.
 */
void posix_libproc_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                   void *(*calloc_fn)(size_t, size_t));
void posix_libproc_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                   void *(*calloc_fn)(size_t, size_t)) {
  g_libproc_malloc = malloc_fn;
#if !defined(_WIN32) && !defined(_WIN64)
  g_libproc_calloc = calloc_fn;
#else
  (void)calloc_fn;
#endif
}

#if defined(_WIN32) || defined(_WIN64)

typedef void *LIBPROC_WIN_HANDLE;
typedef unsigned long LIBPROC_WIN_DWORD;
typedef int LIBPROC_WIN_BOOL;
#if defined(_WIN64)
typedef unsigned __int64 LIBPROC_UINTPTR;
#else
typedef unsigned long LIBPROC_UINTPTR;
#endif

#define LIBPROC_INVALID_HANDLE_VALUE ((LIBPROC_WIN_HANDLE)(ptrdiff_t)(-1))
#define LIBPROC_TH32CS_SNAPPROCESS 0x00000002UL

typedef struct tagLIBPROC_PROCESSENTRY32 {
  LIBPROC_WIN_DWORD dwSize;
  LIBPROC_WIN_DWORD cntUsage;
  LIBPROC_WIN_DWORD th32ProcessID;
  LIBPROC_UINTPTR th32DefaultHeapID;
  LIBPROC_WIN_DWORD th32ModuleID;
  LIBPROC_WIN_DWORD cntThreads;
  LIBPROC_WIN_DWORD th32ParentProcessID;
  long pcPriClassBase;
  LIBPROC_WIN_DWORD dwFlags;
  char szExeFile[260];
} LIBPROC_PROCESSENTRY32;

__declspec(dllimport) LIBPROC_WIN_HANDLE __stdcall
CreateToolhelp32Snapshot(LIBPROC_WIN_DWORD dwFlags,
                         LIBPROC_WIN_DWORD th32ProcessID);
__declspec(dllimport) LIBPROC_WIN_BOOL __stdcall
Process32First(LIBPROC_WIN_HANDLE hSnapshot, LIBPROC_PROCESSENTRY32 *lppe);
__declspec(dllimport) LIBPROC_WIN_BOOL __stdcall
Process32Next(LIBPROC_WIN_HANDLE hSnapshot, LIBPROC_PROCESSENTRY32 *lppe);
__declspec(dllimport) LIBPROC_WIN_BOOL __stdcall
CloseHandle(LIBPROC_WIN_HANDLE hObject);
__declspec(dllimport) LIBPROC_WIN_DWORD __stdcall GetCurrentProcessId(void);
__declspec(dllimport) LIBPROC_WIN_DWORD __stdcall
GetModuleFileNameA(void *hModule, char *lpFilename, LIBPROC_WIN_DWORD nSize);

static LIBPROC_WIN_HANDLE default_libproc_snapshot(LIBPROC_WIN_DWORD flags,
                                                   LIBPROC_WIN_DWORD pid) {
  return CreateToolhelp32Snapshot(flags, pid);
}
static LIBPROC_WIN_BOOL default_libproc_first(LIBPROC_WIN_HANDLE snap,
                                              LIBPROC_PROCESSENTRY32 *pe) {
  return Process32First(snap, pe);
}
static LIBPROC_WIN_BOOL default_libproc_next(LIBPROC_WIN_HANDLE snap,
                                             LIBPROC_PROCESSENTRY32 *pe) {
  return Process32Next(snap, pe);
}
static LIBPROC_WIN_BOOL default_libproc_close(LIBPROC_WIN_HANDLE h) {
  return CloseHandle(h);
}
static LIBPROC_WIN_DWORD default_libproc_modname(void *hMod, char *buf,
                                                 LIBPROC_WIN_DWORD sz) {
  return GetModuleFileNameA(hMod, buf, sz);
}

static LIBPROC_WIN_HANDLE (*g_libproc_snapshot)(
    LIBPROC_WIN_DWORD, LIBPROC_WIN_DWORD) = default_libproc_snapshot;
static LIBPROC_WIN_BOOL (*g_libproc_first)(
    LIBPROC_WIN_HANDLE, LIBPROC_PROCESSENTRY32 *) = default_libproc_first;
static LIBPROC_WIN_BOOL (*g_libproc_next)(
    LIBPROC_WIN_HANDLE, LIBPROC_PROCESSENTRY32 *) = default_libproc_next;
static LIBPROC_WIN_BOOL (*g_libproc_close)(LIBPROC_WIN_HANDLE) =
    default_libproc_close;
static LIBPROC_WIN_DWORD (*g_libproc_modname)(
    void *, char *, LIBPROC_WIN_DWORD) = default_libproc_modname;

/**
 * @brief Sets custom Win32 hooks for testing posix-libproc on Windows.
 */
void posix_libproc_set_win32_hooks(
    LIBPROC_WIN_HANDLE (*snap_fn)(LIBPROC_WIN_DWORD, LIBPROC_WIN_DWORD),
    LIBPROC_WIN_BOOL (*first_fn)(LIBPROC_WIN_HANDLE, LIBPROC_PROCESSENTRY32 *),
    LIBPROC_WIN_BOOL (*next_fn)(LIBPROC_WIN_HANDLE, LIBPROC_PROCESSENTRY32 *),
    LIBPROC_WIN_BOOL (*close_fn)(LIBPROC_WIN_HANDLE),
    LIBPROC_WIN_DWORD (*modname_fn)(void *, char *, LIBPROC_WIN_DWORD));
void posix_libproc_set_win32_hooks(
    LIBPROC_WIN_HANDLE (*snap_fn)(LIBPROC_WIN_DWORD, LIBPROC_WIN_DWORD),
    LIBPROC_WIN_BOOL (*first_fn)(LIBPROC_WIN_HANDLE, LIBPROC_PROCESSENTRY32 *),
    LIBPROC_WIN_BOOL (*next_fn)(LIBPROC_WIN_HANDLE, LIBPROC_PROCESSENTRY32 *),
    LIBPROC_WIN_BOOL (*close_fn)(LIBPROC_WIN_HANDLE),
    LIBPROC_WIN_DWORD (*modname_fn)(void *, char *, LIBPROC_WIN_DWORD)) {
  g_libproc_snapshot = snap_fn;
  g_libproc_first = first_fn;
  g_libproc_next = next_fn;
  g_libproc_close = close_fn;
  g_libproc_modname = modname_fn;
}

/**
 * @brief Resets allocator and Win32 hooks to defaults.
 */
void posix_libproc_reset_hooks(void);
void posix_libproc_reset_hooks(void) {
  g_libproc_malloc = default_libproc_malloc;
  g_libproc_snapshot = default_libproc_snapshot;
  g_libproc_first = default_libproc_first;
  g_libproc_next = default_libproc_next;
  g_libproc_close = default_libproc_close;
  g_libproc_modname = default_libproc_modname;
}

struct win_proc_cursor {
  LIBPROC_WIN_HANDLE snap;
  LIBPROC_PROCESSENTRY32 pe;
  int first;
};

static void strip_exe_suffix(char *name) {
  size_t len = strlen(name);
  if (len > 4) {
    const char *ext = name + len - 4;
    if ((ext[0] == '.') && (ext[1] == 'e' || ext[1] == 'E') &&
        (ext[2] == 'x' || ext[2] == 'X') && (ext[3] == 'e' || ext[3] == 'E')) {
      name[len - 4] = '\0';
    }
  }
}

void *posix_proc_open(void) {
  struct win_proc_cursor *c = (struct win_proc_cursor *)g_libproc_malloc(
      sizeof(struct win_proc_cursor));
  if (c == NULL) {
    return NULL;
  }
  memset(c, 0, sizeof(*c));
  c->snap = g_libproc_snapshot(LIBPROC_TH32CS_SNAPPROCESS, 0);
  if (c->snap == LIBPROC_INVALID_HANDLE_VALUE) {
    free(c);
    return NULL;
  }
  c->pe.dwSize = sizeof(LIBPROC_PROCESSENTRY32);
  if (!g_libproc_first(c->snap, &c->pe)) {
    g_libproc_close(c->snap);
    free(c);
    return NULL;
  }
  c->first = 1;
  return c;
}

int posix_proc_next(void *cursor, struct posix_proc_entry *out_entry) {
  struct win_proc_cursor *c = (struct win_proc_cursor *)cursor;
  const char *base;
  if (c == NULL || out_entry == NULL) {
    return 0;
  }
  for (;;) {
    if (c->first) {
      c->first = 0;
    } else {
      if (!g_libproc_next(c->snap, &c->pe)) {
        return 0;
      }
    }
    if (c->pe.th32ProcessID != 0) {
      break;
    }
  }
  memset(out_entry, 0, sizeof(*out_entry));
  out_entry->pid = (unsigned int)c->pe.th32ProcessID;
  out_entry->ppid = (unsigned int)c->pe.th32ParentProcessID;
  out_entry->pgid = out_entry->pid;
  out_entry->uid = 0;
  out_entry->gid = 0;
  out_entry->vsz = 1024UL;
  out_entry->rss = 512UL;
  out_entry->state[0] = 'S';
  out_entry->state[1] = ' ';
  out_entry->state[2] = ' ';
  out_entry->state[3] = '\0';

  base = c->pe.szExeFile;
  {
    const char *s1 = strrchr(base, 92);
    const char *s2 = strrchr(base, '/');
    if (s1 && s2) {
      base = (s1 > s2) ? (s1 + 1) : (s2 + 1);
    } else if (s1) {
      base = s1 + 1;
    } else if (s2) {
      base = s2 + 1;
    }
  }
#if defined(_MSC_VER)
  strncpy_s(out_entry->comm, sizeof(out_entry->comm), base, _TRUNCATE);
#else
  {
    size_t blen = strlen(base);
    if (blen >= sizeof(out_entry->comm)) {
      blen = sizeof(out_entry->comm) - 1;
    }
    memcpy(out_entry->comm, base, blen);
    out_entry->comm[blen] = '\0';
  }
#endif
  strip_exe_suffix(out_entry->comm);

#if defined(_MSC_VER)
  strcpy_s(out_entry->cmdline, sizeof(out_entry->cmdline), out_entry->comm);
#else
  memcpy(out_entry->cmdline, out_entry->comm, strlen(out_entry->comm) + 1);
#endif
  return 1;
}

void posix_proc_close(void *cursor) {
  struct win_proc_cursor *c = (struct win_proc_cursor *)cursor;
  if (c == NULL) {
    return;
  }
  g_libproc_close(c->snap);
  free(c);
}

int proc_pidpath(int pid, void *buffer, unsigned int buffersize) {
  LIBPROC_WIN_DWORD len;
  LIBPROC_WIN_DWORD cur_pid;

  if (buffer == NULL || buffersize == 0) {
    return 0;
  }

  cur_pid = GetCurrentProcessId();
  if (pid != 0 && (LIBPROC_WIN_DWORD)pid != cur_pid) {
    return 0;
  }

  len = g_libproc_modname(NULL, (char *)buffer, (LIBPROC_WIN_DWORD)buffersize);
  if (len == 0 || len >= (LIBPROC_WIN_DWORD)buffersize) {
    return 0;
  }

  return (int)len;
}

#else

#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
static int default_libproc_sysctl(int *name, unsigned int namelen, void *oldp,
                                  size_t *oldlenp, void *newp, size_t newlen) {
  return sysctl(name, namelen, oldp, oldlenp, newp, newlen);
}
static int (*g_libproc_sysctl)(int *, unsigned int, void *, size_t *, void *,
                               size_t) = default_libproc_sysctl;

/**
 * @brief Sets custom sysctl hook for testing posix-libproc on macOS.
 */
void posix_libproc_set_sysctl_hook(int (*sysctl_fn)(int *, unsigned int, void *,
                                                    size_t *, void *, size_t));
void posix_libproc_set_sysctl_hook(int (*sysctl_fn)(int *, unsigned int, void *,
                                                    size_t *, void *, size_t)) {
  g_libproc_sysctl = sysctl_fn;
}
#endif

/**
 * @brief Resets allocator and OS hooks to defaults.
 */
void posix_libproc_reset_hooks(void);
void posix_libproc_reset_hooks(void) {
  g_libproc_malloc = default_libproc_malloc;
  g_libproc_calloc = default_libproc_calloc;
#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
  g_libproc_sysctl = default_libproc_sysctl;
#endif
}

struct generic_proc_cursor {
  struct posix_proc_entry *entries;
  size_t count;
  size_t index;
};

void *posix_proc_open(void) {
  struct generic_proc_cursor *c =
      (struct generic_proc_cursor *)g_libproc_malloc(
          sizeof(struct generic_proc_cursor));
  if (c == NULL) {
    return NULL;
  }
  c->entries = NULL;
  c->count = 0;
  c->index = 0;
#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
  {
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
    size_t len = 0;
    if (g_libproc_sysctl(mib, 4, NULL, &len, NULL, 0) == 0 &&
        len >= sizeof(struct kinfo_proc)) {
      struct kinfo_proc *kp = (struct kinfo_proc *)g_libproc_malloc(len);
      if (kp != NULL) {
        if (g_libproc_sysctl(mib, 4, kp, &len, NULL, 0) == 0) {
          size_t n = len / sizeof(struct kinfo_proc);
          size_t i;
          c->entries = (struct posix_proc_entry *)g_libproc_calloc(
              n, sizeof(struct posix_proc_entry));
          if (c->entries != NULL) {
            for (i = 0; i < n; ++i) {
              struct posix_proc_entry *pe;
              if (kp[i].kp_proc.p_pid <= 0) {
                continue;
              }
              pe = &c->entries[c->count++];
              pe->pid = (unsigned int)kp[i].kp_proc.p_pid;
              pe->ppid = (unsigned int)kp[i].kp_eproc.e_ppid;
              pe->pgid = (unsigned int)kp[i].kp_eproc.e_pgid;
              pe->uid = (unsigned int)kp[i].kp_eproc.e_ucred.cr_uid;
              pe->gid = (unsigned int)kp[i].kp_eproc.e_pcred.p_rgid;
              pe->vsz = 1024UL;
              pe->rss = 512UL;
              pe->state[0] = 'S';
              pe->state[1] = ' ';
              pe->state[2] = ' ';
              pe->state[3] = '\0';
              {
                size_t clen = strlen(kp[i].kp_proc.p_comm);
                (memcpy)(pe->comm, kp[i].kp_proc.p_comm, clen + 1);
                (memcpy)(pe->cmdline, pe->comm, clen + 1);
              }
            }
          }
        }
        free(kp);
      }
    }
  }
  if (c->count > 0) {
    return c;
  }
  free(c->entries);
#endif
  c->entries = (struct posix_proc_entry *)g_libproc_calloc(
      1, sizeof(struct posix_proc_entry));
  if (c->entries == NULL) {
    free(c);
    return NULL;
  }
  c->count = 1;
  c->entries[0].pid = (unsigned int)getpid();
#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
  c->entries[0].ppid = 0;
#else
  c->entries[0].ppid = (unsigned int)getppid();
#endif
  c->entries[0].pgid = c->entries[0].pid;
  c->entries[0].state[0] = 'S';
  c->entries[0].state[1] = ' ';
  c->entries[0].state[2] = ' ';
  memcpy(c->entries[0].comm, "busybox", 8);
  memcpy(c->entries[0].cmdline, "busybox", 8);
  return c;
}

int posix_proc_next(void *cursor, struct posix_proc_entry *out_entry) {
  struct generic_proc_cursor *c = (struct generic_proc_cursor *)cursor;
  if (c == NULL || out_entry == NULL || c->index >= c->count) {
    return 0;
  }
  *out_entry = c->entries[c->index++];
  return 1;
}

void posix_proc_close(void *cursor) {
  struct generic_proc_cursor *c = (struct generic_proc_cursor *)cursor;
  if (c != NULL) {
    free(c->entries);
    free(c);
  }
}

#if !defined(__APPLE__)
int proc_pidpath(int pid, void *buffer, unsigned int buffersize) {
  if (pid < 0 || buffer == NULL || buffersize == 0) {
    return 0;
  }
  return 0;
}
#endif

#endif

/**
 * @brief Retrieves information on posix-libproc availability.
 */
auto_win_msvc_error_t posix_libproc_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_libproc;

#endif
typedef int make_iso_compilers_happy_mega_batch3_6;
