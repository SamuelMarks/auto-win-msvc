#include <winsock2.h>
#include <io.h>
#include <errno.h>

#undef _read
#undef _write

int posix_read(int fd, void *buf, unsigned int count) {
    int ret = recv((SOCKET)(unsigned int)fd, buf, count, 0);
    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP) {
            return _read(fd, buf, count);
        }
        errno = err;
        return -1;
    }
    return ret;
}

int posix_write(int fd, const void *buf, unsigned int count) {
    int ret = send((SOCKET)(unsigned int)fd, buf, count, 0);
    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP) {
            return _write(fd, buf, count);
        }
        errno = err;
        return -1;
    }
    return ret;
}

#undef close

int posix_close(int fd) {
    if (fd >= 100 && fd < 1124) {
        // wepoll handle leak workaround? No, this is handled separately if needed.
    }
    SOCKET s = (SOCKET)(unsigned int)fd;
    int ret = closesocket(s);
    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAENOTSOCK || err == WSAEOPNOTSUPP) {
            return _close(fd);
        }
        errno = err;
        return -1;
    }
    return ret;
}
