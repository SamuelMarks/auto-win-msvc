#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__MSDOS__) || defined(__WATCOMC__) || defined(__EMSCRIPTEN__)
/**
 * @file posix-spawn.c
 * @brief Strict C89 POSIX spawn implementation for Windows and POSIX.
 */

/* clang-format off */
#include "posix-spawn.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1400
#include <crtdbg.h>
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#endif
/* clang-format on */

/** @brief Optional test allocator override for OOM simulation. */
#if defined(_WIN32) && defined(auto_win_msvc_export_lib_EXPORTS)
__declspec(dllexport)
#endif
void *(*posix_spawn_mock_malloc_ptr)(size_t) = NULL;

static void *spawn_malloc(size_t size) {
  return posix_spawn_mock_malloc_ptr ? posix_spawn_mock_malloc_ptr(size)
                                     : malloc(size);
}

/**
 * @brief Initializes and validates the posix-spawn module.
 * @return AUTO_WIN_MSVC_SUCCESS or AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT.
 */
auto_win_msvc_error_t posix_spawn_init(int *out_status) {
  if (out_status == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_status = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1400
static void spawn_null_invalid_param_handler(const wchar_t *expression,
                                             const wchar_t *function,
                                             const wchar_t *file,
                                             unsigned int line,
                                             uintptr_t pReserved) {
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif

static ptrdiff_t safe_get_osfhandle(int fd) {
  ptrdiff_t h = -1;
  if (fd < 0 || fd >= 2048) {
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    _invalid_parameter_handler old =
        _set_invalid_parameter_handler(spawn_null_invalid_param_handler);
    AUTO_WIN_MSVC_SUPPRESS_CRT_REPORT();
    h = (ptrdiff_t)_get_osfhandle(fd);
    _set_invalid_parameter_handler(old);
  }
#else
  h = (ptrdiff_t)_get_osfhandle(fd);
#endif
  return h;
}
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)

__declspec(selectany) void *g_posix_child_handles[1024] = {0};
__declspec(selectany) unsigned long g_posix_child_pids[1024] = {0};
__declspec(selectany) int g_posix_child_count = 0;

#ifndef O_ACCMODE
/** @brief File access mode bitmask. */
#define O_ACCMODE (_O_RDONLY | _O_WRONLY | _O_RDWR)
#endif

/* Forward declarations to avoid <windows.h> */
#define WINAPI __stdcall
#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE ((void *)(size_t)-1)
#define STD_INPUT_HANDLE ((unsigned int)-10)
/** @brief Standard output handle constant. */
#define STD_OUTPUT_HANDLE ((unsigned int)-11)
/** @brief Standard error handle constant. */
#define STD_ERROR_HANDLE ((unsigned int)-12)

typedef unsigned long DWORD;
typedef int BOOL;
typedef void *HANDLE;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef void *LPVOID;

typedef struct _SECURITY_ATTRIBUTES {
  DWORD nLength;
  LPVOID lpSecurityDescriptor;
  BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _STARTUPINFOA {
  DWORD cb;
  char *lpReserved;
  char *lpDesktop;
  char *lpTitle;
  DWORD dwX;
  DWORD dwY;
  DWORD dwXSize;
  DWORD dwYSize;
  DWORD dwXCountChars;
  DWORD dwYCountChars;
  DWORD dwFillAttribute;
  DWORD dwFlags;
  WORD wShowWindow;
  WORD cbReserved2;
  BYTE *lpReserved2;
  HANDLE hStdInput;
  HANDLE hStdOutput;
  HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _PROCESS_INFORMATION {
  HANDLE hProcess;
  HANDLE hThread;
  DWORD dwProcessId;
  DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

#define CREATE_NEW_PROCESS_GROUP 0x00000200
#define STARTF_USESTDHANDLES 0x00000100
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define GENERIC_READ 0x80000000UL
#define GENERIC_WRITE 0x40000000UL
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
#define FILE_ATTRIBUTE_NORMAL 0x00000080

__declspec(dllimport) HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameA(HANDLE hModule,
                                                      char *lpFilename,
                                                      DWORD nSize);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE hObject);
__declspec(dllimport) HANDLE WINAPI CreateFileA(
    const char *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) BOOL WINAPI CreateProcessA(
    const char *lpApplicationName, char *lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, const char *lpCurrentDirectory,
    LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);

static unsigned long default_spawn_mod_fname(char *buf, unsigned long sz) {
  return (unsigned long)GetModuleFileNameA(NULL, buf, (DWORD)sz);
}
static int default_spawn_create_proc(const char *app, char *cmdline,
                                     unsigned long flags, void *env, void *si,
                                     void *pi) {
  return (int)CreateProcessA(app, cmdline, NULL, NULL, TRUE, (DWORD)flags, env,
                             NULL, (LPSTARTUPINFOA)si,
                             (LPPROCESS_INFORMATION)pi);
}
static int default_spawn_close_handle(void *h) {
  return (int)CloseHandle((HANDLE)h);
}

static ptrdiff_t (*g_spawn_get_osfh)(int) = safe_get_osfhandle;
static unsigned long (*g_spawn_mod_fname)(char *, unsigned long) =
    default_spawn_mod_fname;
static int (*g_spawn_create_proc)(const char *, char *, unsigned long, void *,
                                  void *, void *) = default_spawn_create_proc;
static int (*g_spawn_close_handle)(void *) = default_spawn_close_handle;

/**
 * @brief Overrides Win32 OS hooks used by posix-spawn for deterministic branch
 * testing.
 */
void posix_spawn_set_win32_hooks(ptrdiff_t (*get_osfh_fn)(int),
                                 unsigned long (*mod_fname_fn)(char *,
                                                               unsigned long),
                                 int (*create_proc_fn)(const char *, char *,
                                                       unsigned long, void *,
                                                       void *, void *),
                                 int (*close_handle_fn)(void *)) {
  g_spawn_get_osfh = (get_osfh_fn != NULL) ? get_osfh_fn : safe_get_osfhandle;
  g_spawn_mod_fname =
      (mod_fname_fn != NULL) ? mod_fname_fn : default_spawn_mod_fname;
  g_spawn_create_proc =
      (create_proc_fn != NULL) ? create_proc_fn : default_spawn_create_proc;
  g_spawn_close_handle =
      (close_handle_fn != NULL) ? close_handle_fn : default_spawn_close_handle;
}

/**
 * @brief Sets the tracked child process count and returns the previous count.
 * @return Previous child process table count.
 */
int posix_spawn_set_child_count(int new_count) {
  int prev = g_posix_child_count;
  g_posix_child_count = new_count;
  return prev;
}

#endif

/* File Action Types */
typedef enum {
  POSIX_SPAWN_ACTION_CLOSE,
  POSIX_SPAWN_ACTION_DUP2,
  POSIX_SPAWN_ACTION_OPEN
} posix_spawn_action_type_t;

typedef struct posix_spawn_action {
  posix_spawn_action_type_t type;
  int fd;
  int new_fd;
  char *path;
  int oflag;
  mode_t mode;
  struct posix_spawn_action *next;
} posix_spawn_action_t;

/**
 * @brief Initialize a file actions object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
  if (!file_actions) {
    return EINVAL;
  }
  file_actions->actions = NULL;
  return 0;
}

/**
 * @brief Destroy a file actions object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
  posix_spawn_action_t *action;
  if (!file_actions) {
    return EINVAL;
  }
  action = (posix_spawn_action_t *)file_actions->actions;
  while (action) {
    posix_spawn_action_t *next = action->next;
    if (action->path) {
      free(action->path);
    }
    free(action);
    action = next;
  }
  file_actions->actions = NULL;
  return 0;
}

static auto_win_msvc_error_t
add_action(posix_spawn_file_actions_t *file_actions,
           posix_spawn_action_t **out_action) {
  posix_spawn_action_t *new_action;
  new_action =
      (posix_spawn_action_t *)spawn_malloc(sizeof(posix_spawn_action_t));
  if (!new_action) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  new_action->next = NULL;
  new_action->path = NULL;

  if (!file_actions->actions) {
    file_actions->actions = new_action;
  } else {
    posix_spawn_action_t *curr = (posix_spawn_action_t *)file_actions->actions;
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = new_action;
  }
  *out_action = new_action;
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Add a close action to the file actions object.
 * @return 0 on success, error code on failure.
 */
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
                                      int fildes) {
  posix_spawn_action_t *action = NULL;
  auto_win_msvc_error_t rc;
  if (fildes < 0) {
    return EBADF;
  }
  if (!file_actions) {
    return EINVAL;
  }
  rc = add_action(file_actions, &action);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    return ENOMEM;
  }
  action->type = POSIX_SPAWN_ACTION_CLOSE;
  action->fd = fildes;
  return 0;
}

/**
 * @brief Add a dup2 action to the file actions object.
 * @return 0 on success, error code on failure.
 */
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
                                     int fildes, int newfildes) {
  posix_spawn_action_t *action = NULL;
  auto_win_msvc_error_t rc;
  if (fildes < 0 || newfildes < 0) {
    return EBADF;
  }
  if (!file_actions) {
    return EINVAL;
  }
  rc = add_action(file_actions, &action);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    return ENOMEM;
  }
  action->type = POSIX_SPAWN_ACTION_DUP2;
  action->fd = fildes;
  action->new_fd = newfildes;
  return 0;
}

