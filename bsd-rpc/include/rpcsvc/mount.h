#ifndef RPCSVC_MOUNT_H
#define RPCSVC_MOUNT_H

/**
 * @file mount.h
 * @brief NFS mount protocol definitions (<rpcsvc/mount.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define MOUNTPROG 100005UL
#define MOUNTVERS 1UL
#define MOUNTVERS3 3UL
#define MOUNTPROC_NULL 0UL
#define MOUNTPROC_MNT 1UL
#define MOUNTPROC_DUMP 2UL
#define MOUNTPROC_UMNT 3UL
#define MOUNTPROC_UMNTALL 4UL
#define MOUNTPROC_EXPORT 5UL

#define MNTPATHLEN 1024
#define MNTNAMLEN 255

typedef char *dirpath;
typedef char *name;

struct mountbody {
  name ml_hostname;
  dirpath ml_directory;
  struct mountbody *ml_next;
};
typedef struct mountbody *mountlist;

struct groupnode {
  name gr_name;
  struct groupnode *gr_next;
};
typedef struct groupnode *groups;

struct exportnode {
  dirpath ex_dir;
  groups ex_groups;
  struct exportnode *ex_next;
};
typedef struct exportnode *exports;

bool_t xdr_dirpath(XDR *xdrs, dirpath *objp);
bool_t xdr_mountlist(XDR *xdrs, mountlist *objp);
bool_t xdr_exports(XDR *xdrs, exports *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_MOUNT_H */
