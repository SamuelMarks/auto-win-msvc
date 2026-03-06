#include <sys/types.h>
#include <sys/wait.h>

#ifndef POSIX_WAIT_H
#define POSIX_WAIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Our custom waitpid declaration */
pid_t waitpid(pid_t pid, int *stat_loc, int options);

#ifdef __cplusplus
}
#endif

#endif
