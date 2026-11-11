#ifndef CASPER_CAP_FILEARGS_H
#define CASPER_CAP_FILEARGS_H

/**
 * @file cap_fileargs.h
 * @brief FreeBSD Casper fileargs service passthrough shim.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/capsicum.h>
#include <libcasper.h>
#include <stdio.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Allow fileargs_open / fileargs_fopen. */
#define FA_OPEN 1
/** @brief Allow fileargs_lstat. */
#define FA_LSTAT 2
/** @brief Allow fileargs_realpath. */
#define FA_REALPATH 4

/** @brief Passthrough state for Casper fileargs service. */
struct fileargs {
  int flags;
  mode_t mode;
  int operations;
};
typedef struct fileargs fileargs_t;

/**
 * @brief Initialize fileargs structure from command-line arguments.
 * @param[in] argc Argument count.
 * @param[in] argv Argument vector.
 * @param[in] flags Open flags (O_*).
 * @param[in] mode Open creation mode.
 * @param[in] rightsp Optional Capsicum rights.
 * @param[in] operations Bitmask of FA_* operations.
 * @return Allocated fileargs_t pointer, or NULL on error.
 */
fileargs_t *fileargs_init(int argc, char *argv[], int flags, mode_t mode,
                          cap_rights_t *rightsp, int operations);

/**
 * @brief Initialize fileargs structure from an nvlist.
 * @param[in] limits Configuration nvlist.
 * @return Allocated fileargs_t pointer.
 */
fileargs_t *fileargs_initnv(nvlist_t *limits);

/**
 * @brief Open a file by name using the configured flags and mode.
 * @param[in] fa fileargs handle.
 * @param[in] name File path.
 * @return Open file descriptor, or -1 on error.
 */
int fileargs_open(fileargs_t *fa, const char *name);

/**
 * @brief Open a stdio FILE stream by name.
 * @param[in] fa fileargs handle.
 * @param[in] name File path.
 * @param[in] mode fopen mode string.
 * @return FILE pointer, or NULL on error.
 */
FILE *fileargs_fopen(fileargs_t *fa, const char *name, const char *mode);

/**
 * @brief Obtain file status via lstat.
 * @param[in] fa fileargs handle.
 * @param[in] name File path.
 * @param[out] sb Output stat buffer.
 * @return 0 on success, -1 on error.
 */
int fileargs_lstat(fileargs_t *fa, const char *name, struct stat *sb);

/**
 * @brief Resolve canonical path via realpath.
 * @param[in] fa fileargs handle.
 * @param[in] pathname Input path.
 * @param[out] reserved_path Output path buffer or NULL.
 * @return Resolved path string, or NULL on error.
 */
char *fileargs_realpath(fileargs_t *fa, const char *pathname,
                        char *reserved_path);

/**
 * @brief Enter capability mode after fileargs setup (no-op).
 * @param[in] fa fileargs handle.
 * @return Always 0.
 */
int fileargs_caph_enter(fileargs_t *fa);

/**
 * @brief Free fileargs handle.
 * @param[in] fa fileargs handle.
 */
void fileargs_free(fileargs_t *fa);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CASPER_CAP_FILEARGS_H */
