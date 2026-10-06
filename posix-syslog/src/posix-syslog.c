/**
 * @file posix-syslog.c
 * @brief Implementation of posix-syslog compatibility functions.
 */

/* clang-format off */
#include "posix-syslog.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_MSC_VER) && !defined(vsnprintf) && !defined(__APPLE__)
extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif

#if defined(_WIN32)
typedef void *WIN_HANDLE;
typedef unsigned short WIN_WORD;
typedef unsigned long WIN_DWORD;
typedef const char *WIN_LPCSTR;
typedef void *WIN_PSID;

#define WIN_EVENTLOG_ERROR_TYPE 0x0001
#define WIN_EVENTLOG_WARNING_TYPE 0x0002
#define WIN_EVENTLOG_INFORMATION_TYPE 0x0004

#if !defined(MOCK_EVENTLOG)
__declspec(dllimport) WIN_HANDLE __stdcall
RegisterEventSourceA(WIN_LPCSTR lpUNCServerName, WIN_LPCSTR lpSourceName);
__declspec(dllimport) int __stdcall DeregisterEventSource(WIN_HANDLE hEventLog);
__declspec(dllimport) int __stdcall
ReportEventA(WIN_HANDLE hEventLog, WIN_WORD wType, WIN_WORD wCategory,
             WIN_DWORD dwEventID, WIN_PSID lpUserSid, WIN_WORD wNumStrings,
             WIN_DWORD dwDataSize, WIN_LPCSTR *lpStrings, void *lpRawData);
#endif
#endif
/* clang-format on */

#if defined(_WIN32)
static WIN_HANDLE g_EventSource = NULL;
#endif
static char *g_Ident = NULL;
static int g_LogOpt = 0;
static int g_Facility = LOG_USER;
static int g_LogMask = 0xFF;

#if defined(_WIN32) && !defined(DEFINED_WIN32_FOR_TEST)
__declspec(dllexport)
#endif
void *(*posix_syslog_mock_malloc_ptr)(size_t) = NULL;

#define LOCAL_MALLOC(size)                                                     \
  (posix_syslog_mock_malloc_ptr ? posix_syslog_mock_malloc_ptr(size)           \
                                : malloc(size))

/**
 * @brief Retrieves information on posix-syslog availability.
 */
auto_win_msvc_error_t posix_syslog_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

void closelog(void) {
#if defined(_WIN32)
  if (g_EventSource != NULL) {
    DeregisterEventSource(g_EventSource);
    g_EventSource = NULL;
  }
#endif
  if (g_Ident != NULL) {
    free(g_Ident);
    g_Ident = NULL;
  }
}

void openlog(const char *ident, int option, int facility) {
  closelog();

  if (ident != NULL) {
    size_t len;
    len = strlen(ident);
    g_Ident = (char *)LOCAL_MALLOC(len + 1);
    if (g_Ident != NULL) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
      strncpy_s(g_Ident, len + 1, ident, _TRUNCATE);
#else
      strncpy(g_Ident, ident, len + 1);
      g_Ident[len] = '\0';
#endif
    }
  }

  g_LogOpt = option;
  g_Facility = facility;

#if defined(_WIN32)
  if (g_LogOpt & LOG_NDELAY) {
    g_EventSource =
        RegisterEventSourceA(NULL, g_Ident != NULL ? g_Ident : "Application");
  }
#endif
}

int setlogmask(int mask) {
  int old_mask;
  old_mask = g_LogMask;
  if (mask != 0) {
    g_LogMask = mask;
  }
  return old_mask;
}

void syslog(int priority, const char *format, ...) {
  va_list args;
  char buffer[4096];
  int prio;
#if defined(_WIN32)
  WIN_WORD eventType;
  WIN_LPCSTR strings[1];
#endif

  prio = LOG_PRI(priority);

  if (!(g_LogMask & LOG_MASK(prio))) {
    return;
  }

  va_start(args, format);
#if defined(_MSC_VER) && _MSC_VER >= 1400
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
#elif defined(_MSC_VER)
  _vsnprintf(buffer, sizeof(buffer), format, args);
  buffer[sizeof(buffer) - 1] = '\0';
#else
  vsnprintf(buffer, sizeof(buffer), format, args);
#endif
  va_end(args);

  if (g_LogOpt & LOG_PERROR) {
    fprintf(stderr, "%s%s%s\n", g_Ident != NULL ? g_Ident : "",
            g_Ident != NULL ? ": " : "", buffer);
  }

#if defined(_WIN32)
  strings[0] = buffer;

  if (prio <= LOG_ERR) {
    eventType = WIN_EVENTLOG_ERROR_TYPE;
  } else if (prio == LOG_WARNING) {
    eventType = WIN_EVENTLOG_WARNING_TYPE;
  } else {
    eventType = WIN_EVENTLOG_INFORMATION_TYPE;
  }

  if (g_EventSource == NULL) {
    g_EventSource =
        RegisterEventSourceA(NULL, g_Ident != NULL ? g_Ident : "Application");
  }

  if (g_EventSource != NULL) {
    ReportEventA(g_EventSource, eventType, 0, 0, NULL, 1, 0, strings, NULL);
  }
#endif
}

typedef int make_iso_compilers_happy_tu_posix_syslog;
