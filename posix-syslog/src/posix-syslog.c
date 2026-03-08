/* posix-syslog.c - Strict C89 Implementation */
#include "posix-syslog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
/* Typedefs to avoid including windows.h */
typedef void *WIN_HANDLE;
typedef unsigned short WIN_WORD;
typedef unsigned long WIN_DWORD;
typedef const char *WIN_LPCSTR;
typedef void *WIN_PSID;

#define WIN_EVENTLOG_ERROR_TYPE       0x0001
#define WIN_EVENTLOG_WARNING_TYPE     0x0002
#define WIN_EVENTLOG_INFORMATION_TYPE 0x0004

/* Declarations for Advapi32 functions */
__declspec(dllimport) WIN_HANDLE __stdcall RegisterEventSourceA(WIN_LPCSTR lpUNCServerName, WIN_LPCSTR lpSourceName);
__declspec(dllimport) int __stdcall DeregisterEventSource(WIN_HANDLE hEventLog);
__declspec(dllimport) int __stdcall ReportEventA(
    WIN_HANDLE hEventLog,
    WIN_WORD wType,
    WIN_WORD wCategory,
    WIN_DWORD dwEventID,
    WIN_PSID lpUserSid,
    WIN_WORD wNumStrings,
    WIN_DWORD dwDataSize,
    WIN_LPCSTR *lpStrings,
    void *lpRawData
);
#endif

/* Global state */
#ifdef _WIN32
static WIN_HANDLE g_EventSource = NULL;
#endif
static char* g_Ident = NULL;
static int g_LogOpt = 0;
static int g_Facility = LOG_USER;
static int g_LogMask = 0xFF; /* All priorities by default */

void closelog(void) {
#ifdef _WIN32
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

/** \brief openlog function. */
void openlog(const char *ident, int option, int facility) {
    closelog(); /* Clean up previous state if any */

    if (ident != NULL) {
        size_t len;
        len = strlen(ident);
        g_Ident = (char*)malloc(len + 1);
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

#ifdef _WIN32
    if (g_LogOpt & LOG_NDELAY) {
        g_EventSource = RegisterEventSourceA(NULL, g_Ident != NULL ? g_Ident : "Application");
    }
#endif
}

/** \brief setlogmask function. */
int setlogmask(int mask) {
    int old_mask = g_LogMask;
    if (mask != 0) {
        g_LogMask = mask;
    }
    return old_mask;
}

/** \brief syslog function. */
void syslog(int priority, const char *format, ...) {
    va_list args;
    char buffer[4096];
    int prio = LOG_PRI(priority);
    int written = 0;

#ifdef _WIN32
    WIN_WORD eventType;
    WIN_LPCSTR strings[1];
#endif

    if (!(g_LogMask & LOG_MASK(prio))) {
        return;
    }

    va_start(args, format);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
#elif defined(_MSC_VER)
    written = _vsnprintf(buffer, sizeof(buffer), format, args);
#else
    /* Fallback for other platforms */
    written = vsprintf(buffer, format, args);
#endif
    va_end(args);

    if (written < 0) {
        return;
    }

    /* Print to stderr if LOG_PERROR is set */
    if (g_LogOpt & LOG_PERROR) {
        fprintf(stderr, "%s%s%s\n", g_Ident != NULL ? g_Ident : "", g_Ident != NULL ? ": " : "", buffer);
    }

#ifdef _WIN32
    strings[0] = buffer;

    switch (prio) {
        case LOG_EMERG:
        case LOG_ALERT:
        case LOG_CRIT:
        case LOG_ERR:
            eventType = WIN_EVENTLOG_ERROR_TYPE;
            break;
        case LOG_WARNING:
            eventType = WIN_EVENTLOG_WARNING_TYPE;
            break;
        case LOG_NOTICE:
        case LOG_INFO:
        case LOG_DEBUG:
        default:
            eventType = WIN_EVENTLOG_INFORMATION_TYPE;
            break;
    }

    if (g_EventSource == NULL) {
        g_EventSource = RegisterEventSourceA(NULL, g_Ident != NULL ? g_Ident : "Application");
    }

    if (g_EventSource != NULL) {
        ReportEventA(g_EventSource, eventType, 0, 0, NULL, 1, 0, strings, NULL);
    }
#else
    /* Non-Windows stub */
    (void)priority;
    (void)format;
#endif
}

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_syslog(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_syslog;

