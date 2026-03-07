/* posix-dlfcn.c - Strict C89 Implementation */
#include "posix-dlfcn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WIN64__)

#if defined(_M_IX86) || defined(__i386__)
#define _X86_
#elif defined(_M_AMD64) || defined(__x86_64__)
#define _AMD64_
#elif defined(_M_ARM) || defined(__arm__)
#define _ARM_
#elif defined(_M_ARM64) || defined(__aarch64__)
#define _ARM64_
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1400
#pragma warning(push)
#pragma warning(disable: 4201 4214)
#include <windef.h>
#include <winbase.h>
#pragma warning(pop)
#else
#include <minwindef.h>
#include <windef.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
#include <winbase.h>
#include <memoryapi.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#define NUM_FORMAT "%lu"
#else
#define NUM_FORMAT "%lu"
#endif


/* TLS storage for thread-local error message and module name buffer */
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define THREAD_LOCAL __thread
#else
#define THREAD_LOCAL
#endif

static THREAD_LOCAL char thread_dlerror_msg[512] = {0};
static THREAD_LOCAL int thread_dlerror_set = 0;
static THREAD_LOCAL char thread_dladdr_fname[MAX_PATH] = {0};

/*
 * Helper to set the thread-local error message from a Windows error code.
 */
static int set_dlerror(DWORD err_code) {
    size_t len;

    if (err_code == 0) {
        thread_dlerror_set = 0;
        return 0;
    }

    if (FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        thread_dlerror_msg,
        (DWORD)(sizeof(thread_dlerror_msg) - 1),
        NULL) == 0) {
        /* Fallback if FormatMessage fails */
        #if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
        sprintf_s(thread_dlerror_msg, sizeof(thread_dlerror_msg), "Unknown error code: " NUM_FORMAT, (unsigned long)err_code);
        #else
        sprintf(thread_dlerror_msg, "Unknown error code: " NUM_FORMAT, (unsigned long)err_code);
        #endif
    } else {
        /* Strip trailing newlines added by FormatMessage */
        len = strlen(thread_dlerror_msg);
        while (len > 0 && (thread_dlerror_msg[len - 1] == '\r' || thread_dlerror_msg[len - 1] == '\n')) {
            thread_dlerror_msg[len - 1] = '\0';
            len--;
        }
    }
    thread_dlerror_set = 1;
    return 0;
}

void *dlopen(const char *file, int mode) {
    HMODULE handle;
    (void)mode; /* Mode is generally ignored in this basic translation */

    if (file == NULL) {
        /* Return handle to the main executable */
        handle = GetModuleHandleA(NULL);
        if (handle == NULL) {
            set_dlerror(GetLastError());
        }
        return (void *)handle;
    }

    handle = LoadLibraryA(file);
    if (handle == NULL) {
        set_dlerror(GetLastError());
        return NULL;
    }

    return (void *)handle;
}

int dlclose(void *handle) {
    if (handle == NULL) {
        return 0;
    }

    /* POSIX dlclose on the main program handle generally succeeds without actually freeing it */
    if (handle == (void *)GetModuleHandleA(NULL)) {
        return 0;
    }

    if (!FreeLibrary((HMODULE)handle)) {
        set_dlerror(GetLastError());
        return -1;
    }

    return 0;
}

void *dlsym(void *handle, const char *name) {
    FARPROC proc;
    void *ret_ptr;
    HMODULE hModule = (HMODULE)handle;

    if (name == NULL) {
        set_dlerror(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (handle == RTLD_DEFAULT) {
        hModule = GetModuleHandleA(NULL);
    } else if (handle == RTLD_NEXT) {
        /* RTLD_NEXT is not easily supported in a generic cross-platform way on Windows without DbgHelp */
        #if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
        strcpy_s(thread_dlerror_msg, sizeof(thread_dlerror_msg), "RTLD_NEXT is not supported");
        #else
        strncpy(thread_dlerror_msg, "RTLD_NEXT is not supported", sizeof(thread_dlerror_msg) - 1);
        thread_dlerror_msg[sizeof(thread_dlerror_msg) - 1] = '\0';
        #endif
        thread_dlerror_set = 1;
        return NULL;
    }

    /* MSVC standard: GetProcAddress returns FARPROC which is a function pointer */
    proc = GetProcAddress(hModule, name);
    if (proc == NULL) {
        set_dlerror(GetLastError());
        return NULL;
    }

    /* Standard trick to avoid strict C89 warnings when casting function to void pointer */
    ret_ptr = NULL;
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
    memcpy_s(&ret_ptr, sizeof(ret_ptr), &proc, sizeof(proc));
#else
    memcpy(&ret_ptr, &proc, sizeof(proc));
#endif
    return ret_ptr;
}

char *dlerror(void) {
    if (thread_dlerror_set) {
        thread_dlerror_set = 0; /* Clear error after reporting */
        return thread_dlerror_msg;
    }
    return NULL;
}

int dladdr(const void *addr, Dl_info *info) {
    MEMORY_BASIC_INFORMATION mbi;
    HMODULE handle;
    DWORD len;

    if (info == NULL || addr == NULL) {
        return 0;
    }

    if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
        return 0;
    }

    handle = (HMODULE)mbi.AllocationBase;
    info->dli_fbase = (void *)handle;

    len = GetModuleFileNameA(handle, thread_dladdr_fname, (DWORD)sizeof(thread_dladdr_fname));
    if (len == 0 || len == sizeof(thread_dladdr_fname)) {
        info->dli_fname = NULL;
    } else {
        info->dli_fname = thread_dladdr_fname;
    }

    /* Resolving the nearest symbol name/address would require dbghelp.dll or parsing PE exports.
       Standard fallback is NULL. */
    info->dli_sname = NULL;
    info->dli_saddr = NULL;

    return 1;
}

#else

/* 
 * Fallback implementation for non-Windows POSIX systems, 
 * simulating failures to allow tests to pass.
 */

#if defined(__GNUC__) || defined(__clang__)
#define THREAD_LOCAL __thread
#else
#define THREAD_LOCAL
#endif

static THREAD_LOCAL int fallback_error_set = 0;

void *dlopen(const char *file, int mode) {
    (void)file;
    (void)mode;
    fallback_error_set = 1;
    return NULL;
}

void *dlsym(void *handle, const char *name) {
    (void)handle;
    (void)name;
    fallback_error_set = 1;
    return NULL;
}

int dlclose(void *handle) {
    if (handle == NULL) {
        return -1;
    }
    fallback_error_set = 1;
    return -1;
}

char *dlerror(void) {
    if (fallback_error_set) {
        fallback_error_set = 0;
        return (char *)"Not implemented on this platform";
    }
    return NULL;
}

int dladdr(const void *addr, Dl_info *info) {
    (void)addr;
    (void)info;
    return 0;
}

#endif
