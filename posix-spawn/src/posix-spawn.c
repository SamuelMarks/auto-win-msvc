/* posix-spawn.c - Strict C89 Implementation */
/* clang-format off */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posix-spawn.h"

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle(fd))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <fcntl.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
/* clang-format on */

#ifndef O_ACCMODE
/** \brief O_ACCMODE macro. */
#define O_ACCMODE (_O_RDONLY | _O_WRONLY | _O_RDWR)
#endif

/* Forward declarations to replace <windows.h> */
#define WINAPI __stdcall
#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE ((void *)(size_t)-1)
#define STD_INPUT_HANDLE ((unsigned int)-10)
/** \brief STD_OUTPUT_HANDLE macro. */
#define STD_OUTPUT_HANDLE ((unsigned int)-11)
/** \brief STD_ERROR_HANDLE macro. */
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
#define GENERIC_READ 0x80000000L
#define GENERIC_WRITE 0x40000000L
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
#define FILE_ATTRIBUTE_NORMAL 0x00000080

__declspec(dllimport) HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
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

/** \brief posix_spawn_file_actions_init function. */
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
  if (!file_actions)
    return EINVAL;
  file_actions->actions = NULL;
  return 0;
}

/** \brief posix_spawn_file_actions_destroy function. */
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
  posix_spawn_action_t *action;
  if (!file_actions)
    return EINVAL;
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

/** \brief add_action function. */
static int add_action(posix_spawn_file_actions_t *file_actions,
                      posix_spawn_action_t **out_action) {
  posix_spawn_action_t *new_action =
      (posix_spawn_action_t *)malloc(sizeof(posix_spawn_action_t));
  if (!new_action)
    return ENOMEM;
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
  return 0;
}

/** \brief posix_spawn_file_actions_addclose function. */
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
                                      int fildes) {
  posix_spawn_action_t *action;
  int err;
  if (fildes < 0)
    return EBADF;
  if (!file_actions)
    return EINVAL;
  err = add_action(file_actions, &action);
  if (err != 0)
    return err;
  action->type = POSIX_SPAWN_ACTION_CLOSE;
  action->fd = fildes;
  return 0;
}

/** \brief posix_spawn_file_actions_adddup2 function. */
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
                                     int fildes, int newfildes) {
  posix_spawn_action_t *action;
  int err;
  if (fildes < 0 || newfildes < 0)
    return EBADF;
  if (!file_actions)
    return EINVAL;
  err = add_action(file_actions, &action);
  if (err != 0)
    return err;
  action->type = POSIX_SPAWN_ACTION_DUP2;
  action->fd = fildes;
  action->new_fd = newfildes;
  return 0;
}

/** \brief posix_spawn_file_actions_addopen function. */
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *file_actions,
                                     int fildes, const char *path, int oflag,
                                     mode_t mode) {
  posix_spawn_action_t *action;
  int err;
  if (fildes < 0)
    return EBADF;
  if (!path || !file_actions)
    return EINVAL;
  err = add_action(file_actions, &action);
  if (err != 0)
    return err;
  action->type = POSIX_SPAWN_ACTION_OPEN;
  action->fd = fildes;
  action->path = (char *)malloc(strlen(path) + 1);
  if (!action->path) {
    return ENOMEM;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  if (strcpy_s(action->path, strlen(path) + 1, path) != 0)
    return ENOMEM;
#else
  strcpy(action->path, path);
#endif
  action->oflag = oflag;
  action->mode = mode;
  return 0;
}

/** \brief posix_spawnattr_init function. */
int posix_spawnattr_init(posix_spawnattr_t *attr) {
  if (!attr)
    return EINVAL;
  attr->flags = 0;
  attr->pgroup = 0;
  attr->schedparam.sched_priority = 0;
  attr->schedpolicy = 0;
  attr->sigmask = 0;
  attr->sigdefault = 0;
  return 0;
}

/** \brief posix_spawnattr_destroy function. */
int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
  if (!attr)
    return EINVAL;
  return 0;
}

/** \brief posix_spawnattr_getflags function. */
int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short *flags) {
  if (!attr || !flags)
    return EINVAL;
  *flags = attr->flags;
  return 0;
}

/** \brief posix_spawnattr_setflags function. */
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
  if (!attr)
    return EINVAL;
  attr->flags = flags;
  return 0;
}

/** \brief posix_spawnattr_getpgroup function. */
int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, pid_t *pgroup) {
  if (!attr || !pgroup)
    return EINVAL;
  *pgroup = attr->pgroup;
  return 0;
}

/** \brief posix_spawnattr_setpgroup function. */
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup) {
  if (!attr)
    return EINVAL;
  attr->pgroup = pgroup;
  return 0;
}

/** \brief posix_spawnattr_getschedparam function. */
int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr,
                                  struct sched_param *schedparam) {
  if (!attr || !schedparam)
    return EINVAL;
  *schedparam = attr->schedparam;
  return 0;
}

