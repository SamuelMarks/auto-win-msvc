#ifndef BSD_FTS_H
#define BSD_FTS_H

/**
 * @file fts.h
 * @brief BSD file tree stream (<fts.h>) traversal API for Windows, DOS, and
 * POSIX.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/* fts_open options */
#define FTS_COMFOLLOW 0x0001  /* follow command line symlinks */
#define FTS_LOGICAL 0x0002    /* logical walk */
#define FTS_NOCHDIR 0x0004    /* don't change directories */
#define FTS_NOSTAT 0x0008     /* don't get stat info */
#define FTS_PHYSICAL 0x0010   /* physical walk */
#define FTS_SEEDOT 0x0020     /* return dot and dot-dot */
#define FTS_XDEV 0x0040       /* don't cross devices */
#define FTS_WHITEOUT 0x0080   /* return whiteout information */
#define FTS_OPTIONMASK 0x00ff /* valid user option mask */
#define FTS_NAMEONLY 0x0100   /* (private) child names only */
#define FTS_STOP 0x0200       /* (private) unrecoverable error */

/* FTSENT fts_info values */
#define FTS_D 1       /* preorder directory */
#define FTS_DC 2      /* directory that causes cycles */
#define FTS_DEFAULT 3 /* none of the above */
#define FTS_DNR 4     /* unreadable directory */
#define FTS_DOT 5     /* dot or dot-dot */
#define FTS_DP 6      /* postorder directory */
#define FTS_ERR 7     /* error; errno is set */
#define FTS_F 8       /* regular file */
#define FTS_INIT 9    /* initialized only */
#define FTS_NS 10     /* stat(2) failed */
#define FTS_NSOK 11   /* no stat(2) requested */
#define FTS_SL 12     /* symbolic link */
#define FTS_SLNONE 13 /* symbolic link without target */
#define FTS_W 14      /* whiteout object */

/* fts_set instructions */
#define FTS_AGAIN 1   /* read node again */
#define FTS_FOLLOW 2  /* follow symbolic link */
#define FTS_NOINSTR 3 /* no instructions */
#define FTS_SKIP 4    /* discard node */

#define FTS_ROOTPARENTLEVEL -1
#define FTS_ROOTLEVEL 0

struct _ftsent;

/** @brief File hierarchy traversal stream state. */
typedef struct {
  struct _ftsent *fts_cur;    /* current node */
  struct _ftsent *fts_child;  /* linked list of children */
  struct _ftsent **fts_array; /* sort array */
  dev_t fts_dev;              /* starting device # */
  char *fts_path;             /* path for this descent */
  int fts_rfd;                /* fd for root */
  size_t fts_pathlen;         /* sizeof(path) */
  size_t fts_nitems;          /* elements in the sort array */
  int (*fts_compar)(const struct _ftsent *const *,
                    const struct _ftsent *const *); /* compare fn */
  int fts_options;                                  /* fts_open options */
  void *fts_clientptr; /*的数据 pointer for application */
} FTS;

/** @brief Entry representing a single file or directory in FTS traversal. */
typedef struct _ftsent {
  struct _ftsent *fts_cycle;  /* cycle node */
  struct _ftsent *fts_parent; /* parent directory */
  struct _ftsent *fts_link;   /* next file in directory */
  int64_t fts_number;         /* local numeric value */
  void *fts_pointer;          /* local address value */
  char *fts_accpath;          /* access path */
  char *fts_path;             /* root path */
  int fts_errno;              /* errno for this node */
  int fts_symfd;              /* fd for symlink */
  size_t fts_pathlen;         /* strlen(fts_path) */
  size_t fts_namelen;         /* strlen(fts_name) */
  ino_t fts_ino;              /* inode */
  dev_t fts_dev;              /* device */
  nlink_t fts_nlink;          /* link count */
  long fts_level;             /* depth (-1 to N) */
  unsigned short fts_info;    /* user status for FTSENT structure */
  unsigned short fts_flags;   /* private flags for FTSENT structure */
  unsigned short fts_instr;   /* fts_set() instructions */
  struct stat *fts_statp;     /* stat(2) information */
  char *fts_name;             /* file name */
  FTS *fts_fts;               /* backpointer to FTS */
  struct stat fts_statbuf;    /* inline stat storage */
  char fts_namebuf[260];      /* inline name storage */
} FTSENT;

/**
 * @brief Open a file hierarchy for traversal.
 * @param[in] path_argv Null-terminated array of root paths.
 * @param[in] options Bitmask of FTS_* flags.
 * @param[in] compar Optional comparison callback.
 * @return Allocated FTS stream, or NULL on error.
 */
FTS *fts_open(char *const *path_argv, int options,
              int (*compar)(const FTSENT *const *, const FTSENT *const *));

/**
 * @brief Read the next entry in the file hierarchy.
 * @param[in,out] sp FTS stream.
 * @return Pointer to next FTSENT, or NULL on completion/error.
 */
FTSENT *fts_read(FTS *sp);

/**
 * @brief Return linked list of children of the current directory entry.
 * @param[in,out] sp FTS stream.
 * @param[in] instr Optional FTS_NAMEONLY flag.
 * @return Linked list of FTSENT children, or NULL.
 */
FTSENT *fts_children(FTS *sp, int instr);

/**
 * @brief Set traversal instruction on an entry.
 * @param[in,out] sp FTS stream.
 * @param[in,out] p FTSENT node.
 * @param[in] instr FTS_AGAIN, FTS_FOLLOW, or FTS_SKIP.
 * @return 0 on success, -1 on error.
 */
int fts_set(FTS *sp, FTSENT *p, int instr);

/**
 * @brief Close FTS stream and free all associated resources.
 * @param[in,out] sp FTS stream.
 * @return 0 on success, -1 on error.
 */
int fts_close(FTS *sp);

/**
 * @brief Get client pointer associated with FTS stream.
 * @param[in] sp FTS stream.
 * @return Client pointer.
 */
void *fts_get_clientptr(FTS *sp);

/**
 * @brief Get parent FTS stream from an FTSENT node.
 * @param[in] p FTSENT node.
 * @return FTS stream pointer.
 */
FTS *fts_get_stream(FTSENT *p);

/**
 * @brief Set client pointer associated with FTS stream.
 * @param[in,out] sp FTS stream.
 * @param[in] clientptr Client pointer.
 */
void fts_set_clientptr(FTS *sp, void *clientptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_FTS_H */
