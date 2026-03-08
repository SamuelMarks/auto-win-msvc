#include "greatest.h"
#include "posix-sys-uio.h"

#if defined(_MSC_VER) || defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#include <string.h>
#include <winsock2.h>


TEST test_sys_uio(void) {
    int fd;
    struct iovec iov[2];
    char buf1[] = "Hello ";
    char buf2[] = "World";
    char out1[10] = {0};
    char out2[10] = {0};
    long written, r;

#if defined(__STDC_LIB_EXT1__) || defined(_MSC_VER)
    _sopen_s(&fd, "test_uio.txt", _O_CREAT | _O_RDWR | _O_TRUNC, _SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
    fd = _open("test_uio.txt", _O_CREAT | _O_RDWR | _O_TRUNC, _S_IREAD | _S_IWRITE);
#endif

    ASSERT(fd >= 0);

    iov[0].iov_base = buf1;
    iov[0].iov_len = 6;
    iov[1].iov_base = buf2;
    iov[1].iov_len = 5;

    written = posix_writev(fd, iov, 2);
    ASSERT_EQ_FMT(11L, written, "%ld");

    _lseek(fd, 0, SEEK_SET);

    iov[0].iov_base = out1;
    iov[0].iov_len = 6;
    iov[1].iov_base = out2;
    iov[1].iov_len = 5;

    r = posix_readv(fd, iov, 2);
    ASSERT_EQ_FMT(11L, r, "%ld");

    ASSERT_STR_EQ("Hello ", out1);
    ASSERT_STR_EQ("World", out2);

    _close(fd);
    _unlink("test_uio.txt");

    PASS();
}

SUITE(sys_uio_suite) {
    RUN_TEST(test_sys_uio);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    WSADATA wsaData;
    GREATEST_MAIN_BEGIN();
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    RUN_SUITE(sys_uio_suite);
    WSACleanup();
    GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif
