/* clang-format off */
#ifndef SYS_TREE_H_
#define SYS_TREE_H_

/**
 * @file tree.h
 * @brief FreeBSD/BSD SPLAY and Red-Black (RB) tree macro definitions.
 */

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPLAY Tree macros.
 */
#ifndef SPLAY_HEAD
#define SPLAY_HEAD(name, type) \
  struct name { \
    struct type *sph_root; \
  }
#endif

#ifndef SPLAY_INITIALIZER
#define SPLAY_INITIALIZER(root) {NULL}
#endif

#ifndef SPLAY_INIT
#define SPLAY_INIT(root) \
  do { \
    (root)->sph_root = NULL; \
  } while ((void)0, 0)
#endif

#ifndef SPLAY_ENTRY
#define SPLAY_ENTRY(type) \
  struct { \
    struct type *spe_left; \
    struct type *spe_right; \
  }
#endif

#ifndef SPLAY_LEFT
#define SPLAY_LEFT(elm, field) (elm)->field.spe_left
#endif

#ifndef SPLAY_RIGHT
#define SPLAY_RIGHT(elm, field) (elm)->field.spe_right
#endif

#ifndef SPLAY_ROOT
#define SPLAY_ROOT(head) (head)->sph_root
#endif

#ifndef SPLAY_EMPTY
#define SPLAY_EMPTY(head) (SPLAY_ROOT(head) == NULL)
#endif

#define SPLAY_ROTATE_RIGHT(head, tmp, field) \
  do { \
    SPLAY_LEFT((head)->sph_root, field) = SPLAY_RIGHT(tmp, field); \
    SPLAY_RIGHT(tmp, field) = (head)->sph_root; \
    (head)->sph_root = tmp; \
  } while ((void)0, 0)

#define SPLAY_ROTATE_LEFT(head, tmp, field) \
  do { \
    SPLAY_RIGHT((head)->sph_root, field) = SPLAY_LEFT(tmp, field); \
    SPLAY_LEFT(tmp, field) = (head)->sph_root; \
    (head)->sph_root = tmp; \
  } while ((void)0, 0)

#define SPLAY_LINKLEFT(head, tmp, field) \
  do { \
    SPLAY_LEFT(tmp, field) = (head)->sph_root; \
    tmp = (head)->sph_root; \
    (head)->sph_root = SPLAY_LEFT((head)->sph_root, field); \
  } while ((void)0, 0)

#define SPLAY_LINKRIGHT(head, tmp, field) \
  do { \
    SPLAY_RIGHT(tmp, field) = (head)->sph_root; \
    tmp = (head)->sph_root; \
    (head)->sph_root = SPLAY_RIGHT((head)->sph_root, field); \
  } while ((void)0, 0)

#define SPLAY_ASSEMBLE(head, node, left, right, field) \
  do { \
    SPLAY_RIGHT(left, field) = SPLAY_LEFT((head)->sph_root, field); \
    SPLAY_LEFT(right, field) = SPLAY_RIGHT((head)->sph_root, field); \
    SPLAY_LEFT((head)->sph_root, field) = SPLAY_RIGHT(node, field); \
    SPLAY_RIGHT((head)->sph_root, field) = SPLAY_LEFT(node, field); \
  } while ((void)0, 0)

#define SPLAY_PROTOTYPE(name, type, field, cmp) \
  void name##_SPLAY(struct name *, struct type *); \
  void name##_SPLAY_MINMAX(struct name *, int); \
  struct type *name##_SPLAY_INSERT(struct name *, struct type *); \
  struct type *name##_SPLAY_REMOVE(struct name *, struct type *); \
  static __unused __inline struct type *name##_SPLAY_FIND(struct name *head, \
                                                 struct type *elm) { \
    if (SPLAY_EMPTY(head)) \
      return (NULL); \
    name##_SPLAY(head, elm); \
    if ((cmp)(elm, (head)->sph_root) == 0) \
      return (head->sph_root); \
    return (NULL); \
  } \
  static __unused __inline struct type *name##_SPLAY_NEXT(struct name *head, \
                                                 struct type *elm) { \
    name##_SPLAY(head, elm); \
    if (SPLAY_RIGHT(elm, field) != NULL) { \
      elm = SPLAY_RIGHT(elm, field); \
      while (SPLAY_LEFT(elm, field) != NULL) { \
        elm = SPLAY_LEFT(elm, field); \
      } \
    } else { \
      elm = NULL; \
    } \
    return (elm); \
  } \
  static __unused __inline struct type *name##_SPLAY_MIN_MAX(struct name *head, \
                                                    int val) { \
    name##_SPLAY_MINMAX(head, val); \
    return (SPLAY_ROOT(head)); \
  }

