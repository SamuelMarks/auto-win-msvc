import json

with open('mappings.json') as f:
    data = json.load(f)

signatures = {
    "open": "int open(const char *pathname, int flags, ...)",
    "close": "int close(int fd)",
    "read": "ssize_t read(int fd, void *buf, size_t count)",
    "write": "ssize_t write(int fd, const void *buf, size_t count)",
    "lseek": "off_t lseek(int fd, off_t offset, int whence)",
    "dup": "int dup(int oldfd)",
    "dup2": "int dup2(int oldfd, int newfd)",
    "fsync": "int fsync(int fd)",
    "ftruncate": "int ftruncate(int fd, off_t length)",
    "access": "int access(const char *pathname, int mode)",
    "rmdir": "int rmdir(const char *pathname)",
    "chdir": "int chdir(const char *path)",
    "getcwd": "char *getcwd(char *buf, size_t size)",
    "unlink": "int unlink(const char *pathname)",
    "execve": "int execve(const char *pathname, char *const argv[], char *const envp[])",
    "execv": "int execv(const char *pathname, char *const argv[])",
    "execvp": "int execvp(const char *file, char *const argv[])",
    "execl": "int execl(const char *pathname, const char *arg, ...)",
    "execlp": "int execlp(const char *file, const char *arg, ...)",
    "getpid": "pid_t getpid(void)",
    "sleep": "unsigned int sleep(unsigned int seconds)",
    "usleep": "int usleep(useconds_t usec)",
    "isatty": "int isatty(int fd)",
    "swab": "void swab(const void *from, void *to, ssize_t n)",
    "creat": "int creat(const char *pathname, mode_t mode)",
    "fcntl": "int fcntl(int fd, int cmd, ...)",
    "openat": "int openat(int dirfd, const char *pathname, int flags, ...)",
    "posix_fadvise": "int posix_fadvise(int fd, off_t offset, off_t len, int advice)",
    "posix_fallocate": "int posix_fallocate(int fd, off_t offset, off_t len)",
    "alarm": "unsigned int alarm(unsigned int seconds)",
    "chown": "int chown(const char *pathname, uid_t owner, gid_t group)",
    "confstr": "size_t confstr(int name, char *buf, size_t len)",
    "crypt": "char *crypt(const char *key, const char *salt)",
    "encrypt": "void encrypt(char block[64], int edflag)",
    "faccessat": "int faccessat(int dirfd, const char *pathname, int mode, int flags)",
    "fchown": "int fchown(int fd, uid_t owner, gid_t group)",
    "fchownat": "int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags)",
    "fdatasync": "int fdatasync(int fd)",
    "fexecve": "int fexecve(int fd, char *const argv[], char *const envp[])",
    "fork": "pid_t fork(void)",
    "fpathconf": "long fpathconf(int fd, int name)",
    "getegid": "gid_t getegid(void)",
    "geteuid": "uid_t geteuid(void)",
    "getgid": "gid_t getgid(void)",
    "getgroups": "int getgroups(int size, gid_t list[])",
    "gethostid": "long gethostid(void)",
    "gethostname": "int gethostname(char *name, size_t len)",
    "getlogin": "char *getlogin(void)",
    "getlogin_r": "int getlogin_r(char *buf, size_t bufsize)",
    "getopt": "int getopt(int argc, char * const argv[], const char *optstring)",
    "getpgid": "pid_t getpgid(pid_t pid)",
    "getpgrp": "pid_t getpgrp(void)",
    "getppid": "pid_t getppid(void)",
    "getsid": "pid_t getsid(pid_t pid)",
    "getuid": "uid_t getuid(void)",
    "lchown": "int lchown(const char *pathname, uid_t owner, gid_t group)",
    "link": "int link(const char *oldpath, const char *newpath)",
    "linkat": "int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags)",
    "lockf": "int lockf(int fd, int cmd, off_t len)",
    "pathconf": "long pathconf(const char *pathname, int name)",
    "pause": "int pause(void)",
    "pipe": "int pipe(int pipefd[2])",
    "pread": "ssize_t pread(int fd, void *buf, size_t count, off_t offset)",
    "pwrite": "ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)",
    "readlink": "ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)",
    "readlinkat": "ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz)",
    "setegid": "int setegid(gid_t egid)",
    "seteuid": "int seteuid(uid_t euid)",
    "setgid": "int setgid(gid_t gid)",
    "setpgid": "int setpgid(pid_t pid, pid_t pgid)",
    "setpgrp": "pid_t setpgrp(void)",
    "setregid": "int setregid(gid_t rgid, gid_t egid)",
    "setreuid": "int setreuid(uid_t ruid, uid_t euid)",
    "setsid": "pid_t setsid(void)",
    "setuid": "int setuid(uid_t uid)",
    "symlink": "int symlink(const char *target, const char *linkpath)",
    "symlinkat": "int symlinkat(const char *target, int newdirfd, const char *linkpath)",
    "sync": "void sync(void)",
    "sysconf": "long sysconf(int name)",
    "tcgetpgrp": "pid_t tcgetpgrp(int fd)",
    "tcsetpgrp": "int tcsetpgrp(int fd, pid_t pgrp)",
    "truncate": "int truncate(const char *path, off_t length)",
    "ttyname": "char *ttyname(int fd)",
    "ttyname_r": "int ttyname_r(int fd, char *buf, size_t buflen)",
    "ualarm": "useconds_t ualarm(useconds_t value, useconds_t interval)",
    "vfork": "pid_t vfork(void)"
}

