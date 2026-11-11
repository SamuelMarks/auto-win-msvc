#ifndef SYS_EXTATTR_H
#define SYS_EXTATTR_H

/**
 * @file extattr.h
 * @brief FreeBSD extended attributes (<sys/extattr.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define EXTATTR_NAMESPACE_EMPTY 0x00000000
#define EXTATTR_NAMESPACE_USER 0x00000001
#define EXTATTR_NAMESPACE_SYSTEM 0x00000002

ssize_t extattr_get_fd(int fd, int attrnamespace, const char *attrname,
                       void *data, size_t nbytes);
ssize_t extattr_get_file(const char *path, int attrnamespace,
                         const char *attrname, void *data, size_t nbytes);
ssize_t extattr_get_link(const char *path, int attrnamespace,
                         const char *attrname, void *data, size_t nbytes);
ssize_t extattr_list_fd(int fd, int attrnamespace, void *data, size_t nbytes);
ssize_t extattr_list_file(const char *path, int attrnamespace, void *data,
                          size_t nbytes);
ssize_t extattr_list_link(const char *path, int attrnamespace, void *data,
                          size_t nbytes);
int extattr_set_fd(int fd, int attrnamespace, const char *attrname,
                   const void *data, size_t nbytes);
int extattr_set_file(const char *path, int attrnamespace, const char *attrname,
                     const void *data, size_t nbytes);
int extattr_set_link(const char *path, int attrnamespace, const char *attrname,
                     const void *data, size_t nbytes);
int extattr_delete_fd(int fd, int attrnamespace, const char *attrname);
int extattr_delete_file(const char *path, int attrnamespace,
                        const char *attrname);
int extattr_delete_link(const char *path, int attrnamespace,
                        const char *attrname);
int extattr_namespace_to_string(int attrnamespace, char **string);
int extattr_string_to_namespace(const char *string, int *attrnamespace);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_EXTATTR_H */
