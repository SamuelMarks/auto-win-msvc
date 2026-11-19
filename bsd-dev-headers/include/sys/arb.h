#ifndef SYS_ARB_H
#define SYS_ARB_H

/**
 * @file arb.h
 * @brief FreeBSD array-based red-black tree (<sys/arb.h>) header stub.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define ARB_HEAD(name, type)                                                   \
  struct name {                                                                \
    int arb_root;                                                              \
  }
#define ARB_ENTRY(type)                                                        \
  struct {                                                                     \
    int arb_parent;                                                            \
    int arb_left;                                                              \
    int arb_right;                                                             \
    int arb_color;                                                             \
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_ARB_H */