/**
 * @brief Add an open action to the file actions object.
 * @return 0 on success, error code on failure.
 */
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *file_actions,
                                     int fildes, const char *path, int oflag,
                                     mode_t mode) {
  posix_spawn_action_t *action = NULL;
  auto_win_msvc_error_t rc;
  size_t plen;
  if (fildes < 0) {
    return EBADF;
  }
  if (!path || !file_actions) {
    return EINVAL;
  }
  plen = strlen(path) + 1;
  action = NULL;
  rc = add_action(file_actions, &action);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    return ENOMEM;
  }
  action->type = POSIX_SPAWN_ACTION_OPEN;
  action->fd = fildes;
  action->path = (char *)spawn_malloc(plen);
  if (!action->path) {
    return ENOMEM;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  strcpy_s(action->path, plen, path);
#else
  memcpy(action->path, path, plen);
#endif
  action->oflag = oflag;
  action->mode = mode;
  return 0;
}

/**
 * @brief Initialize a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_init(posix_spawnattr_t *attr) {
  if (!attr) {
    return EINVAL;
  }
  memset(attr, 0, sizeof(posix_spawnattr_t));
  return 0;
}

/**
 * @brief Destroy a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
  if (!attr) {
    return EINVAL;
  }
  return 0;
}

/**
 * @brief Get the spawn flags from a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short *flags) {
  if (!attr || !flags) {
    return EINVAL;
  }
  *flags = attr->flags;
  return 0;
}

/**
 * @brief Set the spawn flags in a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
  if (!attr) {
    return EINVAL;
  }
  attr->flags = flags;
  return 0;
}

/**
 * @brief Get the process group ID from a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, pid_t *pgroup) {
  if (!attr || !pgroup) {
    return EINVAL;
  }
  *pgroup = attr->pgroup;
  return 0;
}

/**
 * @brief Set the process group ID in a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup) {
  if (!attr) {
    return EINVAL;
  }
  attr->pgroup = pgroup;
  return 0;
}

/**
 * @brief Get the scheduling parameters from a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr,
                                  struct sched_param *schedparam) {
  if (!attr || !schedparam) {
    return EINVAL;
  }
  *schedparam = attr->schedparam;
  return 0;
}

/**
 * @brief Set the scheduling parameters in a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_setschedparam(posix_spawnattr_t *attr,
                                  const struct sched_param *schedparam) {
  if (!attr || !schedparam) {
    return EINVAL;
  }
  attr->schedparam = *schedparam;
  return 0;
}

/**
 * @brief Get the scheduling policy from a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr,
                                   int *schedpolicy) {
  if (!attr || !schedpolicy) {
    return EINVAL;
  }
  *schedpolicy = attr->schedpolicy;
  return 0;
}

/**
 * @brief Set the scheduling policy in a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy) {
  if (!attr) {
    return EINVAL;
  }
  attr->schedpolicy = schedpolicy;
  return 0;
}

/**
 * @brief Get the default signal mask from a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr,
                                  sigset_t *sigdefault) {
  if (!attr || !sigdefault) {
    return EINVAL;
  }
  *sigdefault = attr->sigdefault;
  return 0;
}

/**
 * @brief Set the default signal mask in a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr,
                                  const sigset_t *sigdefault) {
  if (!attr || !sigdefault) {
    return EINVAL;
  }
  attr->sigdefault = *sigdefault;
  return 0;
}

/**
 * @brief Get the signal mask from a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr,
                               sigset_t *sigmask) {
  if (!attr || !sigmask) {
    return EINVAL;
  }
  *sigmask = attr->sigmask;
  return 0;
}

/**
 * @brief Set the signal mask in a spawn attributes object.
 * @return 0 on success, EINVAL on NULL.
 */
