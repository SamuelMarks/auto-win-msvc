#ifndef SYS_LINKER_H
#define SYS_LINKER_H

/**
 * @file linker.h
 * @brief FreeBSD kernel dynamic linker (<sys/linker.h>) userland interface.
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
#include <sys/param.h>
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define KLDSYM_LOOKUP 1

struct kld_file_stat {
  int version;
  char name[MAXPATHLEN];
  int refs;
  int id;
  caddr_t address;
  size_t size;
  char pathname[MAXPATHLEN];
};

struct kld_sym_lookup {
  int version;
  char *symname;
  unsigned long symvalue;
  size_t symsize;
};

/** @brief Load a kernel linker file into the kernel. */
int kldload(const char *file);
/** @brief Unload a kernel linker file by file ID. */
int kldunload(int fileid);
/** @brief Unload a kernel linker file by file ID with flags. */
int kldunloadf(int fileid, int flags);
/** @brief Return the file ID of a loaded kernel linker file. */
int kldfind(const char *file);
/** @brief Return the file ID of the next loaded kernel linker file. */
int kldnext(int fileid);
/** @brief Retrieve status metadata for a kernel linker file ID. */
int kldstat(int fileid, struct kld_file_stat *stat);
/** @brief Return module ID of the first module in a kernel linker file. */
int kldfirstmod(int fileid);
/** @brief Look up symbol address and size in a kernel linker file. */
int kldsym(int fileid, int cmd, void *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_LINKER_H */
