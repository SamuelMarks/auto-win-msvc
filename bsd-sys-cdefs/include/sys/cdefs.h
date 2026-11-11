/* clang-format off */
#ifndef SYS_CDEFS_H_
#define SYS_CDEFS_H_

/**
 * @file cdefs.h
 * @brief Polyfill for FreeBSD/BSD <sys/cdefs.h> compiler attributes and macros.
 */

#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if defined(__has_include_next)
#if __has_include_next(<sys/cdefs.h>)
#pragma GCC system_header
#include_next <sys/cdefs.h>
#endif
#else
#pragma GCC system_header
#include_next <sys/cdefs.h>
#endif
#elif !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) && \
    !defined(__DOS__)
#include <sys/cdefs.h>
#endif

#if defined(__cplusplus)
#ifndef __BEGIN_DECLS
/** @brief Begin C linkage block. */
#define __BEGIN_DECLS extern "C" {
#endif
#ifndef __END_DECLS
/** @brief End C linkage block. */
#define __END_DECLS }
#endif
#else
#ifndef __BEGIN_DECLS
/** @brief Begin C linkage block. */
#define __BEGIN_DECLS
#endif
#ifndef __END_DECLS
/** @brief End C linkage block. */
#define __END_DECLS
#endif
#endif

#ifndef __P
/** @brief Legacy prototype macro. */
#define __P(protos) protos
#endif

#ifndef __CONCAT1
#define __CONCAT1(x, y) x##y
#endif
#ifndef __CONCAT
/** @brief Token concatenation macro. */
#define __CONCAT(x, y) __CONCAT1(x, y)
#endif
#ifndef __STRING
/** @brief Stringification macro. */
#define __STRING(x) #x
#endif
#ifndef __XSTRING
/** @brief Expanded stringification macro. */
#define __XSTRING(x) __STRING(x)
#endif

#undef __FBSDID
#ifndef __FBSDID
/** @brief FreeBSD RCS ID macro safely expanding to a dummy struct declaration. */
#define __FBSDID(s) struct __awm_fbsdid_hack
#endif
#undef __RCSID
#ifndef __RCSID
/** @brief RCS ID macro. */
#define __RCSID(s) struct __awm_rcsid_hack
#endif
#undef __SCCSID
#ifndef __SCCSID
/** @brief SCCS ID macro. */
#define __SCCSID(s) struct __awm_sccsid_hack
#endif
#undef __COPYRIGHT
#ifndef __COPYRIGHT
/** @brief Copyright string macro. */
#define __COPYRIGHT(s) struct __awm_copyright_hack
#endif
#undef __IDSTRING
#ifndef __IDSTRING
/** @brief Generic ID string macro. */
#define __IDSTRING(name, string) struct __awm_idstring_hack
#endif

#ifndef __dead2
#if defined(__GNUC__) || defined(__clang__)
#define __dead2 __attribute__((__noreturn__))
#else
/** @brief Function does not return. */
#define __dead2
#endif
#endif

#ifndef __pure
#if defined(__GNUC__) || defined(__clang__)
#define __pure __attribute__((__pure__))
#else
/** @brief Function has no side effects. */
#define __pure
#endif
#endif

#ifndef __pure2
#if defined(__GNUC__) || defined(__clang__)
#define __pure2 __attribute__((__const__))
#else
/** @brief Function return value depends only on arguments. */
#define __pure2
#endif
#endif

#ifndef __constfunc
#define __constfunc __pure2
#endif

#ifndef __unused
#if defined(__GNUC__) || defined(__clang__)
#define __unused __attribute__((__unused__))
#else
/** @brief Variable or parameter may be unused. */
#define __unused
#endif
#endif

#ifndef __used
#if defined(__GNUC__) || defined(__clang__)
#define __used __attribute__((__used__))
#else
/** @brief Symbol must be retained even if unreferenced. */
#define __used
#endif
#endif

#ifndef __packed
#if defined(__GNUC__) || defined(__clang__)
#define __packed __attribute__((__packed__))
#else
/** @brief Struct or union is packed. */
#define __packed
#endif
#endif

#ifndef __aligned
#if defined(__GNUC__) || defined(__clang__)
#define __aligned(x) __attribute__((__aligned__(x)))
#elif defined(_MSC_VER)
#define __aligned(x) __declspec(align(x))
#else
/** @brief Minimum alignment attribute. */
#define __aligned(x)
#endif
#endif

