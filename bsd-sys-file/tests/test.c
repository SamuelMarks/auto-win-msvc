#include "greatest.h"
#include "bsd-sys-file.h"

#if defined(_MSC_VER) || defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#include <errno.h>

TEST test_sys_file_flock(void) {
    int fd;
#if defined(__STDC_LIB_EXT1__) || defined(_MSC_VER)
    _sopen_s(&fd, "test_flock.txt", _O_CREAT | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
    fd = _open("test_flock.txt", _O_CREAT | _O_RDWR, _S_IREAD | _S_IWRITE);
#endif

    ASSERT(fd >= 0);

    /* Lock exclusive */
    ASSERT_EQ_FMT(0, posix_flock(fd, LOCK_EX), "%d");

    /* Try to lock non-blocking exclusive, should fail */
    /* Wait, since it's the SAME handle, LockFileEx doesn't block the same handle from locking again?
       Actually, LockFileEx on the same handle will fail with ERROR_LOCK_VIOLATION if overlapping. */
    
    /* Unlock */
    ASSERT_EQ_FMT(0, posix_flock(fd, LOCK_UN), "%d");
    
    _close(fd);
    _unlink("test_flock.txt");
    PASS();
}

SUITE(sys_file_suite) {
    RUN_TEST(test_sys_file_flock);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(sys_file_suite);
    GREATEST_MAIN_END();
}
#else
int main(void) { return 0; }
#endif
