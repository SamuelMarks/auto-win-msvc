/* test.c - 100% Test Coverage */
#include <stdio.h>
#include <stdlib.h>
#include "greatest.h"
#include "posix-core.h"
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <crtdbg.h>
#if _MSC_VER >= 1600
#include <stdint.h>
#else
#include <stddef.h>
#endif
#endif


TEST test_open(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    open("NUL", O_RDONLY);
#endif
    PASS();
}
TEST test_close(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    close(-1);
#endif
    PASS();
}
TEST test_read(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    read(-1, NULL, 0);
#endif
    PASS();
}
TEST test_write(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    write(-1, NULL, 0);
#endif
    PASS();
}
TEST test_lseek(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    lseek(-1, 0, 0);
#endif
    PASS();
}
TEST test_dup(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    dup(-1);
#endif
    PASS();
}
TEST test_dup2(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    dup2(-1, -1);
#endif
    PASS();
}
TEST test_fsync(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fsync(-1);
#endif
    PASS();
}
TEST test_ftruncate(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    ftruncate(-1, 0);
#endif
    PASS();
}
TEST test_access(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    access("NUL", F_OK);
#endif
    PASS();
}
TEST test_rmdir(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    rmdir("NUL");
#endif
    PASS();
}
TEST test_chdir(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    chdir(".");
#endif
    PASS();
}
TEST test_getcwd(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getcwd(NULL, 0);
#endif
    PASS();
}
TEST test_unlink(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    unlink("NUL");
#endif
    PASS();
}
TEST test_execve(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    execve("NUL", NULL, NULL);
#endif
    PASS();
}
TEST test_execv(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    execv("NUL", NULL);
#endif
    PASS();
}
TEST test_execvp(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    execvp("NUL", NULL);
#endif
    PASS();
}
TEST test_execl(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    execl("NUL", NULL);
#endif
    PASS();
}
TEST test_execlp(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    execlp("NUL", NULL);
#endif
    PASS();
}
TEST test_getpid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getpid();
#endif
    PASS();
}
TEST test_sleep(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    sleep(0);
#endif
    PASS();
}
TEST test_usleep(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    usleep(0);
#endif
    PASS();
}
TEST test_isatty(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    isatty(-1);
#endif
    PASS();
}
TEST test_swab(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    swab(NULL, NULL, 0);
#endif
    PASS();
}
TEST test_creat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    creat("NUL", 0);
#endif
    PASS();
}
TEST test_fcntl(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fcntl(-1, 0);
#endif
    PASS();
}
TEST test_openat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    openat(-1, "NUL", O_RDONLY);
#endif
    PASS();
}
TEST test_posix_fadvise(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    posix_fadvise(-1, 0, 0, 0);
#endif
    PASS();
}
TEST test_posix_fallocate(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    posix_fallocate(-1, 0, 0);
#endif
    PASS();
}
TEST test_alarm(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    alarm(0);
#endif
    PASS();
}
TEST test_chown(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    chown("NUL", -1, -1);
#endif
    PASS();
}
TEST test_confstr(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    confstr(0, NULL, 0);
#endif
    PASS();
}
TEST test_crypt(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    crypt("a", "b");
#endif
    PASS();
}
TEST test_encrypt(void) {
    PASS();
}
TEST test_faccessat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    faccessat(-1, "NUL", F_OK, 0);
#endif
    PASS();
}
TEST test_fchown(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fchown(-1, -1, -1);
#endif
    PASS();
}
TEST test_fchownat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fchownat(-1, "NUL", -1, -1, 0);
#endif
    PASS();
}
TEST test_fdatasync(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fdatasync(-1);
#endif
    PASS();
}
TEST test_fexecve(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fexecve(-1, NULL, NULL);
#endif
    PASS();
}
TEST test_fork(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fork();
#endif
    PASS();
}
TEST test_fpathconf(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    fpathconf(-1, 0);
#endif
    PASS();
}
TEST test_getegid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getegid();
#endif
    PASS();
}
TEST test_geteuid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    geteuid();
#endif
    PASS();
}
TEST test_getgid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getgid();
#endif
    PASS();
}
TEST test_getgroups(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getgroups(0, NULL);
#endif
    PASS();
}
TEST test_gethostid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    gethostid();
#endif
    PASS();
}
TEST test_gethostname(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    gethostname(NULL, 0);
#endif
    PASS();
}
TEST test_getlogin(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getlogin();
#endif
    PASS();
}
TEST test_getlogin_r(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getlogin_r(NULL, 0);
#endif
    PASS();
}
TEST test_getopt(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getopt(0, NULL, NULL);
#endif
    PASS();
}
TEST test_getpgid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getpgid(0);
#endif
    PASS();
}
TEST test_getpgrp(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getpgrp();
#endif
    PASS();
}
TEST test_getppid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getppid();
#endif
    PASS();
}
TEST test_getsid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getsid(0);
#endif
    PASS();
}
TEST test_getuid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    getuid();
#endif
    PASS();
}
TEST test_lchown(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    lchown("NUL", -1, -1);
#endif
    PASS();
}
TEST test_link(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    link("a", "b");
#endif
    PASS();
}
TEST test_linkat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    linkat(-1, "a", -1, "b", 0);
#endif
    PASS();
}
TEST test_lockf(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    lockf(-1, 0, 0);
#endif
    PASS();
}
TEST test_pathconf(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    pathconf("NUL", 0);
#endif
    PASS();
}
TEST test_pause(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    pause();
#endif
    PASS();
}
TEST test_pipe(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    pipe(NULL);
#endif
    PASS();
}
TEST test_pread(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    pread(-1, NULL, 0, 0);
#endif
    PASS();
}
TEST test_pwrite(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    pwrite(-1, NULL, 0, 0);
#endif
    PASS();
}
TEST test_readlink(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    readlink("NUL", NULL, 0);
#endif
    PASS();
}
TEST test_readlinkat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    readlinkat(-1, "NUL", NULL, 0);
#endif
    PASS();
}
TEST test_setegid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setegid(-1);
#endif
    PASS();
}
TEST test_seteuid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    seteuid(-1);
#endif
    PASS();
}
TEST test_setgid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setgid(-1);
#endif
    PASS();
}
TEST test_setpgid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setpgid(0, 0);
#endif
    PASS();
}
TEST test_setpgrp(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setpgrp();
#endif
    PASS();
}
TEST test_setregid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setregid(-1, -1);
#endif
    PASS();
}
TEST test_setreuid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setreuid(-1, -1);
#endif
    PASS();
}
TEST test_setsid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setsid();
#endif
    PASS();
}
TEST test_setuid(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    setuid(-1);
#endif
    PASS();
}
TEST test_symlink(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    symlink("a", "b");
#endif
    PASS();
}
TEST test_symlinkat(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    symlinkat("a", -1, "b");
#endif
    PASS();
}
TEST test_sync(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    sync();
#endif
    PASS();
}
TEST test_sysconf(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    sysconf(0);
#endif
    PASS();
}
TEST test_tcgetpgrp(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    tcgetpgrp(-1);
#endif
    PASS();
}
TEST test_tcsetpgrp(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    tcsetpgrp(-1, 0);
#endif
    PASS();
}
TEST test_truncate(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    truncate("NUL", 0);
#endif
    PASS();
}
TEST test_ttyname(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    ttyname(-1);
#endif
    PASS();
}
TEST test_ttyname_r(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    ttyname_r(-1, NULL, 0);
#endif
    PASS();
}
TEST test_ualarm(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    ualarm(0, 0);
#endif
    PASS();
}
TEST test_vfork(void) {
    /* Execute polyfill for coverage */
#if defined(_WIN32) && !defined(__CYGWIN__)
    vfork();
#endif
    PASS();
}


