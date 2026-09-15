#ifndef POSIX_STDATOMIC_H
#define POSIX_STDATOMIC_H

/**
 * @file posix-stdatomic.h
 * @brief Polyfill for C11 <stdatomic.h> atomic operations.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Macro mapping _Atomic type qualifier. */
#define _Atomic(X) volatile X

/** @brief Macro initializing an atomic variable. */
#define ATOMIC_VAR_INIT(value) (value)

/** @brief Macro initializing an atomic object in place. */
#define atomic_init(obj, value)                                                \
  do {                                                                         \
    *(obj) = (value);                                                          \
  } while ((void)0, 0)

/**
 * @brief Memory ordering semantics for atomic operations.
 */
typedef enum memory_order {
  memory_order_relaxed = 0,
  memory_order_consume = 1,
  memory_order_acquire = 2,
  memory_order_release = 3,
  memory_order_acq_rel = 4,
  memory_order_seq_cst = 5
} memory_order;

/** @brief Atomic signed integer type. */
typedef volatile long atomic_int;
/** @brief Atomic long integer type. */
typedef volatile long atomic_long;
/** @brief Atomic unsigned integer type. */
typedef volatile unsigned long atomic_uint;
/** @brief Atomic unsigned long integer type. */
typedef volatile unsigned long atomic_ulong;

#if defined(_MSC_VER) && !defined(__clang__)

/** @brief Atomically load the value of an atomic object. */
#define atomic_load(obj) (*(obj))
/** @brief Atomically load the value of an atomic object with explicit ordering.
 */
#define atomic_load_explicit(obj, order) (*(obj))

/** @brief Atomically store a value into an atomic object. */
#define atomic_store(obj, val)                                                 \
  do {                                                                         \
    *(obj) = (val);                                                            \
  } while ((void)0, 0)
/** @brief Atomically store a value into an atomic object with explicit
 * ordering. */
#define atomic_store_explicit(obj, val, order)                                 \
  do {                                                                         \
    *(obj) = (val);                                                            \
  } while ((void)0, 0)

/** @brief Atomically add to an atomic object and return old value. */
#define atomic_fetch_add(obj, val)                                             \
  _InterlockedExchangeAdd((volatile long *)(obj), (long)(val))
/** @brief Atomically add to an atomic object with explicit ordering. */
#define atomic_fetch_add_explicit(obj, val, order)                             \
  _InterlockedExchangeAdd((volatile long *)(obj), (long)(val))

/** @brief Atomically subtract from an atomic object and return old value. */
#define atomic_fetch_sub(obj, val)                                             \
  _InterlockedExchangeAdd((volatile long *)(obj), -(long)(val))
/** @brief Atomically subtract from an atomic object with explicit ordering. */
#define atomic_fetch_sub_explicit(obj, val, order)                             \
  _InterlockedExchangeAdd((volatile long *)(obj), -(long)(val))

/** @brief Atomically exchange value with an atomic object. */
#define atomic_exchange(obj, val)                                              \
  _InterlockedExchange((volatile long *)(obj), (long)(val))
/** @brief Atomically exchange value with an atomic object with explicit
 * ordering. */
#define atomic_exchange_explicit(obj, val, order)                              \
  _InterlockedExchange((volatile long *)(obj), (long)(val))

/** @brief Explicit memory barrier fence. */
#define atomic_thread_fence(order) _ReadWriteBarrier()

#else

/** @brief Atomically load the value of an atomic object. */
#define atomic_load(obj) __atomic_load_n((obj), 5)
/** @brief Atomically load the value of an atomic object with explicit ordering.
 */
#define atomic_load_explicit(obj, order) __atomic_load_n((obj), (order))

/** @brief Atomically store a value into an atomic object. */
#define atomic_store(obj, val) __atomic_store_n((obj), (val), 5)
/** @brief Atomically store a value into an atomic object with explicit
 * ordering. */
#define atomic_store_explicit(obj, val, order)                                 \
  __atomic_store_n((obj), (val), (order))

/** @brief Atomically add to an atomic object and return old value. */
#define atomic_fetch_add(obj, val) __atomic_fetch_add((obj), (val), 5)
/** @brief Atomically add to an atomic object with explicit ordering. */
#define atomic_fetch_add_explicit(obj, val, order)                             \
  __atomic_fetch_add((obj), (val), (order))

/** @brief Atomically subtract from an atomic object and return old value. */
#define atomic_fetch_sub(obj, val) __atomic_fetch_sub((obj), (val), 5)
/** @brief Atomically subtract from an atomic object with explicit ordering. */
#define atomic_fetch_sub_explicit(obj, val, order)                             \
  __atomic_fetch_sub((obj), (val), (order))

/** @brief Atomically exchange value with an atomic object. */
#define atomic_exchange(obj, val) __atomic_exchange_n((obj), (val), 5)
/** @brief Atomically exchange value with an atomic object with explicit
 * ordering. */
#define atomic_exchange_explicit(obj, val, order)                              \
  __atomic_exchange_n((obj), (val), (order))

/** @brief Explicit memory barrier fence. */
#define atomic_thread_fence(order) __atomic_thread_fence(order)

#endif

/**
 * @brief Error codes returned by posix-stdatomic functions.
 */
enum posix_stdatomic_error_code {
  /** @brief Operation completed successfully. */
  POSIX_STDATOMIC_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_STDATOMIC_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on stdatomic polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STDATOMIC_SUCCESS on success, or an error code on failure.
 */
enum posix_stdatomic_error_code posix_stdatomic_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDATOMIC_H */
