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
#endif
/* clang-format on */

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

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) LIBPROC_WIN_HANDLE __stdcall CreateToolhelp32Snapshot(
    LIBPROC_WIN_DWORD dwFlags, LIBPROC_WIN_DWORD th32ProcessID);
__declspec(dllimport)
LIBPROC_WIN_BOOL __stdcall Process32First(LIBPROC_WIN_HANDLE hSnapshot,
                                          LIBPROC_PROCESSENTRY32 *lppe);
__declspec(dllimport)
LIBPROC_WIN_BOOL __stdcall Process32Next(LIBPROC_WIN_HANDLE hSnapshot,
                                         LIBPROC_PROCESSENTRY32 *lppe);
__declspec(dllimport)
LIBPROC_WIN_BOOL __stdcall CloseHandle(LIBPROC_WIN_HANDLE hObject);
__declspec(dllimport) LIBPROC_WIN_DWORD __stdcall GetCurrentProcessId(void);
__declspec(dllimport)
LIBPROC_WIN_DWORD __stdcall GetModuleFileNameA(void *hModule, char *lpFilename,
                                               LIBPROC_WIN_DWORD nSize);
#ifdef __cplusplus
}
#endif

struct win_proc_cursor {
  LIBPROC_WIN_HANDLE snap;
  LIBPROC_PROCESSENTRY32 pe;
  int first;
};

static void strip_exe_suffix(char *name) {
  size_t len;
  if (name == NULL) {
    return;
  }
  len = strlen(name);
  if (len > 4) {
    const char *ext = name + len - 4;
    if ((ext[0] == '.') && (ext[1] == 'e' || ext[1] == 'E') &&
        (ext[2] == 'x' || ext[2] == 'X') && (ext[3] == 'e' || ext[3] == 'E')) {
      name[len - 4] = '\0';
    }
  }
}

void *posix_proc_open(void) {
  struct win_proc_cursor *c =
      (struct win_proc_cursor *)malloc(sizeof(struct win_proc_cursor));
  if (c == NULL) {
    return NULL;
  }
  memset(c, 0, sizeof(*c));
  c->snap = CreateToolhelp32Snapshot(LIBPROC_TH32CS_SNAPPROCESS, 0);
  if (c->snap == LIBPROC_INVALID_HANDLE_VALUE) {
    free(c);
    return NULL;
  }
  c->pe.dwSize = sizeof(LIBPROC_PROCESSENTRY32);
  if (!Process32First(c->snap, &c->pe)) {
    CloseHandle(c->snap);
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
      if (!Process32Next(c->snap, &c->pe)) {
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
    const char *s1 = strrchr(base, '\\');
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
  strncpy(out_entry->comm, base, sizeof(out_entry->comm) - 1);
  out_entry->comm[sizeof(out_entry->comm) - 1] = '\0';
#endif
  strip_exe_suffix(out_entry->comm);

#if defined(_MSC_VER)
  strncpy_s(out_entry->cmdline, sizeof(out_entry->cmdline), out_entry->comm,
            _TRUNCATE);
#else
  strncpy(out_entry->cmdline, out_entry->comm, sizeof(out_entry->cmdline) - 1);
  out_entry->cmdline[sizeof(out_entry->cmdline) - 1] = '\0';
#endif
  return 1;
}

void posix_proc_close(void *cursor) {
  struct win_proc_cursor *c = (struct win_proc_cursor *)cursor;
  if (c == NULL) {
    return;
  }
  if (c->snap != LIBPROC_INVALID_HANDLE_VALUE) {
    CloseHandle(c->snap);
  }
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

  len = GetModuleFileNameA(NULL, (char *)buffer, (LIBPROC_WIN_DWORD)buffersize);
  if (len == 0 || len >= (LIBPROC_WIN_DWORD)buffersize) {
    return 0;
  }

  return (int)len;
}

#else

#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#endif

struct generic_proc_cursor {
  struct posix_proc_entry *entries;
  size_t count;
  size_t index;
};

void *posix_proc_open(void) {
  struct generic_proc_cursor *c =
      (struct generic_proc_cursor *)malloc(sizeof(struct generic_proc_cursor));
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
    if (sysctl(mib, 4, NULL, &len, NULL, 0) == 0 && len > 0) {
      struct kinfo_proc *kp = (struct kinfo_proc *)malloc(len);
      if (kp != NULL) {
        if (sysctl(mib, 4, kp, &len, NULL, 0) == 0) {
          size_t n = len / sizeof(struct kinfo_proc);
          size_t i;
          c->entries = (struct posix_proc_entry *)calloc(
              n > 0 ? n : 1, sizeof(struct posix_proc_entry));
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
              strncpy(pe->comm, kp[i].kp_proc.p_comm, sizeof(pe->comm) - 1);
              pe->comm[sizeof(pe->comm) - 1] = '\0';
              strncpy(pe->cmdline, pe->comm, sizeof(pe->cmdline) - 1);
              pe->cmdline[sizeof(pe->cmdline) - 1] = '\0';
            }
          }
        }
        free(kp);
      }
    }
  }
#endif
  if (c->entries == NULL || c->count == 0) {
    free(c->entries);
    c->entries =
        (struct posix_proc_entry *)calloc(1, sizeof(struct posix_proc_entry));
    if (c->entries == NULL) {
      free(c);
      return NULL;
    }
    c->count = 1;
    c->entries[0].pid = (unsigned int)getpid();
    c->entries[0].ppid = (unsigned int)getppid();
    c->entries[0].pgid = c->entries[0].pid;
    c->entries[0].state[0] = 'S';
    c->entries[0].state[1] = ' ';
    c->entries[0].state[2] = ' ';
    strcpy(c->entries[0].comm, "busybox");
    strcpy(c->entries[0].cmdline, "busybox");
  }
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
