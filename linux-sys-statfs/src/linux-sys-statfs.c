#include <linux-sys-statfs.h>

#if defined(_MSC_VER)

typedef int BOOL;
typedef unsigned __int64 ULONGLONG;

typedef union _ULARGE_INTEGER {
    struct {
        unsigned long LowPart;
        unsigned long HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

__declspec(dllimport) BOOL __stdcall GetDiskFreeSpaceExA(
    const char *lpDirectoryName,
    ULARGE_INTEGER *lpFreeBytesAvailableToCaller,
    ULARGE_INTEGER *lpTotalNumberOfBytes,
    ULARGE_INTEGER *lpTotalNumberOfFreeBytes
);

int statfs(const char *path, struct statfs *buf) {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    
    if (!buf || !path) return -1;
    
    if (GetDiskFreeSpaceExA(path, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        buf->f_type = 0; /* Unknown / Windows */
        buf->f_bsize = 4096;
        buf->f_blocks = (long)(totalNumberOfBytes.QuadPart / 4096);
        buf->f_bfree = (long)(totalNumberOfFreeBytes.QuadPart / 4096);
        buf->f_bavail = (long)(freeBytesAvailable.QuadPart / 4096);
        buf->f_files = 0;
        buf->f_ffree = 0;
        buf->f_namelen = 255;
        return 0;
    }
    
    return -1;
}

int fstatfs(int fd, struct statfs *buf) {
    (void)fd; /* Cannot easily get path from standard fd on Windows without handle info */
    (void)buf;
    return -1;
}

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_linux_sys_statfs(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_sys_statfs;

