#include <linux-execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#if defined(_MSC_VER)
/* We try to avoid <windows.h> by forward declaring the necessary parts from WinBase/DbgHelp */
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef void* PVOID;
typedef unsigned long* PULONG;
typedef void* HANDLE;
typedef int BOOL;
typedef unsigned __int64 DWORD64;
typedef unsigned long DWORD;

#define MAX_SYM_NAME 2000
#define FALSE 0
#define TRUE 1

#ifndef STD_ERR_HANDLE
/** \brief STD_ERR_HANDLE macro. */
#define STD_ERR_HANDLE ((DWORD)-12)
#endif

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) USHORT __stdcall CaptureStackBackTrace(ULONG FramesToSkip, ULONG FramesToCapture, PVOID *BackTrace, PULONG BackTraceHash);
__declspec(dllimport) HANDLE __stdcall GetCurrentProcess(void);
__declspec(dllimport) BOOL __stdcall SymInitialize(HANDLE hProcess, const char *UserSearchPath, BOOL fInvadeProcess);
__declspec(dllimport) BOOL __stdcall SymSetOptions(DWORD SymOptions);
__declspec(dllimport) DWORD __stdcall SymGetOptions(void);
__declspec(dllimport) PVOID __stdcall GetStdHandle(DWORD nStdHandle);

/* WriteFile */
__declspec(dllimport) BOOL __stdcall WriteFile(
    HANDLE       hFile,
    const void   *lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    DWORD        *lpNumberOfBytesWritten,
    void         *lpOverlapped
);

/* DbgHelp structs */
#pragma pack(push, 8)
typedef struct _SYMBOL_INFO {
    ULONG       SizeOfStruct;
    ULONG       TypeIndex;        
    DWORD64     Reserved[2];
    ULONG       Index;
    ULONG       Size;
    DWORD64     ModBase;          
    ULONG       Flags;
    DWORD64     Value;            
    DWORD64     Address;          
    ULONG       Register;
    ULONG       Scope;
    ULONG       Tag;
    ULONG       NameLen;
    ULONG       MaxNameLen;
    char        Name[1];          
} SYMBOL_INFO, *PSYMBOL_INFO;
#pragma pack(pop)

#define SYMOPT_DEFERRED_LOADS 0x00000004
#define SYMOPT_UNDNAME 0x00000002

__declspec(dllimport) BOOL __stdcall SymFromAddr(HANDLE hProcess, DWORD64 Address, DWORD64 *Displacement, PSYMBOL_INFO Symbol);

#ifdef __cplusplus
}
#endif

#define NUM_FORMAT "%p"

/** \brief backtrace function. */
int backtrace(void **buffer, int size) {
    if (size <= 0 || !buffer) return 0;
    return (int)CaptureStackBackTrace(1, (ULONG)size, buffer, NULL);
}

static int init_sym_done = 0;

char **backtrace_symbols(void *const *buffer, int size) {
    int i;
    char **strings;
    HANDLE process;
    char *p;
    size_t total_size = (size_t)size * sizeof(char *);
    size_t line_size = 256; /* generous estimate per line */
    
    if (size <= 0 || !buffer) return NULL;
    
    process = GetCurrentProcess();
    
    if (!init_sym_done) {
        SymSetOptions(SymGetOptions() | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(process, NULL, TRUE);
        init_sym_done = 1;
    }
    
    total_size += (size_t)size * line_size;
    strings = (char **)malloc(total_size);
    if (!strings) return NULL;
    
    p = (char *)(strings + size);
    
    for (i = 0; i < size; i++) {
        DWORD64 displacement = 0;
        char buffer_sym[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(char)] = {0};
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer_sym;
        
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        strings[i] = p;
        if (SymFromAddr(process, (DWORD64)(size_t)buffer[i], &displacement, symbol)) {
            sprintf_s(p, line_size, "%s [%p]", symbol->Name, buffer[i]);
        } else {
            sprintf_s(p, line_size, "??? [%p]", buffer[i]);
        }
        p += strlen(p) + 1;
    }
    
    return strings;
}

/** \brief backtrace_symbols_fd function. */
void backtrace_symbols_fd(void *const *buffer, int size, int fd) {
    int i;
    char **strings;
    DWORD written;
    HANDLE hOut;
    
    (void)fd; /* Windows doesn't easily map typical POSIX fd to HANDLE directly here, default to stderr */
    
    strings = backtrace_symbols(buffer, size);
    if (!strings) return;
    
    hOut = GetStdHandle(STD_ERR_HANDLE);
    
    for (i = 0; i < size; i++) {
        size_t len = strlen(strings[i]);
        WriteFile(hOut, strings[i], (DWORD)len, &written, NULL);
        WriteFile(hOut, "\n", 1, &written, NULL);
    }
    
    free(strings);
}

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_linux_execinfo(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_execinfo;

