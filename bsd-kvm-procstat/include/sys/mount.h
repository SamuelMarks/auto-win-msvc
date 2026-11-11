#ifndef SYS_MOUNT_H
#define SYS_MOUNT_H

/**
 * @file mount.h
 * @brief FreeBSD filesystem mount and statfs (<sys/mount.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/statfs.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MNAMELEN
#define MNAMELEN 1024
#endif
#ifndef MFSNAMELEN
#define MFSNAMELEN 16
#endif

#define MNT_RDONLY (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000001UL)
#define MNT_SYNCHRONOUS                                                        \
  (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000002UL)
#define MNT_NOEXEC (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000004UL)
#define MNT_NOSUID (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000008UL)
#define MNT_NFS4ACLS (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000010UL)
#define MNT_UNION (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000020UL)
#define MNT_ASYNC (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000040UL)
#define MNT_SUIDDIR (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00100000UL)
#define MNT_SOFTDEP (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00200000UL)
#define MNT_NOSYMFOLLOW                                                        \
  (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00400000UL)
#define MNT_GJOURNAL (((uint64_t)0x00000000UL << 32) | (uint64_t)0x02000000UL)
#define MNT_MULTILABEL (((uint64_t)0x00000000UL << 32) | (uint64_t)0x04000000UL)
#define MNT_ACLS (((uint64_t)0x00000000UL << 32) | (uint64_t)0x08000000UL)
#define MNT_NOATIME (((uint64_t)0x00000000UL << 32) | (uint64_t)0x10000000UL)
#define MNT_CLUSTERW (((uint64_t)0x00000000UL << 32) | (uint64_t)0x40000000UL)
#define MNT_NOCLUSTERR (((uint64_t)0x00000000UL << 32) | (uint64_t)0x80000000UL)
#define MNT_EXRDONLY (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000080UL)
#define MNT_EXPORTED (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000100UL)
#define MNT_DEFEXPORTED                                                        \
  (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000200UL)
#define MNT_EXPORTANON (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000400UL)
#define MNT_EXKERB (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000800UL)
#define MNT_LOCAL (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00001000UL)
#define MNT_QUOTA (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00002000UL)
#define MNT_ROOTFS (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00004000UL)
#define MNT_USER (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00008000UL)
#define MNT_IGNORE (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00800000UL)
#define MNT_UPDATE (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00010000UL)
#define MNT_DELEXPORT (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00020000UL)
#define MNT_RELOAD (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00040000UL)
#define MNT_FORCE (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00080000UL)
#define MNT_LAZY (((uint64_t)0x00000000UL << 32) | (uint64_t)0x01000000UL)
#define MNT_SNAPSHOT (((uint64_t)0x00000001UL << 32) | (uint64_t)0x00000000UL)
#define MNT_BYFSID (((uint64_t)0x00000000UL << 32) | (uint64_t)0x08000000UL)

#define MNT_WAIT 1
#define MNT_NOWAIT 2

#define VFCF_STATIC 0x00010000
#define VFCF_NETWORK 0x00020000
#define VFCF_READONLY 0x00040000
#define VFCF_SYNTHETIC 0x00080000
#define VFCF_LOOPBACK 0x00100000
#define VFCF_UNICODE 0x00200000
#define VFCF_JAIL 0x00400000
#define VFCF_DELEGADMIN 0x00800000

/** @brief Virtual filesystem configuration entry for lsvfs. */
struct xvfsconf {
  void *vfc_vfsops;
  char vfc_name[MFSNAMELEN];
  int vfc_typenum;
  int vfc_refcount;
  int vfc_flags;
  void *vfc_next;
};

/** @brief File handle identifier. */
typedef struct fhandle {
  char fh_fid[32];
} fhandle_t;

int getmntinfo(struct statfs **mntbufp, int flags);
int getfsstat(struct statfs *buf, long bufsize, int mode);
int fhstatfs(const fhandle_t *fhp, struct statfs *buf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_MOUNT_H */
