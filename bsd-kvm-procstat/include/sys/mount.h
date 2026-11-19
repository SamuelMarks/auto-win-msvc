#ifndef SYS_MOUNT_H
#define SYS_MOUNT_H

/**
 * @file mount.h
 * @brief FreeBSD filesystem mount and statfs (<sys/mount.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#if defined(__APPLE__) && (defined(__GNUC__) || defined(__clang__))
#pragma GCC system_header
#include_next <sys/mount.h>
#elif defined(__linux__)
#include <string.h>
#include <sys/statfs.h>
#else
#include <sys/statfs.h>
#endif
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__) && !defined(AWM_LINUX_BSD_STATFS_DEFINED)
#define AWM_LINUX_BSD_STATFS_DEFINED 1
/** @brief BSD-compatible statfs structure with mountpoint and fstype names on
 * Linux. */
struct awm_bsd_statfs {
  long f_type;
  long f_bsize;
  long f_iosize;
  unsigned long f_blocks;
  unsigned long f_bfree;
  long f_bavail;
  unsigned long f_files;
  unsigned long f_ffree;
  uid_t f_owner;
  fsid_t f_fsid;
  long f_namelen;
  long f_frsize;
  unsigned long f_flags;
  char f_fstypename[16];
  char f_mntfromname[1024];
  char f_mntonname[1024];
};
/**
 * @brief Query filesystem status into a BSD-compatible statfs buffer on Linux.
 * @param[in] path Filesystem path, or NULL to query current directory.
 * @param[out] buf Output awm_bsd_statfs buffer.
 * @return 0 on success, or -1 on error.
 */
static __inline int awm_bsd_statfs(const char *path,
                                   struct awm_bsd_statfs *buf) {
  struct statfs lsb;
  if (buf == NULL) {
    return -1;
  }
  memset(buf, 0, sizeof(*buf));
  if (statfs(path != NULL ? path : ".", &lsb) != 0) {
    return -1;
  }
  buf->f_type = (long)lsb.f_type;
  buf->f_bsize = (long)lsb.f_bsize;
  buf->f_iosize = (long)lsb.f_bsize;
  buf->f_blocks = (unsigned long)lsb.f_blocks;
  buf->f_bfree = (unsigned long)lsb.f_bfree;
  buf->f_bavail = (long)lsb.f_bavail;
  buf->f_files = (unsigned long)lsb.f_files;
  buf->f_ffree = (unsigned long)lsb.f_ffree;
  buf->f_fsid = lsb.f_fsid;
  buf->f_namelen = (long)lsb.f_namelen;
  buf->f_frsize = (long)lsb.f_frsize;
  memcpy(buf->f_fstypename, "ext4", 5);
  memcpy(buf->f_mntfromname, "/dev/root", 10);
  memcpy(buf->f_mntonname, "/", 2);
  return 0;
}
/**
 * @brief Query open descriptor filesystem status into a BSD-compatible buffer.
 * @param[in] fd Open file descriptor.
 * @param[out] buf Output awm_bsd_statfs buffer.
 * @return 0 on success, or -1 on error.
 */
static __inline int awm_bsd_fstatfs(int fd, struct awm_bsd_statfs *buf) {
  struct statfs lsb;
  if (buf == NULL) {
    return -1;
  }
  memset(buf, 0, sizeof(*buf));
  if (fstatfs(fd, &lsb) != 0) {
    return -1;
  }
  buf->f_type = (long)lsb.f_type;
  buf->f_bsize = (long)lsb.f_bsize;
  buf->f_iosize = (long)lsb.f_bsize;
  buf->f_blocks = (unsigned long)lsb.f_blocks;
  buf->f_bfree = (unsigned long)lsb.f_bfree;
  buf->f_bavail = (long)lsb.f_bavail;
  buf->f_files = (unsigned long)lsb.f_files;
  buf->f_ffree = (unsigned long)lsb.f_ffree;
  buf->f_fsid = lsb.f_fsid;
  buf->f_namelen = (long)lsb.f_namelen;
  buf->f_frsize = (long)lsb.f_frsize;
  memcpy(buf->f_fstypename, "ext4", 5);
  memcpy(buf->f_mntfromname, "/dev/root", 10);
  memcpy(buf->f_mntonname, "/", 2);
  return 0;
}
#undef statfs
#undef fstatfs
#define statfs awm_bsd_statfs
#define fstatfs awm_bsd_fstatfs
#endif

/** @cond INTERNAL */
#ifndef MNAMELEN
#define MNAMELEN 1024
#endif
#ifndef MFSNAMELEN
#define MFSNAMELEN 16
#endif

#ifndef MNT_RDONLY
#define MNT_RDONLY (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000001UL)
#endif
#ifndef MNT_SYNCHRONOUS
#define MNT_SYNCHRONOUS                                                        \
  (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000002UL)
#endif
#ifndef MNT_NOEXEC
#define MNT_NOEXEC (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000004UL)
#endif
#ifndef MNT_NOSUID
#define MNT_NOSUID (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000008UL)
#endif
#ifndef MNT_NFS4ACLS
#define MNT_NFS4ACLS (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000010UL)
#endif
#ifndef MNT_UNION
#define MNT_UNION (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000020UL)
#endif
#ifndef MNT_ASYNC
#define MNT_ASYNC (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000040UL)
#endif
#ifndef MNT_SUIDDIR
#define MNT_SUIDDIR (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00100000UL)
#endif
#ifndef MNT_SOFTDEP
#define MNT_SOFTDEP (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00200000UL)
#endif
#ifndef MNT_NOSYMFOLLOW
#define MNT_NOSYMFOLLOW                                                        \
  (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00400000UL)
