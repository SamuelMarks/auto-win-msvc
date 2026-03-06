/* posix-wait.c - Strict C89 Implementation */
#include <stddef.h>
#include "posix-wait.h"
#include <errno.h>

#ifdef _WIN32
#include <windows.h>

pid_t wait(int *stat_loc) {
    return waitpid(-1, stat_loc, 0);
}

pid_t waitpid(pid_t pid, int *stat_loc, int options) {
    HANDLE hProcess;
    DWORD wait_res;
    DWORD exit_code;

    if (pid <= 0) {
        /* On Windows, waiting for any child (-1) or process group is not directly supported
         * without maintaining an internal list of all child processes. 
         * Return ECHILD to signify no such child process. */
        errno = ECHILD;
        return (pid_t)-1;
    }

    hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
    if (hProcess == NULL) {
        errno = ECHILD;
        return (pid_t)-1;
    }

    wait_res = WaitForSingleObject(hProcess, (options & WNOHANG) ? 0 : INFINITE);
    if (wait_res == WAIT_TIMEOUT) {
        CloseHandle(hProcess);
        return 0;
    } else if (wait_res == WAIT_OBJECT_0) {
        if (stat_loc != NULL) {
            if (GetExitCodeProcess(hProcess, &exit_code)) {
                /* Simulate POSIX status byte shifting */
                *stat_loc = ((int)(exit_code & 0xFF) << 8);
            } else {
                *stat_loc = 0;
            }
        }
        CloseHandle(hProcess);
        return pid;
    }

    CloseHandle(hProcess);
    errno = EINVAL;
    return (pid_t)-1;
}

int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
    pid_t res;
    int status = 0;
    int pid_to_wait = -1;

    if (infop != NULL) {
        infop->si_signo = 0;
        infop->si_code = 0;
        infop->si_pid = 0;
        infop->si_status = 0;
        infop->si_uid = 0;
    }

    if (idtype == P_PID) {
        pid_to_wait = (int)id;
    } else if (idtype == P_ALL) {
        pid_to_wait = -1;
    } else {
        errno = ENOSYS;
        return -1;
    }

    res = waitpid((pid_t)pid_to_wait, &status, options);
    if (res > 0) {
        if (infop != NULL) {
            infop->si_pid = res;
            infop->si_status = WEXITSTATUS(status);
            infop->si_code = 1; /* CLD_EXITED */
        }
        return 0;
    } else if (res == 0) {
        return 0;
    }
    
    return -1;
}

#endif /* _WIN32 */

/* Non-standard Microsoft-compatible cwait */
pid_t cwait(int *termstat, pid_t pid, int action) {
    (void)action; /* Unused */
    return waitpid(pid, termstat, 0);
}

