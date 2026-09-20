/* posix-mman.h - Strict C89 Header */
#ifndef POSIX_MMAN_H
#define POSIX_MMAN_H

/**
 * @file posix-mman.h
 * @brief Memory management declarations (POSIX mman compatible).
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__MSDOS__) && !defined(__WATCOMC__)
#include <sys/mman.h>
#else
#include <stddef.h>
#include <sys/types.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
/** @brief Error type representation for non-standard functions. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__MSDOS__) &&             \
    !defined(__WATCOMC__)

#ifndef MADV_NORMAL
/** \brief No further special treatment. */
#define MADV_NORMAL 0
#endif
#ifndef MADV_RANDOM
/** \brief Expect page references in random order. */
#define MADV_RANDOM 1
#endif
#ifndef MADV_SEQUENTIAL
/** \brief Expect page references in sequential order. */
#define MADV_SEQUENTIAL 2
#endif
#ifndef MADV_WILLNEED
/** \brief Expect access in the near future. */
#define MADV_WILLNEED 3
#endif
#ifndef MADV_DONTNEED
/** \brief Do not expect access in the near future. */
#define MADV_DONTNEED 4
#endif
#ifndef MADV_FREE
/** \brief Free pages only if memory pressure. */
#define MADV_FREE 8
#endif

#if !defined(__USE_MISC) && !defined(__USE_XOPEN2K)
/**
 * \brief Give advice about use of memory.
 * \param addr Starting address.
 * \param length Length of the memory region.
 * \param advice Advice to give.
 * \return 0 on success, -1 on failure.
 */
int madvise(void *addr, size_t length, int advice);
#endif

#if defined(__CYGWIN__)
#ifndef MCL_CURRENT
/** \brief MCL_CURRENT macro. */
#define MCL_CURRENT 0x01
#endif
#ifndef MCL_FUTURE
/** \brief MCL_FUTURE macro. */
#define MCL_FUTURE 0x02
#endif

/**
 * \brief Lock all pages mapped into the address space of the calling process.
 * \param flags Options for page locking.
 * \return 0 on success, -1 on failure.
 */
int mlockall(int flags);

/**
 * \brief Unlock all pages mapped into the address space of the calling process.
 * \return 0 on success, -1 on failure.
 */
int munlockall(void);
#endif /* defined(__CYGWIN__) */

#else

/* Protection flags */
#ifndef PROT_NONE
/** \brief Page cannot be accessed. */
#define PROT_NONE 0x00
#endif
#ifndef PROT_READ
/** \brief Page can be read. */
#define PROT_READ 0x01
#endif
#ifndef PROT_WRITE
/** \brief Page can be written. */
#define PROT_WRITE 0x02
#endif
#ifndef PROT_EXEC
/** \brief Page can be executed. */
#define PROT_EXEC 0x04
#endif

/* Mapping flags */
#ifndef MAP_SHARED
/** \brief Share this mapping. */
#define MAP_SHARED 0x01
#endif
#ifndef MAP_PRIVATE
/** \brief Create a private copy-on-write mapping. */
#define MAP_PRIVATE 0x02
#endif
#ifndef MAP_FIXED
/** \brief Interpret addr exactly. */
#define MAP_FIXED 0x10
#endif
#ifndef MAP_ANONYMOUS
/** \brief The mapping is not backed by any file. */
#define MAP_ANONYMOUS 0x20
#endif
#ifndef MAP_ANON
/** \brief Compatibility alias for MAP_ANONYMOUS. */
#define MAP_ANON MAP_ANONYMOUS
#endif
#ifndef MAP_FAILED
/** \brief MAP_FAILED macro. */
#define MAP_FAILED ((void *)(size_t)-1)
#endif

/* msync flags */
#ifndef MS_ASYNC
/** \brief Perform asynchronous writes. */
#define MS_ASYNC 0x01
#endif
#ifndef MS_SYNC
/** \brief Perform synchronous writes. */
#define MS_SYNC 0x02
#endif
#ifndef MS_INVALIDATE
/** \brief Invalidate cached data. */
#define MS_INVALIDATE 0x04
#endif

