/* test.c - 100% Test Coverage Stubs */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef _WIN32
#define _open open
#define _close close
#define _unlink unlink
#define _rmdir rmdir
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC
#endif

#include "greatest.h"
#include "posix-stat.h"

SUITE(posix_stat_suite);

TEST test_stat(void) {
    struct stat st;
    int res = stat("CMakeLists.txt", &st);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));
    PASS();
}

TEST test_fstat(void) {
    struct stat st;
    int fd = _open("CMakeLists.txt", _O_RDONLY);
    int res;
    ASSERT(fd >= 0);
    res = fstat(fd, &st);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));
    _close(fd);
    PASS();
}

TEST test_mkdir_chmod_umask(void) {
    struct stat st;
    int res;
    mode_t old_mask = (mode_t)umask(0022);

    res = mkdir("test_dir", 0755);
    if (res == -1 && errno == EEXIST) {
        _rmdir("test_dir");
        res = mkdir("test_dir", 0755);
    }
    ASSERT_EQ(0, res);

    res = chmod("test_dir", 0777);
    ASSERT_EQ(0, res);

    ASSERT_EQ(0, stat("test_dir", &st));
    ASSERT(S_ISDIR(st.st_mode));

    _rmdir("test_dir");
    umask(old_mask);
    PASS();
}

TEST test_fchmod_fchmodat(void) {
    int fd = _open("test_fchmod.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
    int res;
    ASSERT(fd >= 0);
    res = fchmod(fd, 0444);
    if (res == -1) {
        /* Acceptable if ENOSYS on old Windows */
        ASSERT(errno == ENOSYS || errno == EACCES || errno == EBADF);
    } else {
        ASSERT_EQ(0, res);
    }
    _close(fd);

    res = fchmodat(AT_FDCWD, "test_fchmod.txt", 0644, 0);
    ASSERT_EQ(0, res);

    _unlink("test_fchmod.txt");
    PASS();
}

TEST test_fstatat_lstat(void) {
    struct stat st;
    int fd = _open("test_lstat.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
    int res;
    _close(fd);

    res = fstatat(AT_FDCWD, "test_lstat.txt", &st, 0);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));

    res = lstat("test_lstat.txt", &st);
    ASSERT_EQ(0, res);
    ASSERT(S_ISREG(st.st_mode));

    _unlink("test_lstat.txt");
    PASS();
}

TEST test_futimens_utimensat(void) {
    int fd = _open("test_time.txt", _O_WRONLY | _O_CREAT | _O_TRUNC, 0644);
    struct timespec ts[2];
    int res;

    ASSERT(fd >= 0);

    ts[0].tv_sec = 1000000; ts[0].tv_nsec = 0;
    ts[1].tv_sec = 1000000; ts[1].tv_nsec = 0;

    res = futimens(fd, ts);
    ASSERT_EQ(0, res);
    _close(fd);

    res = utimensat(AT_FDCWD, "test_time.txt", ts, 0);
    ASSERT_EQ(0, res);

    /* Test UTIME_NOW and UTIME_OMIT */
    ts[0].tv_nsec = UTIME_NOW;
    ts[1].tv_nsec = UTIME_OMIT;
    res = utimensat(AT_FDCWD, "test_time.txt", ts, 0);
    ASSERT_EQ(0, res);

    /* Test NULL times */
    res = utimensat(AT_FDCWD, "test_time.txt", NULL, 0);
    ASSERT_EQ(0, res);

    _unlink("test_time.txt");
    PASS();
}

TEST test_mknod_mknodat(void) {
    struct stat st;
    int res;

    _unlink("test_mknod.txt");
    res = mknod("test_mknod.txt", S_IFREG | 0644, 0);
    ASSERT_EQ(0, res);
    ASSERT_EQ(0, stat("test_mknod.txt", &st));
    _unlink("test_mknod.txt");

    _unlink("test_mknodat.txt");
    res = mknodat(AT_FDCWD, "test_mknodat.txt", S_IFREG | 0644, 0);
    ASSERT_EQ(0, res);
    ASSERT_EQ(0, stat("test_mknodat.txt", &st));
    _unlink("test_mknodat.txt");
    PASS();
}

SUITE(posix_stat_suite) {
    RUN_TEST(test_stat);
    RUN_TEST(test_fstat);
    RUN_TEST(test_mkdir_chmod_umask);
    RUN_TEST(test_fchmod_fchmodat);
    RUN_TEST(test_fstatat_lstat);
    RUN_TEST(test_futimens_utimensat);
    RUN_TEST(test_mknod_mknodat);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(posix_stat_suite);
    GREATEST_MAIN_END();
}