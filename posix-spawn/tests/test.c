/* test.c - 100% Test Coverage Stubs */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "greatest.h"
#include "posix-spawn.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else
#include <unistd.h>
#endif

GREATEST_MAIN_DEFS();

TEST test_posix_spawnattr_lifecycle(void) {
    posix_spawnattr_t attr;
    short flags;
    pid_t pgroup;
    struct sched_param sched;
    int policy;
    sigset_t sig;

    ASSERT_EQ(0, posix_spawnattr_init(&attr));

    /* Flags */
    ASSERT_EQ(0, posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP));
    ASSERT_EQ(0, posix_spawnattr_getflags(&attr, &flags));
    ASSERT_EQ(POSIX_SPAWN_SETPGROUP, flags);

    /* PGroup */
    ASSERT_EQ(0, posix_spawnattr_setpgroup(&attr, 1234));
    ASSERT_EQ(0, posix_spawnattr_getpgroup(&attr, &pgroup));
    ASSERT_EQ(1234, pgroup);

    /* SchedParam */
    sched.sched_priority = 10;
    ASSERT_EQ(0, posix_spawnattr_setschedparam(&attr, &sched));
    sched.sched_priority = 0;
    ASSERT_EQ(0, posix_spawnattr_getschedparam(&attr, &sched));
    ASSERT_EQ(10, sched.sched_priority);

    /* SchedPolicy */
    ASSERT_EQ(0, posix_spawnattr_setschedpolicy(&attr, 1));
    ASSERT_EQ(0, posix_spawnattr_getschedpolicy(&attr, &policy));
    ASSERT_EQ(1, policy);

    /* SigMask */
    sig = 0x1234;
    ASSERT_EQ(0, posix_spawnattr_setsigmask(&attr, &sig));
    sig = 0;
    ASSERT_EQ(0, posix_spawnattr_getsigmask(&attr, &sig));
    ASSERT_EQ(0x1234, sig);

    /* SigDefault */
    sig = 0x5678;
    ASSERT_EQ(0, posix_spawnattr_setsigdefault(&attr, &sig));
    sig = 0;
    ASSERT_EQ(0, posix_spawnattr_getsigdefault(&attr, &sig));
    ASSERT_EQ(0x5678, sig);

    ASSERT_EQ(0, posix_spawnattr_destroy(&attr));
    PASS();
}

