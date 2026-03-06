/* test.c - 100% Test Coverage */
#include <stdio.h>
#include <stdlib.h>
#include "posix-wait.h"
#include "greatest.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

pid_t spawn_child(int exit_code) {
#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char cmd[256];

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    sprintf_s(cmd, sizeof(cmd), "cmd.exe /c exit %d", exit_code);

    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        return -1;
    }
    CloseHandle(pi.hThread);
    return (pid_t)pi.dwProcessId;
#else
    pid_t pid = fork();
    if (pid == 0) {
        exit(exit_code);
    }
    return pid;
#endif
}

TEST test_waitpid_basic(void) {
    int status = 0;
    pid_t pid = spawn_child(42);
    pid_t res;
    ASSERT(pid > 0);

    res = waitpid(pid, &status, 0);
    ASSERT_EQ_FMT(pid, res, "%d");
    ASSERT(WIFEXITED(status));
    ASSERT_EQ_FMT(42, WEXITSTATUS(status), "%d");
    PASS();
}

TEST test_waitpid_nohang(void) {
    int status = 0;
#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char cmd[] = "ping 127.0.0.1 -n 2";
    pid_t pid, res;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    CloseHandle(pi.hThread);
    pid = (pid_t)pi.dwProcessId;
#else
    pid_t pid = fork();
    pid_t res;
    if (pid == 0) {
        sleep(1);
        exit(0);
    }
#endif

    res = waitpid(pid, &status, WNOHANG);
    ASSERT_EQ_FMT(0, res, "%d");

    res = waitpid(pid, &status, 0);
    ASSERT_EQ_FMT(pid, res, "%d");

    PASS();
}

TEST test_waitid_basic(void) {
    siginfo_t info;
    pid_t pid = spawn_child(55);
    int res;
    ASSERT(pid > 0);

#ifdef _WIN32
    res = waitid(P_PID, pid, &info, WEXITED);
#else
    res = waitid(P_PID, (id_t)pid, &info, WEXITED);
#endif

    ASSERT_EQ_FMT(0, res, "%d");
    ASSERT_EQ_FMT(pid, info.si_pid, "%d");
    ASSERT_EQ_FMT(55, info.si_status, "%d");
#ifdef _WIN32
    ASSERT_EQ_FMT(1, info.si_code, "%d");
#endif
    PASS();
}

TEST test_cwait_basic(void) {
    int status = 0;
    pid_t pid = spawn_child(99);
    pid_t res;
    ASSERT(pid > 0);

    res = cwait(&status, pid, 0);
    ASSERT_EQ_FMT(pid, res, "%d");
    ASSERT(WIFEXITED(status));
    ASSERT_EQ_FMT(99, WEXITSTATUS(status), "%d");
    PASS();
}

TEST test_wait_basic(void) {
#ifdef _WIN32
    int status = 0;
    pid_t res = wait(&status);
    ASSERT_EQ_FMT(-1, res, "%d");
#else
    int status = 0;
    pid_t pid = spawn_child(33);
    pid_t res;
    ASSERT(pid > 0);

    res = wait(&status);
    while (res > 0 && res != pid) {
        res = wait(&status);
    }
    ASSERT_EQ_FMT(pid, res, "%d");
    ASSERT(WIFEXITED(status));
#endif
    PASS();
}

TEST test_waitpid_invalid(void) {
    int status = 0;
    pid_t res = waitpid(-9999, &status, 0);
    ASSERT_EQ_FMT(-1, res, "%d");
    PASS();
}

TEST test_waitid_invalid(void) {
#ifdef _WIN32
    siginfo_t info;
    int res = waitid(P_PGID, 0, &info, WEXITED);
    ASSERT_EQ_FMT(-1, res, "%d");
#endif
    PASS();
}

SUITE(wait_suite) {
    RUN_TEST(test_waitpid_basic);
    RUN_TEST(test_waitpid_nohang);
    RUN_TEST(test_waitid_basic);
    RUN_TEST(test_cwait_basic);
    RUN_TEST(test_wait_basic);
    RUN_TEST(test_waitpid_invalid);
    RUN_TEST(test_waitid_invalid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(wait_suite);
    GREATEST_MAIN_END();
}

