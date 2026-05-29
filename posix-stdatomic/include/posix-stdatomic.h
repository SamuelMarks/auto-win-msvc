#ifndef POSIX_STDATOMIC_H
#define POSIX_STDATOMIC_H

/* Polyfill for <stdatomic.h> */

/* clang-format off */
#if defined(__GNUC__) || defined(__clang__)
#include_next <stdatomic.h>
#endif

#if defined(_MSC_VER) && !defined(__clang__)

#include <intrin.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** \brief _Atomic macro. */
#define _Atomic(X) volatile X
/** \brief ATOMIC_VAR_INIT macro. */
#define ATOMIC_VAR_INIT(value) (value)
/** \brief atomic_init macro. */
#define atomic_init(obj, value)                                                \
  do {                                                                         \
    *(obj) = (value);                                                          \
  } while (0)

typedef enum memory_order {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
} memory_order;

/* Types */
typedef volatile long atomic_int;
typedef volatile long atomic_long;
typedef volatile unsigned long atomic_uint;
typedef volatile unsigned long atomic_ulong;

/** \brief atomic_load macro. */
#define atomic_load(obj) (*(obj))
/** \brief atomic_load_explicit macro. */
#define atomic_load_explicit(obj, order) (*(obj))
/** \brief atomic_store macro. */
#define atomic_store(obj, val)                                                 \
  do {                                                                         \
    *(obj) = (val);                                                            \
  } while (0)
/** \brief atomic_store_explicit macro. */
#define atomic_store_explicit(obj, val, order)                                 \
  do {                                                                         \
    *(obj) = (val);                                                            \
  } while (0)

/** \brief atomic_fetch_add macro. */
#define atomic_fetch_add(obj, val)                                             \
  _InterlockedExchangeAdd((volatile long *)(obj), (long)(val))
/** \brief atomic_fetch_add_explicit macro. */
#define atomic_fetch_add_explicit(obj, val, order)                             \
  _InterlockedExchangeAdd((volatile long *)(obj), (long)(val))

/** \brief atomic_fetch_sub macro. */
#define atomic_fetch_sub(obj, val)                                             \
  _InterlockedExchangeAdd((volatile long *)(obj), -(long)(val))
/** \brief atomic_fetch_sub_explicit macro. */
#define atomic_fetch_sub_explicit(obj, val, order)                             \
  _InterlockedExchangeAdd((volatile long *)(obj), -(long)(val))

/** \brief atomic_exchange macro. */
#define atomic_exchange(obj, val)                                              \
  _InterlockedExchange((volatile long *)(obj), (long)(val))
/** \brief atomic_exchange_explicit macro. */
#define atomic_exchange_explicit(obj, val, order)                              \
  _InterlockedExchange((volatile long *)(obj), (long)(val))

/** \brief atomic_thread_fence macro. */
#define atomic_thread_fence(order) _ReadWriteBarrier()

#endif /* _MSC_VER && !__clang__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STDATOMIC_H */
