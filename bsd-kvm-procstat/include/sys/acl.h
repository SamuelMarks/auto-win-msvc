#ifndef SYS_ACL_H
#define SYS_ACL_H

/**
 * @file acl.h
 * @brief FreeBSD POSIX.1e / NFSv4 Access Control Lists (<sys/acl.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef int acl_type_t;
typedef int acl_tag_t;
typedef void *acl_t;
typedef void *acl_entry_t;
typedef void *acl_permset_t;
typedef int acl_perm_t;

#define ACL_TYPE_ACCESS 0x00000002
#define ACL_TYPE_DEFAULT 0x00000001
#define ACL_TYPE_NFS4 0x00000004

#define ACL_FIRST_ENTRY 0
#define ACL_NEXT_ENTRY 1

#define ACL_USER_OBJ 0x00000001
#define ACL_USER 0x00000002
#define ACL_GROUP_OBJ 0x00000004
#define ACL_GROUP 0x00000008
#define ACL_MASK 0x00000010
#define ACL_OTHER 0x00000020

acl_t acl_get_file(const char *path_p, acl_type_t type);
acl_t acl_get_link_np(const char *path_p, acl_type_t type);
acl_t acl_get_fd(int fd);
acl_t acl_get_fd_np(int fd, acl_type_t type);
int acl_get_entry(acl_t acl, int entry_id, acl_entry_t *entry_p);
int acl_get_tag_type(acl_entry_t entry_d, acl_tag_t *tag_type_p);
int acl_free(void *obj_p);
int acl_is_trivial_np(const acl_t acl, int *trivialp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_ACL_H */