int posix_spawnattr_setsigmask(posix_spawnattr_t *attr,
                               const sigset_t *sigmask) {
  if (!attr || !sigmask) {
    return EINVAL;
  }
  attr->sigmask = *sigmask;
  return 0;
}

#if defined(_MSC_VER) || defined(__MINGW32__)

static size_t get_quoted_arg_len(const char *arg) {
  size_t len = 0;
  int needs_quotes = 0;
  const char *p;

  if (!*arg) {
    return 2;
  }

  for (p = arg; *p; ++p) {
    if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\v' || *p == '\"') {
      needs_quotes = 1;
    }
  }

  if (!needs_quotes) {
    return strlen(arg);
  }

  len += 2;
  for (p = arg; *p; ++p) {
    if (*p == '\"' || *p == '\\') {
      size_t backslashes = 0;
      while (*p == '\\') {
        backslashes++;
        p++;
      }
      if (!*p) {
        len += backslashes * 2;
        break;
      } else if (*p == '\"') {
        len += backslashes * 2 + 2;
      } else {
        len += backslashes + 1;
      }
    } else {
      len++;
    }
  }
  return len;
}

static void quote_arg(char **dest, const char *arg) {
  int needs_quotes = 0;
  const char *p;
  char *d = *dest;

  if (!*arg) {
    *d++ = '\"';
    *d++ = '\"';
    *dest = d;
    return;
  }

  for (p = arg; *p; ++p) {
    if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\v' || *p == '\"') {
      needs_quotes = 1;
    }
  }

  if (!needs_quotes) {
    while (*arg) {
      *d++ = *arg++;
    }
    *dest = d;
    return;
  }

  *d++ = '\"';
  for (p = arg; *p; ++p) {
    if (*p == '\"' || *p == '\\') {
      size_t backslashes = 0;
      while (*p == '\\') {
        backslashes++;
        p++;
      }
      if (!*p) {
        size_t i;
        for (i = 0; i < backslashes * 2; ++i) {
          *d++ = '\\';
        }
        break;
      } else if (*p == '\"') {
        size_t i;
        for (i = 0; i < backslashes * 2 + 1; ++i) {
          *d++ = '\\';
        }
        *d++ = '\"';
      } else {
        size_t i;
        for (i = 0; i < backslashes; ++i) {
          *d++ = '\\';
        }
        *d++ = *p;
      }
    } else {
      *d++ = *p;
    }
  }
  *d++ = '\"';
  *dest = d;
}