#ifndef __section
#if defined(__GNUC__) || defined(__clang__)
#define __section(x) __attribute__((__section__(x)))
#else
/** @brief Section placement attribute. */
#define __section(x)
#endif
#endif

#ifndef __noinline
#if defined(__GNUC__) || defined(__clang__)
#define __noinline __attribute__((__noinline__))
#elif defined(_MSC_VER)
#define __noinline __declspec(noinline)
#else
/** @brief Prevent function inlining. */
#define __noinline
#endif
#endif

#ifndef __always_inline
#if defined(__GNUC__) || defined(__clang__)
#define __always_inline __inline__ __attribute__((__always_inline__))
#elif defined(_MSC_VER)
#define __always_inline __forceinline
#else
/** @brief Force function inlining. */
#define __always_inline __inline
#endif
#endif

#ifndef __printflike
#if defined(__GNUC__) || defined(__clang__)
#define __printflike(fmtarg, firstvararg) __attribute__((__format__(__printf__, fmtarg, firstvararg)))
#else
/** @brief Printf format checking attribute. */
#define __printflike(fmtarg, firstvararg)
#endif
#endif

#ifndef __scanflike
#if defined(__GNUC__) || defined(__clang__)
#define __scanflike(fmtarg, firstvararg) __attribute__((__format__(__scanf__, fmtarg, firstvararg)))
#else
/** @brief Scanf format checking attribute. */
#define __scanflike(fmtarg, firstvararg)
#endif
#endif

#ifndef __printf0like
#define __printf0like(fmtarg, firstvararg) __printflike(fmtarg, firstvararg)
#endif

#ifndef __format_arg
#if defined(__GNUC__) || defined(__clang__)
#define __format_arg(fmtarg) __attribute__((__format_arg__(fmtarg)))
#else
/** @brief Format string pass-through attribute. */
#define __format_arg(fmtarg)
#endif
#endif

#ifndef __nonnull
#if defined(_MSC_VER) && _MSC_VER < 1400
#define __nonnull(x)
#else
/** @brief Non-null pointer parameter attribute. */
#define __nonnull(x)
#endif
#endif

#ifndef __nonnull_all
/** @brief All pointer parameters are non-null. */
#define __nonnull_all
#endif

#ifndef __result_use_check
/** @brief Warn if return value is ignored. */
#define __result_use_check
#endif

#ifndef __alloc_size
/** @brief Allocation size parameter attribute. */
#define __alloc_size(x)
#endif

#ifndef __alloc_size2
/** @brief Allocation count and size parameter attribute. */
#define __alloc_size2(n, x)
#endif

#ifndef __alloc_align
/** @brief Allocation alignment parameter attribute. */
#define __alloc_align(x)
#endif

#ifndef __returns_twice
/** @brief Function may return more than once (like setjmp/vfork). */
#define __returns_twice
#endif

#ifndef __weak_symbol
/** @brief Weak symbol attribute. */
#define __weak_symbol
#endif
#ifndef __weak_reference
/** @brief Weak symbol alias reference. */
#define __weak_reference(sym, alias)
#endif
#ifndef __strong_reference
/** @brief Strong symbol alias reference. */
#define __strong_reference(sym, aliassym)
#endif
#ifndef __sym_compat
/** @brief Symbol version compatibility macro. */
#define __sym_compat(sym, impl, verid)
#endif
#ifndef __sym_default
/** @brief Default symbol version macro. */
#define __sym_default(sym, impl, verid)
#endif
#ifndef __warn_references
/** @brief Linker warning reference macro. */
#define __warn_references(sym, msg)
#endif

#ifndef __DECONST
/** @brief Cast away const qualifier safely. */
#define __DECONST(type, var) ((type)(size_t)(const void *)(var))
#endif

#ifndef __DEVOLATILE
/** @brief Cast away volatile qualifier safely. */
#define __DEVOLATILE(type, var) ((type)(size_t)(volatile void *)(var))
#endif

#ifndef __DEQUALIFY
/** @brief Cast away const and volatile qualifiers safely. */
#define __DEQUALIFY(type, var) ((type)(size_t)(const volatile void *)(var))
#endif