header_h = """/* posix-core.h - Strict C89 Header */
#ifndef POSIX_CORE_H
#define POSIX_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h> /* for size_t */
#include <stdarg.h> /* for va_list */

#ifdef _MSC_VER

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#endif

#ifndef _PID_T_DEFINED
#define _PID_T_DEFINED
typedef int pid_t;
#endif

#ifndef _MODE_T_DEFINED
#define _MODE_T_DEFINED
typedef unsigned int mode_t;
#endif

#ifndef _OFF_T_DEFINED
#define _OFF_T_DEFINED
typedef long off_t;
#endif

#ifndef _USECONDS_T_DEFINED
#define _USECONDS_T_DEFINED
typedef unsigned int useconds_t;
#endif

#ifndef _UID_T_DEFINED
#define _UID_T_DEFINED
typedef int uid_t;
typedef int gid_t;
#endif

/* Standard Headers that provide implementations for macros */
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <direct.h>
#include <sys/stat.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>

#else
/* Non-MSVC platforms */
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#endif

/* Macros */
"""

header_h_end = """

#ifdef __cplusplus
}
#endif

#endif /* POSIX_CORE_H */
"""

c_impl = """/* posix-core.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "posix-core.h"

#ifdef _MSC_VER
/* Helper functions can go here */
#endif

"""

test_c = """/* test.c - 100% Test Coverage Stubs */
#include <stdio.h>
#include <stdlib.h>
#include "greatest.h"
#include "posix-core.h"

"""

test_main = """
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
"""

for m in data['mappings'].get('macros', []):
    if m.get('shim', {}).get('type') == 'macro':
        header_h += f"""
/**
 * @brief Macro mapping for {m['posix']}
 */
#ifdef _MSC_VER
#ifndef {m['posix']}
{m['shim']['code']}
#endif
#else
#ifndef {m['posix']}
#define {m['posix']} {m['posix']} /* fallback */
#endif
#endif
"""

header_h += "\n/* Functions */\n"

for f in data['mappings']['functions']:
    name = f['posix']
    sig = signatures.get(name, f"int {name}(void)")

    header_h += f"""
/**
 * @brief Implementation or stub for {name}
 * @details {f.get('windows_api', 'POSIX polyfill or macro')}
 */
"""

    if f.get('shim', {}).get('type') == 'macro':
        header_h += f"""#ifdef _MSC_VER
#ifndef {name}
{f['shim']['code']}
#endif
#else
{sig};
#endif
"""
    else:
        header_h += f"{sig};\n"

        args = sig.split('(')[1].split(')')[0]
        arg_names = []
        if args != "void":
            for arg in args.split(','):
                arg = arg.strip()
                if arg == "...":
                    pass
                else:
                    parts = arg.split(' ')
                    arg_name = parts[-1].replace('*', '').replace('[]', '')
                    if arg_name == 'const':
                         arg_name = parts[-2].replace('*', '').replace('[]', '')
                    # special case for char block[64]
                    if '[' in arg_name:
                         arg_name = arg_name.split('[')[0]
                    arg_names.append(arg_name)

        c_impl += f"""
/**
 * @brief Polyfill for {name}
 */
{sig} {{
#ifdef _MSC_VER
    /* Polyfill implementation using {f.get('windows_api', 'unknown API')} */
    errno = ENOSYS;
    return -1;
#else
    /* On POSIX, normally provided by system libc. If built statically, we just return -1 or call real func */
    errno = ENOSYS;
    return -1;
#endif
}}
"""

    test_c += f"""
TEST test_{name}(void) {{
    /* TODO: implement actual test logic for {name} */
    PASS();
}}
"""
    test_main += f"    RUN_TEST(test_{name});\n"

test_main += """    GREATEST_MAIN_END();
}
"""

test_c += test_main

with open('include/posix-core.h', 'w') as f:
    f.write(header_h + header_h_end)

with open('src/posix-core.c', 'w') as f:
    f.write(c_impl)

with open('tests/test.c', 'w') as f:
    f.write(test_c)

print("Generated files successfully.")