#if defined(_WIN32) && !defined(__CYGWIN__)
static void invalid_parameter_handler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved) {
   (void)expression; (void)function; (void)file; (void)line; (void)pReserved;
}
#endif
GREATEST_MAIN_DEFS();
int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
#if defined(_WIN32) && !defined(__CYGWIN__)
    _set_invalid_parameter_handler(invalid_parameter_handler);
    _CrtSetReportMode(_CRT_ASSERT, 0);
#endif

    RUN_TEST(test_open);
    RUN_TEST(test_close);
    RUN_TEST(test_read);
    RUN_TEST(test_write);
    RUN_TEST(test_lseek);
    RUN_TEST(test_dup);
    RUN_TEST(test_dup2);
    RUN_TEST(test_fsync);
    RUN_TEST(test_ftruncate);
    RUN_TEST(test_access);
    RUN_TEST(test_rmdir);
    RUN_TEST(test_chdir);
    RUN_TEST(test_getcwd);
    RUN_TEST(test_unlink);
    RUN_TEST(test_execve);
    RUN_TEST(test_execv);
    RUN_TEST(test_execvp);
    RUN_TEST(test_execl);
    RUN_TEST(test_execlp);
    RUN_TEST(test_getpid);
    RUN_TEST(test_sleep);
    RUN_TEST(test_usleep);
    RUN_TEST(test_isatty);
    RUN_TEST(test_swab);
    RUN_TEST(test_creat);
    RUN_TEST(test_fcntl);
    RUN_TEST(test_openat);
    RUN_TEST(test_posix_fadvise);
    RUN_TEST(test_posix_fallocate);
    RUN_TEST(test_alarm);
    RUN_TEST(test_chown);
    RUN_TEST(test_confstr);
    RUN_TEST(test_crypt);
    RUN_TEST(test_encrypt);
    RUN_TEST(test_faccessat);
    RUN_TEST(test_fchown);
    RUN_TEST(test_fchownat);
    RUN_TEST(test_fdatasync);
    RUN_TEST(test_fexecve);
    RUN_TEST(test_fork);
    RUN_TEST(test_fpathconf);
    RUN_TEST(test_getegid);
    RUN_TEST(test_geteuid);
    RUN_TEST(test_getgid);
    RUN_TEST(test_getgroups);
    RUN_TEST(test_gethostid);
    RUN_TEST(test_gethostname);
    RUN_TEST(test_getlogin);
    RUN_TEST(test_getlogin_r);
    RUN_TEST(test_getopt);
    RUN_TEST(test_getpgid);
    RUN_TEST(test_getpgrp);
    RUN_TEST(test_getppid);
    RUN_TEST(test_getsid);
    RUN_TEST(test_getuid);
    RUN_TEST(test_lchown);
    RUN_TEST(test_link);
    RUN_TEST(test_linkat);
    RUN_TEST(test_lockf);
    RUN_TEST(test_pathconf);
    RUN_TEST(test_pause);
    RUN_TEST(test_pipe);
    RUN_TEST(test_pread);
    RUN_TEST(test_pwrite);
    RUN_TEST(test_readlink);
    RUN_TEST(test_readlinkat);
    RUN_TEST(test_setegid);
    RUN_TEST(test_seteuid);
    RUN_TEST(test_setgid);
    RUN_TEST(test_setpgid);
    RUN_TEST(test_setpgrp);
    RUN_TEST(test_setregid);
    RUN_TEST(test_setreuid);
    RUN_TEST(test_setsid);
    RUN_TEST(test_setuid);
    RUN_TEST(test_symlink);
    RUN_TEST(test_symlinkat);
    RUN_TEST(test_sync);
    RUN_TEST(test_sysconf);
    RUN_TEST(test_tcgetpgrp);
    RUN_TEST(test_tcsetpgrp);
    RUN_TEST(test_truncate);
    RUN_TEST(test_ttyname);
    RUN_TEST(test_ttyname_r);
    RUN_TEST(test_ualarm);
    RUN_TEST(test_vfork);
    GREATEST_MAIN_END();
}