#ifndef __offsetof
/** @brief Byte offset of member within structure. */
#define __offsetof(type, field) offsetof(type, field)
#endif

#ifndef __rangeof
/** @brief Byte range between start and end fields in a structure. */
#define __rangeof(type, start, end) (__offsetof(type, end) - __offsetof(type, start))
#endif

#ifndef __containerof
/** @brief Obtain pointer to enclosing structure from member pointer. */
#define __containerof(x, s, m) ((s *)((char *)__DEQUALIFY(void *, (x)) - __offsetof(s, m)))
#endif

#ifndef __predict_true
#if defined(__GNUC__) || defined(__clang__)
#define __predict_true(exp) __builtin_expect((exp), 1)
#else
/** @brief Branch prediction hint: likely true. */
#define __predict_true(exp) (exp)
#endif
#endif

#ifndef __predict_false
#if defined(__GNUC__) || defined(__clang__)
#define __predict_false(exp) __builtin_expect((exp), 0)
#else
/** @brief Branch prediction hint: likely false. */
#define __predict_false(exp) (exp)
#endif
#endif

#ifndef __Static_assert
/** @brief Compile-time assertion compatible with C89. */
#define __Static_assert(x, y) typedef char __CONCAT(__static_assert_, __LINE__)[(x) ? 1 : -1]
#endif

#ifndef _Static_assert
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define _Static_assert(x, y) __Static_assert(x, y)
#endif
#endif

#ifndef __alignof
#if defined(__GNUC__) || defined(__clang__)
#define __alignof(x) __alignof__(x)
#elif defined(_MSC_VER)
#define __alignof(x) __alignof(x)
#else
/** @brief Alignment requirement of type x. */
#define __alignof(x) sizeof(x)
#endif
#endif

#ifndef _Alignof
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define _Alignof(x) __alignof(x)
#endif
#endif

#ifndef _Alignas
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define _Alignas(x) __aligned(x)
#endif
#endif

#ifndef __bitcount16
/** @brief Count set bits in a 16-bit integer. */
static __inline int __awm_bitcount16(unsigned short _x) {
  _x = (unsigned short)((_x & 0x5555U) + ((_x & 0xaaaaU) >> 1));
  _x = (unsigned short)((_x & 0x3333U) + ((_x & 0xccccU) >> 2));
  _x = (unsigned short)((_x + (_x >> 4)) & 0x0f0fU);
  _x = (unsigned short)((_x + (_x >> 8)) & 0x00ffU);
  return (int)_x;
}
#define __bitcount16(x) __awm_bitcount16((unsigned short)(x))
#endif

#ifndef __bitcount32
/** @brief Count set bits in a 32-bit integer. */
static __inline int __awm_bitcount32(unsigned int _x) {
  _x = (_x & 0x55555555U) + ((_x & 0xaaaaaaaaU) >> 1);
  _x = (_x & 0x33333333U) + ((_x & 0xccccccccU) >> 2);
  _x = (_x + (_x >> 4)) & 0x0f0f0f0fU;
  _x = (_x + (_x >> 8));
  _x = (_x + (_x >> 16)) & 0x000000ffU;
  return (int)_x;
}
#define __bitcount32(x) __awm_bitcount32((unsigned int)(x))
#endif

#ifndef __bitcount64
/** @brief Count set bits in a 64-bit integer. */
#define __bitcount64(x)                                                        \
  (__awm_bitcount32((unsigned int)(((x) >> 16) >> 16)) +                       \
   __awm_bitcount32((unsigned int)((x) & 0xffffffffUL)))
#endif

#ifndef __bitcount
#define __bitcount(x) __bitcount32(x)
#endif

#ifndef __min_size
/** @brief Minimum array size qualifier in function parameters. */
#define __min_size(x) (x)
#endif

#ifndef __restrict
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define __restrict __restrict
#else
#define __restrict
#endif
#endif

#ifndef __unreachable
#if defined(__GNUC__) || defined(__clang__)
#define __unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
#define __unreachable() __assume(0)
#else
/** @brief Hint that code path is unreachable. */
#define __unreachable() ((void)0)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Coverage helper for bsd-sys-cdefs.
 * @return Always 0.
 */
int sys_cdefs_dummy_for_coverage(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CDEFS_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