/** \brief posix_spawnattr_setschedparam function. */
int posix_spawnattr_setschedparam(posix_spawnattr_t *attr,
                                  const struct sched_param *schedparam) {
  if (!attr || !schedparam)
    return EINVAL;
  attr->schedparam = *schedparam;
  return 0;
}

/** \brief posix_spawnattr_getschedpolicy function. */
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr,
                                   int *schedpolicy) {
  if (!attr || !schedpolicy)
    return EINVAL;
  *schedpolicy = attr->schedpolicy;
  return 0;
}

/** \brief posix_spawnattr_setschedpolicy function. */
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy) {
  if (!attr)
    return EINVAL;
  attr->schedpolicy = schedpolicy;
  return 0;
}

/** \brief posix_spawnattr_getsigdefault function. */
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr,
                                  sigset_t *sigdefault) {
  if (!attr || !sigdefault)
    return EINVAL;
  *sigdefault = attr->sigdefault;
  return 0;
}

/** \brief posix_spawnattr_setsigdefault function. */
int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr,
                                  const sigset_t *sigdefault) {
  if (!attr || !sigdefault)
    return EINVAL;
  attr->sigdefault = *sigdefault;
  return 0;
}

/** \brief posix_spawnattr_getsigmask function. */
int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr,
                               sigset_t *sigmask) {
  if (!attr || !sigmask)
    return EINVAL;
  *sigmask = attr->sigmask;
  return 0;
}

/** \brief posix_spawnattr_setsigmask function. */
int posix_spawnattr_setsigmask(posix_spawnattr_t *attr,
                               const sigset_t *sigmask) {
  if (!attr || !sigmask)
    return EINVAL;
  attr->sigmask = *sigmask;
  return 0;
}

#if defined(_MSC_VER) || defined(__MINGW32__)

/** \brief get_quoted_arg_len function. */
static int get_quoted_arg_len(const char *arg, size_t *out_len) {
  size_t len = 0;
  int needs_quotes = 0;
  const char *p;

  if (!*arg) {
    *out_len = 2; /* "" */
    return 0;
  }

  for (p = arg; *p; ++p) {
    if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\v' || *p == '\"') {
      needs_quotes = 1;
    }
  }

  if (!needs_quotes) {
    *out_len = strlen(arg);
    return 0;
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
  *out_len = len;
  return 0;
}

/** \brief quote_arg function. */
static int quote_arg(char **dest, const char *arg) {
  int needs_quotes = 0;
  const char *p;
  char *d = *dest;

  if (!*arg) {
    *d++ = '\"';
    *d++ = '\"';
    *dest = d;
    return 0;
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
    return 0;
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
        for (i = 0; i < backslashes * 2; ++i)
          *d++ = '\\';
        break;
      } else if (*p == '\"') {
        size_t i;
        for (i = 0; i < backslashes * 2 + 1; ++i)
          *d++ = '\\';
        *d++ = '\"';
      } else {
        size_t i;
        for (i = 0; i < backslashes; ++i)
          *d++ = '\\';
        *d++ = *p;
      }
    } else {
      *d++ = *p;
    }
  }
  *d++ = '\"';
  *dest = d;
  return 0;
}

/** \brief create_cmdline function. */
static int create_cmdline(char *const argv[], char **out_cmdline) {
  size_t total_len = 0;
  int i;
  char *cmdline, *p;

  if (!argv || !argv[0])
    return EINVAL;

  for (i = 0; argv[i]; ++i) {
    size_t len = 0;
    get_quoted_arg_len(argv[i], &len);
    total_len += len + 1;
  }

  cmdline = (char *)malloc(total_len + 1);
  if (!cmdline)
    return ENOMEM;

  p = cmdline;
  for (i = 0; argv[i]; ++i) {
    quote_arg(&p, argv[i]);
    if (argv[i + 1]) {
      *p++ = ' ';
    }
  }
  *p = '\0';

  *out_cmdline = cmdline;
  return 0;
}

/** \brief create_envblock function. */
static int create_envblock(char *const envp[], char **out_envblock) {
  size_t total_len = 0;
  int i;
  char *envblock, *p;

  if (!envp)
    return EINVAL;

  for (i = 0; envp[i]; ++i) {
    total_len += strlen(envp[i]) + 1;
  }
  total_len += 1;

  envblock = (char *)malloc(total_len);
  if (!envblock)
    return ENOMEM;

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

  *out_envblock = envblock;
  return 0;
}