#endif
#ifndef MNT_GJOURNAL
#define MNT_GJOURNAL (((uint64_t)0x00000000UL << 32) | (uint64_t)0x02000000UL)
#endif
#ifndef MNT_MULTILABEL
#define MNT_MULTILABEL (((uint64_t)0x00000000UL << 32) | (uint64_t)0x04000000UL)
#endif
#ifndef MNT_ACLS
#define MNT_ACLS (((uint64_t)0x00000000UL << 32) | (uint64_t)0x08000000UL)
#endif
#ifndef MNT_NOATIME
#define MNT_NOATIME (((uint64_t)0x00000000UL << 32) | (uint64_t)0x10000000UL)
#endif
#ifndef MNT_CLUSTERW
#define MNT_CLUSTERW (((uint64_t)0x00000000UL << 32) | (uint64_t)0x40000000UL)
#endif
#ifndef MNT_NOCLUSTERR
#define MNT_NOCLUSTERR (((uint64_t)0x00000000UL << 32) | (uint64_t)0x80000000UL)
#endif
#ifndef MNT_EXRDONLY
#define MNT_EXRDONLY (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000080UL)
#endif
#ifndef MNT_EXPORTED
#define MNT_EXPORTED (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000100UL)
#endif
#ifndef MNT_DEFEXPORTED
#define MNT_DEFEXPORTED                                                        \
  (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000200UL)
#endif
#ifndef MNT_EXPORTANON
#define MNT_EXPORTANON (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000400UL)
#endif
#ifndef MNT_EXKERB
#define MNT_EXKERB (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000800UL)
#endif
#ifndef MNT_LOCAL
#define MNT_LOCAL (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00001000UL)
#endif
#ifndef MNT_QUOTA
#define MNT_QUOTA (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00002000UL)
#endif
#ifndef MNT_ROOTFS
#define MNT_ROOTFS (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00004000UL)
#endif
#ifndef MNT_USER
#define MNT_USER (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00008000UL)
#endif
#ifndef MNT_IGNORE
#define MNT_IGNORE (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00800000UL)
#endif
#ifndef MNT_UPDATE
#define MNT_UPDATE (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00010000UL)
#endif
#ifndef MNT_DELEXPORT
#define MNT_DELEXPORT (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00020000UL)
#endif
#ifndef MNT_RELOAD
#define MNT_RELOAD (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00040000UL)
#endif
#ifndef MNT_FORCE
#define MNT_FORCE (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00080000UL)
#endif
#ifndef MNT_LAZY
#define MNT_LAZY (((uint64_t)0x00000000UL << 32) | (uint64_t)0x01000000UL)
#endif
#ifndef MNT_SNAPSHOT
#define MNT_SNAPSHOT (((uint64_t)0x00000001UL << 32) | (uint64_t)0x00000000UL)
#endif
#ifndef MNT_BYFSID
#define MNT_BYFSID (((uint64_t)0x00000000UL << 32) | (uint64_t)0x08000000UL)
#endif

#ifndef MNT_WAIT
#define MNT_WAIT 1
#endif
#ifndef MNT_NOWAIT
#define MNT_NOWAIT 2
#endif

#ifndef VFCF_STATIC
#define VFCF_STATIC 0x00010000
#define VFCF_NETWORK 0x00020000
#define VFCF_READONLY 0x00040000
#define VFCF_SYNTHETIC 0x00080000
#define VFCF_LOOPBACK 0x00100000
#define VFCF_UNICODE 0x00200000
#define VFCF_JAIL 0x00400000
#define VFCF_DELEGADMIN 0x00800000
#endif
/** @endcond */

/** @brief FreeBSD virtual filesystem configuration descriptor. */
struct xvfsconf {
  void *vfc_vfsops;          /**< Filesystem operations vector pointer. */
  char vfc_name[MFSNAMELEN]; /**< Filesystem type name. */
  int vfc_typenum;           /**< Historic filesystem type number. */
  int vfc_refcount;          /**< Number of mounted instances. */
  int vfc_flags;             /**< VFCF_* capability flags. */
  void *vfc_next;            /**< Next entry pointer. */
};

#if !defined(__APPLE__)
/** @brief Opaque kernel file handle structure. */
typedef struct fhandle {
  char fh_fid[32]; /**< Raw file identifier bytes. */
} fhandle_t;

/** @brief Retrieve mounted filesystem statistics array. */
int getmntinfo(struct statfs **mntbufp, int flags);
/** @brief Retrieve list of all mounted filesystems. */
int getfsstat(struct statfs *buf, long bufsize, int mode);
#endif
/** @brief Query filesystem statistics by file handle. */
int fhstatfs(const fhandle_t *fhp, struct statfs *buf);
#if !defined(__FreeBSD__) && !defined(__APPLE__)
/** @brief Retrieve xvfsconf entry for a named filesystem type. */
int getvfsbyname(const char *filesystemname, struct xvfsconf *vfcp);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_MOUNT_H */
