#ifndef BSD_MEMSTAT_H
#define BSD_MEMSTAT_H

/**
 * @file memstat.h
 * @brief FreeBSD kernel memory allocator statistics (<memstat.h>) polyfill.
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

#define MEMSTAT_MAXNAME 32
#define MEMSTAT_MAXCPU 256

struct memory_type_list;
struct memory_type;

/** @brief Allocate a memory_type_list structure. */
struct memory_type_list *memstat_mtl_alloc(void);
/** @brief Return first entry in memory_type_list. */
struct memory_type *memstat_mtl_first(struct memory_type_list *list);
/** @brief Return next entry after mtp in memory_type_list. */
struct memory_type *memstat_mtl_next(struct memory_type *mtp);
/** @brief Find memory_type entry by allocator and name. */
struct memory_type *memstat_mtl_find(struct memory_type_list *list,
                                     int allocator, const char *name);
/** @brief Free memory_type_list and all entries. */
void memstat_mtl_free(struct memory_type_list *list);
/** @brief Return last error code recorded on memory_type_list. */
int memstat_mtl_geterror(struct memory_type_list *list);

/** @brief Populate memory_type_list from all allocators via sysctl. */
int memstat_sysctl_all(struct memory_type_list *list, int flags);
/** @brief Populate memory_type_list from UMA zone allocator via sysctl. */
int memstat_sysctl_uma(struct memory_type_list *list, int flags);
/** @brief Populate memory_type_list from kernel malloc via sysctl. */
int memstat_sysctl_malloc(struct memory_type_list *list, int flags);
/** @brief Populate memory_type_list from all allocators via kvm. */
int memstat_kvm_all(struct memory_type_list *list, void *kvm_handle);
/** @brief Populate memory_type_list from UMA zone allocator via kvm. */
int memstat_kvm_uma(struct memory_type_list *list, void *kvm_handle);
/** @brief Populate memory_type_list from kernel malloc via kvm. */
int memstat_kvm_malloc(struct memory_type_list *list, void *kvm_handle);

/** @brief Return name of memory type. */
const char *memstat_get_name(const struct memory_type *mtp);
/** @brief Return allocator identifier of memory type. */
int memstat_get_allocator(const struct memory_type *mtp);
/** @brief Return maximum item count limit of memory type. */
uint64_t memstat_get_countlimit(const struct memory_type *mtp);
/** @brief Return maximum byte limit of memory type. */
uint64_t memstat_get_byteslimit(const struct memory_type *mtp);
/** @brief Return allocation size bitmask of memory type. */
uint64_t memstat_get_sizemask(const struct memory_type *mtp);
/** @brief Return item size in bytes of memory type. */
uint64_t memstat_get_size(const struct memory_type *mtp);
/** @brief Return total bytes allocated over lifetime. */
uint64_t memstat_get_memalloced(const struct memory_type *mtp);
/** @brief Return total bytes freed over lifetime. */
uint64_t memstat_get_memfreed(const struct memory_type *mtp);
/** @brief Return total allocation requests over lifetime. */
uint64_t memstat_get_numallocs(const struct memory_type *mtp);
/** @brief Return total free requests over lifetime. */
uint64_t memstat_get_numfrees(const struct memory_type *mtp);
/** @brief Return currently allocated bytes. */
uint64_t memstat_get_bytes(const struct memory_type *mtp);
/** @brief Return currently allocated item count. */
uint64_t memstat_get_count(const struct memory_type *mtp);
/** @brief Return cached free item count. */
uint64_t memstat_get_free(const struct memory_type *mtp);
/** @brief Return allocation failure count. */
uint64_t memstat_get_failures(const struct memory_type *mtp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_MEMSTAT_H */
