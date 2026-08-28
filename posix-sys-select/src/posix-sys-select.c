/* posix-sys-select.c */
/* clang-format off */
#include "posix-sys-select.h"

#ifdef _MSC_VER
#undef FD_ZERO
#define FD_ZERO(set) (((fd_set *)(set))->fd_count = 0)
#undef FD_SET
#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set *)(set))->fd_count) { \
        if (((fd_set *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set *)(set))->fd_array[__i] = (fd); \
            ((fd_set *)(set))->fd_count++; \
        } \
    } \
} while((void)0, 0)
#undef FD_CLR
#define FD_CLR(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            while (__i < ((fd_set *)(set))->fd_count - 1) { \
                ((fd_set *)(set))->fd_array[__i] = \
                    ((fd_set *)(set))->fd_array[__i + 1]; \
                __i++; \
            } \
            ((fd_set *)(set))->fd_count--; \
            break; \
        } \
    } \
} while((void)0, 0)
#endif

/* clang-format on */

typedef int make_iso_compilers_happy_tu_posix_sys_select;