/* madvise advice flags */
#ifndef MADV_NORMAL
/** \brief No further special treatment. */
#define MADV_NORMAL 0
#endif
#ifndef MADV_RANDOM
/** \brief Expect random page references. */
#define MADV_RANDOM 1
#endif
#ifndef MADV_SEQUENTIAL
/** \brief Expect sequential page references. */
#define MADV_SEQUENTIAL 2
#endif
#ifndef MADV_WILLNEED
/** \brief Expect access in the near future. */
#define MADV_WILLNEED 3
#endif
#ifndef MADV_DONTNEED
/** \brief Do not expect access in the near future. */
#define MADV_DONTNEED 4
#endif
#ifndef MADV_FREE
/** \brief Pages can be freed immediately. */
#define MADV_FREE 8
#endif
#ifndef MADV_DONTDUMP
/** \brief Do not include these pages in a core dump. */
#define MADV_DONTDUMP 16
#endif

/* mlockall flags */
#ifndef MCL_CURRENT
/** \brief Lock all current pages. */
#define MCL_CURRENT 0x01
#endif
#ifndef MCL_FUTURE
/** \brief Lock all future pages. */
#define MCL_FUTURE 0x02
#endif

/* Windows mode_t polyfill if needed */
#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) &&                 \
    !defined(_MODE_T_) && !defined(__WATCOMC__)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
/** \brief mode_t type definition for file modes. */
typedef unsigned short mode_t;
#endif

/**
 * \brief Give advice about use of memory.
 * \param addr Starting address.
 * \param length Length of the memory region.
 * \param advice Advice to give.
 * \return 0 on success, -1 on failure.
 */
int madvise(void *addr, size_t length, int advice);

/**
 * \brief Lock a range of process address space into memory.
 * \param addr Starting address.
 * \param len Length of the memory to lock.
 * \return 0 on success, -1 on failure.
 */
int mlock(const void *addr, size_t len);

/**
 * \brief Lock all process address space into memory.
 * \param flags Bitwise combination of MCL_CURRENT and/or MCL_FUTURE.
 * \return 0 on success, -1 on failure.
 */
int mlockall(int flags);

/**
 * \brief Map files or devices into memory.
 * \param addr Starting address hint.
 * \param length Length of the mapping.
 * \param prot Memory protection flags.
 * \param flags Mapping flags.
 * \param fd File descriptor.
 * \param offset Offset within the file.
 * \return Mapped address on success, MAP_FAILED on failure.
 */
void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           off_t offset);

/**
 * \brief Set protection on a region of memory.
 * \param addr Starting address.
 * \param len Length of the region.
 * \param prot Memory protection flags.
 * \return 0 on success, -1 on failure.
 */
int mprotect(void *addr, size_t len, int prot);

/**
 * \brief Synchronize a memory-mapped file with physical storage.
 * \param addr Starting address.
 * \param length Length of the region.
 * \param flags Flags controlling synchronization mode.
 * \return 0 on success, -1 on failure.
 */
int msync(void *addr, size_t length, int flags);

/**
 * \brief Unlock a range of process address space.
 * \param addr Starting address.
 * \param len Length of the memory to unlock.
 * \return 0 on success, -1 on failure.
 */
int munlock(const void *addr, size_t len);

/**
 * \brief Unlock all process address space.
 * \return 0 on success, -1 on failure.
 */
int munlockall(void);

/**
 * \brief Unmap files or devices from memory.
 * \param addr Starting address.
 * \param length Length of the mapping.
 * \return 0 on success, -1 on failure.
 */
int munmap(void *addr, size_t length);

/**
 * \brief Open a shared memory object.
 * \param name Name of the shared memory object.
 * \param oflag Open flags.
 * \param mode File mode permissions.
 * \return File descriptor on success, -1 on failure.
 */
int shm_open(const char *name, int oflag, mode_t mode);

/**
 * \brief Remove a shared memory object.
 * \param name Name of the shared memory object.
 * \return 0 on success, -1 on failure.
 */
int shm_unlink(const char *name);

#endif /* !defined(_WIN32) && !defined(_WIN64) && !defined(__MSDOS__) &&       \
          !defined(__WATCOMC__) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_MMAN_H */