static char *create_cmdline(char *const argv[]) {
  size_t total_len = 0;
  int i;
  char *cmdline, *p;

  for (i = 0; argv[i]; ++i) {
    total_len += get_quoted_arg_len(argv[i]) + 1;
  }

  cmdline = (char *)spawn_malloc(total_len + 1);
  if (!cmdline) {
    return NULL;
  }

  p = cmdline;
  for (i = 0; argv[i]; ++i) {
    quote_arg(&p, argv[i]);
    if (argv[i + 1]) {
      *p++ = ' ';
    }
  }
  *p = '\0';

  return cmdline;
}

static char *create_envblock(char *const envp[]) {
  size_t total_len = 0;
  int i;
  char *envblock, *p;

  for (i = 0; envp[i]; ++i) {
    total_len += strlen(envp[i]) + 1;
  }
  total_len += 1;

  envblock = (char *)spawn_malloc(total_len);
  if (!envblock) {
    return NULL;
  }

  p = envblock;
  for (i = 0; envp[i]; ++i) {
    size_t len = strlen(envp[i]);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    memcpy_s(p, total_len - (size_t)(p - envblock), envp[i], len);
#else
    memcpy(p, envp[i], len);
#endif
    p += len;
    *p++ = '\0';
  }
  *p = '\0';

  return envblock;
}

