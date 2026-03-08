#include "posix-sys-uio.h"

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <winsock2.h>
#include <io.h>
#include <errno.h>
#include <stdlib.h>

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

/** \brief posix_writev function. */
long posix_writev(int fd, const struct iovec *iov, int iovcnt) {
    DWORD bytesSent = 0;
    int ret;
    WSABUF *bufs;
    int i;
    
    if (iovcnt < 0 || iovcnt > 1024) {
        errno = EINVAL;
        return -1;
    }
    
    bufs = (WSABUF *)malloc(iovcnt * sizeof(WSABUF));
    if (!bufs) {
        errno = ENOMEM;
        return -1;
    }
    
    for (i = 0; i < iovcnt; i++) {
        bufs[i].buf = (char *)iov[i].iov_base;
        bufs[i].len = (ULONG)iov[i].iov_len;
    }
    
    ret = WSASend((SOCKET)fd, bufs, (DWORD)iovcnt, &bytesSent, 0, NULL, NULL);
    free(bufs);
    
    if (ret == SOCKET_ERROR) {
        int wsaErr = WSAGetLastError();
        if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
            long total = 0;
            for (i = 0; i < iovcnt; i++) {
                int written = _write(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
                if (written < 0) {
                    if (total > 0) return total;
                    return -1;
                }
                total += written;
            }
            return total;
        }
        
        if (wsaErr == WSAEWOULDBLOCK) errno = EWOULDBLOCK;
        else errno = EINVAL;
        
        return -1;
    }
    
    return bytesSent;
}

/** \brief posix_readv function. */
long posix_readv(int fd, const struct iovec *iov, int iovcnt) {
    DWORD bytesRecv = 0;
    DWORD flags = 0;
    int ret;
    WSABUF *bufs;
    int i;
    
    if (iovcnt < 0 || iovcnt > 1024) {
        errno = EINVAL;
        return -1;
    }
    
    bufs = (WSABUF *)malloc(iovcnt * sizeof(WSABUF));
    if (!bufs) {
        errno = ENOMEM;
        return -1;
    }
    
    for (i = 0; i < iovcnt; i++) {
        bufs[i].buf = (char *)iov[i].iov_base;
        bufs[i].len = (ULONG)iov[i].iov_len;
    }
    
    ret = WSARecv((SOCKET)fd, bufs, (DWORD)iovcnt, &bytesRecv, &flags, NULL, NULL);
    free(bufs);
    
    if (ret == SOCKET_ERROR) {
        int wsaErr = WSAGetLastError();
        if (wsaErr == WSAENOTSOCK || wsaErr == WSANOTINITIALISED) {
            long total = 0;
            for (i = 0; i < iovcnt; i++) {
                int bytes_read = _read(fd, iov[i].iov_base, (unsigned int)iov[i].iov_len);
                if (bytes_read < 0) {
                    if (total > 0) return total;
                    return -1;
                }
                total += bytes_read;
                if ((size_t)bytes_read < iov[i].iov_len) {
                    break;
                }
            }
            return total;
        }
        
        if (wsaErr == WSAEWOULDBLOCK) errno = EWOULDBLOCK;
        else errno = EINVAL;
        
        return -1;
    }
    
    return bytesRecv;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sys_uio(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sys_uio;