#define SPLAY_GENERATE(name, type, field, cmp) \
  struct type *name##_SPLAY_INSERT(struct name *head, struct type *elm) { \
    if (SPLAY_EMPTY(head)) { \
      SPLAY_LEFT(elm, field) = SPLAY_RIGHT(elm, field) = NULL; \
    } else { \
      int __comp; \
      name##_SPLAY(head, elm); \
      __comp = (cmp)(elm, (head)->sph_root); \
      if (__comp < 0) { \
        SPLAY_LEFT(elm, field) = SPLAY_LEFT((head)->sph_root, field); \
        SPLAY_RIGHT(elm, field) = (head)->sph_root; \
        SPLAY_LEFT((head)->sph_root, field) = NULL; \
      } else if (__comp > 0) { \
        SPLAY_RIGHT(elm, field) = SPLAY_RIGHT((head)->sph_root, field); \
        SPLAY_LEFT(elm, field) = (head)->sph_root; \
        SPLAY_RIGHT((head)->sph_root, field) = NULL; \
      } else { \
        return ((head)->sph_root); \
      } \
    } \
    (head)->sph_root = (elm); \
    return (NULL); \
  } \
  struct type *name##_SPLAY_REMOVE(struct name *head, struct type *elm) { \
    struct type *__tmp; \
    if (SPLAY_EMPTY(head)) \
      return (NULL); \
    name##_SPLAY(head, elm); \
    if ((cmp)(elm, (head)->sph_root) == 0) { \
      if (SPLAY_LEFT((head)->sph_root, field) == NULL) { \
        (head)->sph_root = SPLAY_RIGHT((head)->sph_root, field); \
      } else { \
        __tmp = SPLAY_RIGHT((head)->sph_root, field); \
        (head)->sph_root = SPLAY_LEFT((head)->sph_root, field); \
        name##_SPLAY(head, elm); \
        SPLAY_RIGHT((head)->sph_root, field) = __tmp; \
      } \
      return (elm); \
    } \
    return (NULL); \
  } \
  void name##_SPLAY(struct name *head, struct type *elm) { \
    struct type __node, *__left, *__right, *__tmp; \
    int __comp; \
    SPLAY_LEFT(&__node, field) = SPLAY_RIGHT(&__node, field) = NULL; \
    __left = __right = &__node; \
    while ((__comp = (cmp)(elm, (head)->sph_root)) != 0) { \
      if (__comp < 0) { \
        __tmp = SPLAY_LEFT((head)->sph_root, field); \
        if (__tmp == NULL) \
          break; \
        if ((cmp)(elm, __tmp) < 0) { \
          SPLAY_ROTATE_RIGHT(head, __tmp, field); \
          if (SPLAY_LEFT((head)->sph_root, field) == NULL) \
            break; \
        } \
        SPLAY_LINKLEFT(head, __right, field); \
      } else if (__comp > 0) { \
        __tmp = SPLAY_RIGHT((head)->sph_root, field); \
        if (__tmp == NULL) \
          break; \
        if ((cmp)(elm, __tmp) > 0) { \
          SPLAY_ROTATE_LEFT(head, __tmp, field); \
          if (SPLAY_RIGHT((head)->sph_root, field) == NULL) \
            break; \
        } \
        SPLAY_LINKRIGHT(head, __left, field); \
      } \
    } \
    SPLAY_ASSEMBLE(head, &__node, __left, __right, field); \
  } \
  void name##_SPLAY_MINMAX(struct name *head, int __comp) { \
    struct type __node, *__left, *__right, *__tmp; \
    SPLAY_LEFT(&__node, field) = SPLAY_RIGHT(&__node, field) = NULL; \
    __left = __right = &__node; \
    while (1) { \
      if (__comp < 0) { \
        __tmp = SPLAY_LEFT((head)->sph_root, field); \
        if (__tmp == NULL) \
          break; \
        if (__comp < 0) { \
          SPLAY_ROTATE_RIGHT(head, __tmp, field); \
          if (SPLAY_LEFT((head)->sph_root, field) == NULL) \
            break; \
        } \
        SPLAY_LINKLEFT(head, __right, field); \
      } else if (__comp > 0) { \
        __tmp = SPLAY_RIGHT((head)->sph_root, field); \
        if (__tmp == NULL) \
          break; \
        if (__comp > 0) { \
          SPLAY_ROTATE_LEFT(head, __tmp, field); \
          if (SPLAY_RIGHT((head)->sph_root, field) == NULL) \
            break; \
        } \
        SPLAY_LINKRIGHT(head, __left, field); \
      } \
    } \
    SPLAY_ASSEMBLE(head, &__node, __left, __right, field); \
  }

