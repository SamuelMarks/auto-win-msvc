#ifndef POSIX_STDATOMIC_H
#define POSIX_STDATOMIC_H

/* Polyfill for <stdatomic.h> */

#if defined(_MSC_VER) && !defined(__clang__)

#include <intrin.h>

#define _Atomic(T) volatile T
#define ATOMIC_VAR_INIT(value) (value)
#define atomic_init(obj, value) do { *(obj) = (value); } while(0)

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

#define atomic_load(obj) (*(obj))
#define atomic_load_explicit(obj, order) (*(obj))
#define atomic_store(obj, val) do { *(obj) = (val); } while(0)
#define atomic_store_explicit(obj, val, order) do { *(obj) = (val); } while(0)

#define atomic_fetch_add(obj, val) _InterlockedExchangeAdd((volatile long*)(obj), (long)(val))
#define atomic_fetch_add_explicit(obj, val, order) _InterlockedExchangeAdd((volatile long*)(obj), (long)(val))

#define atomic_fetch_sub(obj, val) _InterlockedExchangeAdd((volatile long*)(obj), -(long)(val))
#define atomic_fetch_sub_explicit(obj, val, order) _InterlockedExchangeAdd((volatile long*)(obj), -(long)(val))

#define atomic_exchange(obj, val) _InterlockedExchange((volatile long*)(obj), (long)(val))
#define atomic_exchange_explicit(obj, val, order) _InterlockedExchange((volatile long*)(obj), (long)(val))

#define atomic_thread_fence(order) _ReadWriteBarrier()

#endif /* _MSC_VER && !__clang__ */

#endif /* POSIX_STDATOMIC_H */