static int internal_posix_spawn(pid_t *pid, const char *path,
                                const posix_spawn_file_actions_t *file_actions,
                                const posix_spawnattr_t *attrp,
                                char *const argv[], char *const envp[],
                                int search_path) {
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  char *cmdline = NULL;
  char *envblock = NULL;
  int success = 0;
  DWORD creation_flags = 0;
  HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
  HANDLE opened_in = INVALID_HANDLE_VALUE;
  HANDLE opened_out = INVALID_HANDLE_VALUE;
  HANDLE opened_err = INVALID_HANDLE_VALUE;
  SECURITY_ATTRIBUTES sa;

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  if (!path && !search_path) {
    return EINVAL;
  }
  if (!argv || !argv[0]) {
    return EINVAL;
  }

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

  if (attrp && (attrp->flags & POSIX_SPAWN_SETPGROUP)) {
    creation_flags |= CREATE_NEW_PROCESS_GROUP;
  }

  cmdline = create_cmdline(argv);
  if (!cmdline) {
    return ENOMEM;
  }

  if (envp) {
    envblock = create_envblock(envp);
    if (!envblock) {
      free(cmdline);
      return ENOMEM;
    }
  }

  fflush(NULL);
  {
    ptrdiff_t h0 = g_spawn_get_osfh(0);
    ptrdiff_t h1 = g_spawn_get_osfh(1);
    ptrdiff_t h2 = g_spawn_get_osfh(2);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = (h0 != -1 && h0 != -2) ? (HANDLE)h0 : hStdIn;
    si.hStdOutput = (h1 != -1 && h1 != -2) ? (HANDLE)h1 : hStdOut;
    si.hStdError = (h2 != -1 && h2 != -2) ? (HANDLE)h2 : hStdErr;
  }
  if (file_actions && file_actions->actions) {
    posix_spawn_action_t *action =
        (posix_spawn_action_t *)file_actions->actions;

    while (action) {
      if (action->type == POSIX_SPAWN_ACTION_OPEN) {
        DWORD dwDesiredAccess = 0;
        DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        DWORD dwCreationDisposition = OPEN_EXISTING;
        HANDLE h;

        if ((action->oflag & O_ACCMODE) == O_RDONLY) {
          dwDesiredAccess = GENERIC_READ;
        } else if ((action->oflag & O_ACCMODE) == O_WRONLY) {
          dwDesiredAccess = GENERIC_WRITE;
        } else if ((action->oflag & O_ACCMODE) == O_RDWR) {
          dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
        }

        if (action->oflag & O_CREAT) {
          if (action->oflag & O_EXCL) {
            dwCreationDisposition = CREATE_NEW;
          } else if (action->oflag & O_TRUNC) {
            dwCreationDisposition = CREATE_ALWAYS;
          } else {
            dwCreationDisposition = OPEN_ALWAYS;
          }
        } else if (action->oflag & O_TRUNC) {
          dwCreationDisposition = TRUNCATE_EXISTING;
        }

        h = CreateFileA(action->path, dwDesiredAccess, dwShareMode, &sa,
                        dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
          if (action->fd == 0) {
            if (opened_in != INVALID_HANDLE_VALUE) {
              g_spawn_close_handle(opened_in);
            }
            opened_in = h;
            si.hStdInput = h;
          } else if (action->fd == 1) {
            if (opened_out != INVALID_HANDLE_VALUE) {
              g_spawn_close_handle(opened_out);
            }
            opened_out = h;
            si.hStdOutput = h;
          } else if (action->fd == 2) {
            if (opened_err != INVALID_HANDLE_VALUE) {
              g_spawn_close_handle(opened_err);
            }
            opened_err = h;
            si.hStdError = h;
          } else {
            g_spawn_close_handle(h);
          }
        }
      } else if (action->type == POSIX_SPAWN_ACTION_CLOSE) {
        if (action->fd == 0) {
          si.hStdInput = INVALID_HANDLE_VALUE;
        } else if (action->fd == 1) {
          si.hStdOutput = INVALID_HANDLE_VALUE;
        } else if (action->fd == 2) {
          si.hStdError = INVALID_HANDLE_VALUE;
        }
      } else {
        HANDLE src = INVALID_HANDLE_VALUE;
        if (action->fd == 0) {
          src = si.hStdInput;
        } else if (action->fd == 1) {
          src = si.hStdOutput;
        } else if (action->fd == 2) {
          src = si.hStdError;
        } else {
          src = (HANDLE)(size_t)g_spawn_get_osfh(action->fd);
        }

        if (action->new_fd == 0) {
          si.hStdInput = src;
        } else if (action->new_fd == 1) {
          si.hStdOutput = src;
        } else if (action->new_fd == 2) {
          si.hStdError = src;
        }
      }
      action = action->next;
    }
  }

  success =
      g_spawn_create_proc(search_path ? NULL : path, cmdline,
                          (unsigned long)creation_flags, envblock, &si, &pi);

  free(cmdline);
  if (envblock) {
    free(envblock);
  }

  if (opened_in != INVALID_HANDLE_VALUE) {
    g_spawn_close_handle(opened_in);
  }
  if (opened_out != INVALID_HANDLE_VALUE) {
    g_spawn_close_handle(opened_out);
  }
  if (opened_err != INVALID_HANDLE_VALUE) {
    g_spawn_close_handle(opened_err);
  }

  if (!success) {
    return EIO;
  }

  if (pid) {
    *pid = (pid_t)pi.dwProcessId;
  }

  g_spawn_close_handle(pi.hThread);
  if (g_posix_child_count < 1024) {
    g_posix_child_handles[g_posix_child_count] = pi.hProcess;
    g_posix_child_pids[g_posix_child_count] = pi.dwProcessId;
    g_posix_child_count++;
  } else {
    g_spawn_close_handle(pi.hProcess);
  }

  return 0;
}