TEST test_posix_spawnattr_nulls(void) {
    ASSERT_EQ(EINVAL, posix_spawnattr_init(NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_destroy(NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_getflags(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_setflags(NULL, 0));
    ASSERT_EQ(EINVAL, posix_spawnattr_getpgroup(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_setpgroup(NULL, 0));
    ASSERT_EQ(EINVAL, posix_spawnattr_getschedparam(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_setschedparam(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_getschedpolicy(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_setschedpolicy(NULL, 0));
    ASSERT_EQ(EINVAL, posix_spawnattr_getsigmask(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_setsigmask(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_getsigdefault(NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnattr_setsigdefault(NULL, NULL));
    PASS();
}

TEST test_posix_spawn_file_actions_lifecycle(void) {
    posix_spawn_file_actions_t actions;
    
    ASSERT_EQ(0, posix_spawn_file_actions_init(&actions));
    
    ASSERT_EQ(0, posix_spawn_file_actions_addclose(&actions, 0));
    ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&actions, 1, 2));
    ASSERT_EQ(0, posix_spawn_file_actions_addopen(&actions, 3, "test.txt", 0, 0));

    ASSERT_EQ(0, posix_spawn_file_actions_destroy(&actions));
    PASS();
}

TEST test_posix_spawn_file_actions_nulls(void) {
    ASSERT_EQ(EINVAL, posix_spawn_file_actions_init(NULL));
    ASSERT_EQ(EINVAL, posix_spawn_file_actions_destroy(NULL));
    ASSERT_EQ(EINVAL, posix_spawn_file_actions_addclose(NULL, 0));
    ASSERT_EQ(EINVAL, posix_spawn_file_actions_adddup2(NULL, 0, 0));
    ASSERT_EQ(EINVAL, posix_spawn_file_actions_addopen(NULL, 0, NULL, 0, 0));
    PASS();
}

TEST test_posix_spawn_file_actions_badf(void) {
    posix_spawn_file_actions_t actions;
    ASSERT_EQ(0, posix_spawn_file_actions_init(&actions));
    ASSERT_EQ(EBADF, posix_spawn_file_actions_addclose(&actions, -1));
    ASSERT_EQ(EBADF, posix_spawn_file_actions_adddup2(&actions, -1, 0));
    ASSERT_EQ(EBADF, posix_spawn_file_actions_adddup2(&actions, 0, -1));
    ASSERT_EQ(EBADF, posix_spawn_file_actions_addopen(&actions, -1, "test.txt", 0, 0));
    ASSERT_EQ(EINVAL, posix_spawn_file_actions_addopen(&actions, 0, NULL, 0, 0));
    ASSERT_EQ(0, posix_spawn_file_actions_destroy(&actions));
    PASS();
}

TEST test_posix_spawn_execution(void) {
    pid_t pid;
    char *argv[] = {"cmd.exe", "/c", "exit 0", NULL};
    char *envp[] = {"TEST=1", NULL};
    posix_spawnattr_t attr;
    posix_spawn_file_actions_t actions;

    posix_spawnattr_init(&attr);
    posix_spawn_file_actions_init(&actions);

    /* This will return ENOSYS on non-windows, or 0 / error on Windows */
#if defined(_MSC_VER) || defined(__MINGW32__)
    ASSERT_EQ(0, posix_spawnp(&pid, "cmd.exe", &actions, &attr, argv, envp));
    if (pid > 0) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, pid);
        if (hProcess) {
            WaitForSingleObject(hProcess, INFINITE);
            CloseHandle(hProcess);
        }
    }
    ASSERT_EQ(0, posix_spawn(&pid, "C:\\Windows\\System32\\cmd.exe", &actions, &attr, argv, envp));
    if (pid > 0) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, pid);
        if (hProcess) {
            WaitForSingleObject(hProcess, INFINITE);
            CloseHandle(hProcess);
        }
    }
#else
    ASSERT_EQ(ENOSYS, posix_spawnp(&pid, "ls", &actions, &attr, argv, envp));
    ASSERT_EQ(ENOSYS, posix_spawn(&pid, "/bin/ls", &actions, &attr, argv, envp));
#endif

    posix_spawnattr_destroy(&attr);
    posix_spawn_file_actions_destroy(&actions);
    PASS();
}

TEST test_posix_spawn_nulls(void) {
#if defined(_MSC_VER) || defined(__MINGW32__)
    ASSERT_EQ(EINVAL, posix_spawn(NULL, NULL, NULL, NULL, NULL, NULL));
    ASSERT_EQ(EINVAL, posix_spawnp(NULL, NULL, NULL, NULL, NULL, NULL));
#else
    ASSERT_EQ(ENOSYS, posix_spawn(NULL, NULL, NULL, NULL, NULL, NULL));
    ASSERT_EQ(ENOSYS, posix_spawnp(NULL, NULL, NULL, NULL, NULL, NULL));
#endif
    PASS();
}

SUITE(posix_spawn_suite) {
    RUN_TEST(test_posix_spawnattr_lifecycle);
    RUN_TEST(test_posix_spawnattr_nulls);
    RUN_TEST(test_posix_spawn_file_actions_lifecycle);
    RUN_TEST(test_posix_spawn_file_actions_nulls);
    RUN_TEST(test_posix_spawn_file_actions_badf);
    RUN_TEST(test_posix_spawn_execution);
    RUN_TEST(test_posix_spawn_nulls);
}

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(posix_spawn_suite);
    GREATEST_MAIN_END();
}
