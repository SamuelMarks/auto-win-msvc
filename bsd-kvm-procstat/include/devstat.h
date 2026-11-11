#ifndef BSD_DEVSTAT_H
#define BSD_DEVSTAT_H

/**
 * @file devstat.h
 * @brief FreeBSD device statistics (<devstat.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define DEVSTAT_NAME_LEN 16

/** @brief Device statistics record. */
struct devstat {
  unsigned int sequence0;
  int allocated;
  unsigned int start_count;
  unsigned int end_count;
  char device_name[DEVSTAT_NAME_LEN];
  int unit_number;
  uint64_t bytes[4];
  uint64_t operations[4];
  uint32_t block_size;
  int device_type;
};

/** @brief Snapshot of system device statistics. */
struct statinfo {
  long cp_time[5];
  struct devinfo *dinfo;
};

/** @brief Device selection structure. */
struct device_selection {
  uint32_t device_number;
  char device_name[DEVSTAT_NAME_LEN];
  int unit_number;
  int selected;
};

/** @brief Devinfo container structure. */
struct devinfo {
  struct devstat *devices;
  unsigned char *mem_ptr;
  long generation;
  int numdevs;
};

/** @brief Verify userland/kernel devstat version compatibility. */
int devstat_checkversion(void *kd);
/** @brief Return count of registered devices. */
int devstat_getnumdevs(void *kd);
/** @brief Fetch device statistics snapshot. */
int devstat_getdevs(void *kd, struct statinfo *stats);
/** @brief Filter and select devices for display. */
int devstat_selectdevs(struct device_selection **dev_select, int *num_selected,
                       int *num_selections, long *select_generation,
                       long current_generation, struct devstat *devices,
                       int numdevs, void *matches, int num_matches,
                       char **dev_selections, int num_dev_selections,
                       int select_mode, int maxshowdevs, int perf_select);
/** @brief Compute delta statistics between two device snapshots. */
int devstat_compute_statistics(struct devstat *current,
                               struct devstat *previous, long double etime,
                               ...);
/** @brief Compute elapsed time between two bintime/timespec snapshots. */
long double devstat_compute_etime(void *cur_time, void *prev_time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_DEVSTAT_H */