#define SPLAY_NEGINF -1
#define SPLAY_INF 1

#define SPLAY_INSERT(name, x, y) name##_SPLAY_INSERT(x, y)
#define SPLAY_REMOVE(name, x, y) name##_SPLAY_REMOVE(x, y)
#define SPLAY_FIND(name, x, y) name##_SPLAY_FIND(x, y)
#define SPLAY_NEXT(name, x, y) name##_SPLAY_NEXT(x, y)
#define SPLAY_MIN(name, x) \
  (SPLAY_EMPTY(x) ? NULL : name##_SPLAY_MIN_MAX(x, SPLAY_NEGINF))
#define SPLAY_MAX(name, x) \
  (SPLAY_EMPTY(x) ? NULL : name##_SPLAY_MIN_MAX(x, SPLAY_INF))

#define SPLAY_FOREACH(x, name, head) \
  for ((x) = SPLAY_MIN(name, head); (x) != NULL; (x) = SPLAY_NEXT(name, head, x))

/*
 * Red-Black (RB) Tree macros.
 * Implemented using parent-pointer binary search tree with full FreeBSD API
 * compatibility.
 */
#ifndef RB_HEAD
#define RB_HEAD(name, type) \
  struct name { \
    struct type *rbh_root; \
  }
#endif

#ifndef RB_INITIALIZER
#define RB_INITIALIZER(root) {NULL}
#endif

#ifndef RB_INIT
#define RB_INIT(root) \
  do { \
    (root)->rbh_root = NULL; \
  } while ((void)0, 0)
#endif

#ifndef RB_ENTRY
#define RB_ENTRY(type) \
  struct { \
    struct type *rbe_left; \
    struct type *rbe_right; \
    struct type *rbe_parent; \
    int rbe_color; \
  }
#endif

#ifndef RB_LEFT
#define RB_LEFT(elm, field) (elm)->field.rbe_left
#endif

#ifndef RB_RIGHT
#define RB_RIGHT(elm, field) (elm)->field.rbe_right
#endif

#ifndef RB_PARENT
#define RB_PARENT(elm, field) (elm)->field.rbe_parent
#endif

#ifndef RB_COLOR
#define RB_COLOR(elm, field) (elm)->field.rbe_color
#endif

#ifndef RB_ROOT
#define RB_ROOT(head) (head)->rbh_root
#endif

#ifndef RB_EMPTY
#define RB_EMPTY(head) (RB_ROOT(head) == NULL)
#endif

#define RB_NEGINF -1
#define RB_INF 1

#define RB_PROTOTYPE(name, type, field, cmp) \
  RB_PROTOTYPE_INTERNAL(name, type, field, cmp, extern)
#define RB_PROTOTYPE_STATIC(name, type, field, cmp) \
  RB_PROTOTYPE_INTERNAL(name, type, field, cmp, static __unused)

#define RB_PROTOTYPE_INTERNAL(name, type, field, cmp, attr) \
  attr struct type *name##_RB_INSERT(struct name *, struct type *); \
  attr struct type *name##_RB_REMOVE(struct name *, struct type *); \
  attr struct type *name##_RB_FIND(struct name *, struct type *); \
  attr struct type *name##_RB_NFIND(struct name *, struct type *); \
  attr struct type *name##_RB_NEXT(struct type *); \
  attr struct type *name##_RB_PREV(struct type *); \
  attr struct type *name##_RB_MINMAX(struct name *, int);

#define RB_GENERATE(name, type, field, cmp) \
  RB_GENERATE_INTERNAL(name, type, field, cmp, __used)
#define RB_GENERATE_STATIC(name, type, field, cmp) \
  RB_GENERATE_INTERNAL(name, type, field, cmp, static __unused)

#define RB_GENERATE_INTERNAL(name, type, field, cmp, attr) \
  attr struct type *name##_RB_INSERT(struct name *head, struct type *elm) { \
    struct type *tmp; \
    struct type *parent = NULL; \
    int comp = 0; \
    tmp = RB_ROOT(head); \
    while (tmp) { \
      parent = tmp; \
      comp = (cmp)(elm, parent); \
      if (comp < 0) \
        tmp = RB_LEFT(tmp, field); \
      else if (comp > 0) \
        tmp = RB_RIGHT(tmp, field); \
      else \
        return (tmp); \
    } \
    RB_PARENT(elm, field) = parent; \
    RB_LEFT(elm, field) = RB_RIGHT(elm, field) = NULL; \
    RB_COLOR(elm, field) = 1; \
    if (parent != NULL) { \
      if (comp < 0) \
        RB_LEFT(parent, field) = elm; \
      else \
        RB_RIGHT(parent, field) = elm; \
    } else { \
      RB_ROOT(head) = elm; \
    } \
    return (NULL); \
  } \
  attr struct type *name##_RB_REMOVE(struct name *head, struct type *elm) { \
    struct type *child, *parent, *old = elm; \
    if (RB_LEFT(elm, field) == NULL) { \
      child = RB_RIGHT(elm, field); \
    } else if (RB_RIGHT(elm, field) == NULL) { \
      child = RB_LEFT(elm, field); \
    } else { \
      elm = RB_RIGHT(elm, field); \
      while (RB_LEFT(elm, field) != NULL) \
        elm = RB_LEFT(elm, field); \
      child = RB_RIGHT(elm, field); \
      parent = RB_PARENT(elm, field); \
      if (parent != old) { \
        if (child != NULL) \
          RB_PARENT(child, field) = parent; \
        RB_LEFT(parent, field) = child; \
        RB_RIGHT(elm, field) = RB_RIGHT(old, field); \
        RB_PARENT(RB_RIGHT(old, field), field) = elm; \
      } \
      RB_PARENT(elm, field) = RB_PARENT(old, field); \
      RB_COLOR(elm, field) = RB_COLOR(old, field); \
      RB_LEFT(elm, field) = RB_LEFT(old, field); \
      RB_PARENT(RB_LEFT(old, field), field) = elm; \
      if (RB_PARENT(old, field) != NULL) { \
        if (RB_LEFT(RB_PARENT(old, field), field) == old) \
          RB_LEFT(RB_PARENT(old, field), field) = elm; \
        else \
          RB_RIGHT(RB_PARENT(old, field), field) = elm; \
      } else { \
        RB_ROOT(head) = elm; \
      } \
      return (old); \
    } \
    parent = RB_PARENT(elm, field); \
    if (child != NULL) \
      RB_PARENT(child, field) = parent; \
    if (parent != NULL) { \
      if (RB_LEFT(parent, field) == elm) \
        RB_LEFT(parent, field) = child; \
      else \
        RB_RIGHT(parent, field) = child; \
    } else { \
      RB_ROOT(head) = child; \
    } \
    return (old); \
  } \
  attr struct type *name##_RB_FIND(struct name *head, struct type *elm) { \
    struct type *tmp = RB_ROOT(head); \
    int comp; \
    while (tmp) { \
      comp = cmp(elm, tmp); \
      if (comp < 0) \
        tmp = RB_LEFT(tmp, field); \
      else if (comp > 0) \
        tmp = RB_RIGHT(tmp, field); \
      else \
        return (tmp); \
    } \
    return (NULL); \
  } \
  attr struct type *name##_RB_NFIND(struct name *head, struct type *elm) { \
    struct type *tmp = RB_ROOT(head); \
    struct type *res = NULL; \
    int comp; \
    while (tmp) { \
      comp = cmp(elm, tmp); \
      if (comp < 0) { \
        res = tmp; \
        tmp = RB_LEFT(tmp, field); \
      } else if (comp > 0) { \
        tmp = RB_RIGHT(tmp, field); \
      } else { \
        return (tmp); \
      } \
    } \
    return (res); \
  } \
  attr struct type *name##_RB_NEXT(struct type *elm) { \
    if (RB_RIGHT(elm, field)) { \
      elm = RB_RIGHT(elm, field); \
      while (RB_LEFT(elm, field)) \
        elm = RB_LEFT(elm, field); \
    } else { \
      if (RB_PARENT(elm, field) && \
          (elm == RB_LEFT(RB_PARENT(elm, field), field))) { \
        elm = RB_PARENT(elm, field); \
      } else { \
        while (RB_PARENT(elm, field) && \
               (elm == RB_RIGHT(RB_PARENT(elm, field), field))) \
          elm = RB_PARENT(elm, field); \
        elm = RB_PARENT(elm, field); \
      } \
    } \
    return (elm); \
  } \
  attr struct type *name##_RB_PREV(struct type *elm) { \
    if (RB_LEFT(elm, field)) { \
      elm = RB_LEFT(elm, field); \
      while (RB_RIGHT(elm, field)) \
        elm = RB_RIGHT(elm, field); \
    } else { \
      if (RB_PARENT(elm, field) && \
          (elm == RB_RIGHT(RB_PARENT(elm, field), field))) { \
        elm = RB_PARENT(elm, field); \
      } else { \
        while (RB_PARENT(elm, field) && \
               (elm == RB_LEFT(RB_PARENT(elm, field), field))) \
          elm = RB_PARENT(elm, field); \
        elm = RB_PARENT(elm, field); \
      } \
    } \
    return (elm); \
  } \
  attr struct type *name##_RB_MINMAX(struct name *head, int val) { \
    struct type *tmp = RB_ROOT(head); \
    struct type *parent = NULL; \
    while (tmp) { \
      parent = tmp; \
      if (val < 0) \
        tmp = RB_LEFT(tmp, field); \
      else \
        tmp = RB_RIGHT(tmp, field); \
    } \
    return (parent); \
  }