/** \brief internal_posix_spawn function. */
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
  SECURITY_ATTRIBUTES sa;

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  if (!path && !search_path)
    return EINVAL;
  if (!argv || !argv[0])
    return EINVAL;

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

  if (attrp) {
    if (attrp->flags & POSIX_SPAWN_SETPGROUP) {
      creation_flags |= CREATE_NEW_PROCESS_GROUP;
    }
  }

  if (create_cmdline(argv, &cmdline) != 0)
    return ENOMEM;

  if (envp) {
    if (create_envblock(envp, &envblock) != 0) {
      free(cmdline);
      return ENOMEM;
    }
  }

  if (file_actions && file_actions->actions) {
    posix_spawn_action_t *action =
        (posix_spawn_action_t *)file_actions->actions;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = hStdIn;
    si.hStdOutput = hStdOut;
    si.hStdError = hStdErr;

    while (action) {
      if (action->type == POSIX_SPAWN_ACTION_OPEN) {
        DWORD dwDesiredAccess = 0;
        DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        DWORD dwCreationDisposition = OPEN_EXISTING;
        HANDLE h;

        if ((action->oflag & O_ACCMODE) == O_RDONLY)
          dwDesiredAccess = GENERIC_READ;
        else if ((action->oflag & O_ACCMODE) == O_WRONLY)
          dwDesiredAccess = GENERIC_WRITE;
        else if ((action->oflag & O_ACCMODE) == O_RDWR)
          dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;

        if (action->oflag & O_CREAT) {
          if (action->oflag & O_EXCL)
            dwCreationDisposition = CREATE_NEW;
          else if (action->oflag & O_TRUNC)
            dwCreationDisposition = CREATE_ALWAYS;
          else
            dwCreationDisposition = OPEN_ALWAYS;
        } else if (action->oflag & O_TRUNC) {
          dwCreationDisposition = TRUNCATE_EXISTING;
        }

        h = CreateFileA(action->path, dwDesiredAccess, dwShareMode, &sa,
                        dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
          if (action->fd == 0)
            si.hStdInput = h;
          else if (action->fd == 1)
            si.hStdOutput = h;
          else if (action->fd == 2)
            si.hStdError = h;
        }
      } else if (action->type == POSIX_SPAWN_ACTION_CLOSE) {
        if (action->fd == 0)
          si.hStdInput = INVALID_HANDLE_VALUE;
        else if (action->fd == 1)
          si.hStdOutput = INVALID_HANDLE_VALUE;
        else if (action->fd == 2)
          si.hStdError = INVALID_HANDLE_VALUE;
      } else if (action->type == POSIX_SPAWN_ACTION_DUP2) {
        HANDLE src = INVALID_HANDLE_VALUE;
        if (action->fd == 0)
          src = si.hStdInput;
        else if (action->fd == 1)
          src = si.hStdOutput;
        else if (action->fd == 2)
          src = si.hStdError;
        else
          src = (HANDLE)(size_t)safe_get_osfhandle(action->fd);

        if (action->new_fd == 0)
          si.hStdInput = src;
        else if (action->new_fd == 1)
          si.hStdOutput = src;
        else if (action->new_fd == 2)
          si.hStdError = src;
      }
      action = action->next;
    }
  }

  success = CreateProcessA(search_path ? NULL : path, cmdline, NULL, NULL, TRUE,
                           creation_flags, envblock, NULL, &si, &pi);

  free(cmdline);
  if (envblock)
    free(envblock);

  /* Close newly created file handles for file actions to avoid leaks in parent
   */
  if (file_actions && file_actions->actions) {
    if (si.dwFlags & STARTF_USESTDHANDLES) {
      if (si.hStdInput != hStdIn && si.hStdInput != INVALID_HANDLE_VALUE)
        CloseHandle(si.hStdInput);
      if (si.hStdOutput != hStdOut && si.hStdOutput != INVALID_HANDLE_VALUE)
        CloseHandle(si.hStdOutput);
      if (si.hStdError != hStdErr && si.hStdError != INVALID_HANDLE_VALUE)
        CloseHandle(si.hStdError);
    }
  }

  if (!success) {
    return EIO;
  }

  if (pid) {
    *pid = (pid_t)pi.dwProcessId;
  }

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);

  return 0;
}

/** \brief posix_spawn function. */
int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  return internal_posix_spawn(pid, path, file_actions, attrp, argv, envp, 0);
}

/** \brief posix_spawnp function. */
int posix_spawnp(pid_t *pid, const char *file,
                 const posix_spawn_file_actions_t *file_actions,
                 const posix_spawnattr_t *attrp, char *const argv[],
                 char *const envp[]) {
  return internal_posix_spawn(pid, file, file_actions, attrp, argv, envp, 1);
}

#elif defined(__MSDOS__) || defined(__WATCOMC__)

#ifndef ENOSYS
#define ENOSYS 38
#endif

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  (void)pid;
  (void)path;
  (void)file_actions;
  (void)attrp;
  (void)argv;
  (void)envp;
  return ENOSYS;
}

int posix_spawnp(pid_t *pid, const char *file,
                 const posix_spawn_file_actions_t *file_actions,
                 const posix_spawnattr_t *attrp, char *const argv[],
                 char *const envp[]) {
  (void)pid;
  (void)file;
  (void)file_actions;
  (void)attrp;
  (void)argv;
  (void)envp;
  return ENOSYS;
}

#endif
/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_spawn(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_spawn;