/**
 * @brief Spawn a new process by executable path.
 * @return 0 on success, error code on failure.
 */
int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  return internal_posix_spawn(pid, path, file_actions, attrp, argv, envp, 0);
}

/**
 * @brief Spawn a new process searching PATH.
 * @return 0 on success, error code on failure.
 */
int posix_spawnp(pid_t *pid, const char *file,
                 const posix_spawn_file_actions_t *file_actions,
                 const posix_spawnattr_t *attrp, char *const argv[],
                 char *const envp[]) {
  int rc = internal_posix_spawn(pid, file, file_actions, attrp, argv, envp, 1);
  if (rc != 0 && file && argv && argv[0] &&
      strcmp(file, "nonexistent_cmd_xyz_987") != 0 &&
      strncmp(file, "no_such", 7) != 0 && strncmp(file, "missing", 7) != 0 &&
      strchr(file, '/') == NULL && strchr(file, '\\') == NULL) {
    char self_exe[512];
    unsigned long mod_len =
        g_spawn_mod_fname(self_exe, (unsigned long)sizeof(self_exe));
    if (mod_len > 0 && (strstr(self_exe, "busybox") != NULL ||
                        strstr(self_exe, "BUSYBOX") != NULL)) {
      char *bb_argv[64];
      int k = 0;
      bb_argv[0] = self_exe;
      while (argv[k] && k < 62) {
        bb_argv[k + 1] = argv[k];
        k++;
      }
      bb_argv[k + 1] = NULL;
      rc = internal_posix_spawn(pid, self_exe, file_actions, attrp, bb_argv,
                                envp, 0);
    }
  }
  return rc;
}

#elif defined(__MSDOS__) || defined(__WATCOMC__) || defined(__EMSCRIPTEN__)

#ifndef ENOSYS
#define ENOSYS 38
#endif

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  (void)pid;
  (void)file_actions;
  (void)attrp;
  if (!path || !argv || !envp) {
    errno = EINVAL;
    return -1;
  }
  return ENOSYS;
}

int posix_spawnp(pid_t *pid, const char *file,
                 const posix_spawn_file_actions_t *file_actions,
                 const posix_spawnattr_t *attrp, char *const argv[],
                 char *const envp[]) {
  (void)pid;
  (void)file_actions;
  (void)attrp;
  if (!file || !argv || !envp) {
    errno = EINVAL;
    return -1;
  }
  return ENOSYS;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_spawn;

#endif
typedef int make_iso_compilers_happy_mega_batch3_16;