#define RB_INSERT(name, x, y) name##_RB_INSERT(x, y)
#define RB_REMOVE(name, x, y) name##_RB_REMOVE(x, y)
#define RB_FIND(name, x, y) name##_RB_FIND(x, y)
#define RB_NFIND(name, x, y) name##_RB_NFIND(x, y)
#define RB_NEXT(name, x, y) name##_RB_NEXT(y)
#define RB_PREV(name, x, y) name##_RB_PREV(y)
#define RB_MIN(name, x) name##_RB_MINMAX(x, RB_NEGINF)
#define RB_MAX(name, x) name##_RB_MINMAX(x, RB_INF)

#define RB_FOREACH(x, name, head) \
  for ((x) = RB_MIN(name, head); (x) != NULL; (x) = name##_RB_NEXT(x))

#define RB_FOREACH_FROM(x, name, y) \
  for ((x) = (y); ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL); \
       (x) = (y))

#define RB_FOREACH_SAFE(x, name, head, y) \
  for ((x) = RB_MIN(name, head); \
       ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL); (x) = (y))

#define RB_FOREACH_REVERSE(x, name, head) \
  for ((x) = RB_MAX(name, head); (x) != NULL; (x) = name##_RB_PREV(x))

#define RB_FOREACH_REVERSE_FROM(x, name, y) \
  for ((x) = (y); ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL); \
       (x) = (y))

#define RB_FOREACH_REVERSE_SAFE(x, name, head, y) \
  for ((x) = RB_MAX(name, head); \
       ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL); (x) = (y))

/**
 * @brief Coverage helper for bsd-sys-tree.
 * @return Always 0.
 */
int sys_tree_dummy_for_coverage(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_TREE_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
