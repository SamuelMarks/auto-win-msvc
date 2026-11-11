/* clang-format off */
#ifndef SYS_QUEUE_H_
#define SYS_QUEUE_H_

/**
 * @file queue.h
 * @brief FreeBSD/BSD singly-linked, doubly-linked, and tail-queue macro
 * definitions.
 */

#include <stddef.h>

#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if defined(__has_include_next)
#if __has_include_next(<sys/queue.h>)
#pragma GCC system_header
#include_next <sys/queue.h>
#endif
#else
#pragma GCC system_header
#include_next <sys/queue.h>
#endif
#elif !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) && \
    !defined(__DOS__)
#include <sys/queue.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Singly-linked List definitions.
 */
#ifndef SLIST_HEAD
#define SLIST_HEAD(name, type) \
  struct name { \
    struct type *slh_first; \
  }
#endif

#ifndef SLIST_HEAD_INITIALIZER
#define SLIST_HEAD_INITIALIZER(head) {NULL}
#endif

#ifndef SLIST_ENTRY
#define SLIST_ENTRY(type) \
  struct { \
    struct type *sle_next; \
  }
#endif

#ifndef SLIST_EMPTY
#define SLIST_EMPTY(head) ((head)->slh_first == NULL)
#endif

#ifndef SLIST_FIRST
#define SLIST_FIRST(head) ((head)->slh_first)
#endif

#ifndef SLIST_NEXT
#define SLIST_NEXT(elm, field) ((elm)->field.sle_next)
#endif

#ifndef SLIST_INIT
#define SLIST_INIT(head) \
  do { \
    SLIST_FIRST((head)) = NULL; \
  } while ((void)0, 0)
#endif

#ifndef SLIST_INSERT_AFTER
#define SLIST_INSERT_AFTER(slistelm, elm, field) \
  do { \
    SLIST_NEXT((elm), field) = SLIST_NEXT((slistelm), field); \
    SLIST_NEXT((slistelm), field) = (elm); \
  } while ((void)0, 0)
#endif

#ifndef SLIST_INSERT_HEAD
#define SLIST_INSERT_HEAD(head, elm, field) \
  do { \
    SLIST_NEXT((elm), field) = SLIST_FIRST((head)); \
    SLIST_FIRST((head)) = (elm); \
  } while ((void)0, 0)
#endif

#ifndef SLIST_REMOVE_HEAD
#define SLIST_REMOVE_HEAD(head, field) \
  do { \
    SLIST_FIRST((head)) = SLIST_NEXT(SLIST_FIRST((head)), field); \
  } while ((void)0, 0)
#endif

#ifndef SLIST_REMOVE_AFTER
#define SLIST_REMOVE_AFTER(elm, field) \
  do { \
    SLIST_NEXT(elm, field) = SLIST_NEXT(SLIST_NEXT(elm, field), field); \
  } while ((void)0, 0)
#endif

#ifndef SLIST_REMOVE
#define SLIST_REMOVE(head, elm, type, field) \
  do { \
    if (SLIST_FIRST((head)) == (elm)) { \
      SLIST_REMOVE_HEAD((head), field); \
    } else { \
      struct type *curelm = SLIST_FIRST((head)); \
      while (SLIST_NEXT(curelm, field) != (elm)) \
        curelm = SLIST_NEXT(curelm, field); \
      SLIST_REMOVE_AFTER(curelm, field); \
    } \
  } while ((void)0, 0)
#endif

#ifndef SLIST_FOREACH
#define SLIST_FOREACH(var, head, field) \
  for ((var) = SLIST_FIRST((head)); (var); (var) = SLIST_NEXT((var), field))
#endif

#ifndef SLIST_FOREACH_FROM
#define SLIST_FOREACH_FROM(var, head, field) \
  for ((var) = ((var) ? (var) : SLIST_FIRST((head))); (var); \
       (var) = SLIST_NEXT((var), field))
#endif

#ifndef SLIST_FOREACH_SAFE
#define SLIST_FOREACH_SAFE(var, head, field, tvar) \
  for ((var) = SLIST_FIRST((head)); \
       (var) && ((tvar) = SLIST_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef SLIST_FOREACH_FROM_SAFE
#define SLIST_FOREACH_FROM_SAFE(var, head, field, tvar) \
  for ((var) = ((var) ? (var) : SLIST_FIRST((head))); \
       (var) && ((tvar) = SLIST_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef SLIST_SWAP
#define SLIST_SWAP(head1, head2, type) \
  do { \
    struct type *swap_first = SLIST_FIRST(head1); \
    SLIST_FIRST(head1) = SLIST_FIRST(head2); \
    SLIST_FIRST(head2) = swap_first; \
  } while ((void)0, 0)
#endif

#ifndef SLIST_CONCAT
#define SLIST_CONCAT(head1, head2, type, field) \
  do { \
    struct type *curelm = SLIST_FIRST(head1); \
    if (curelm == NULL) { \
      if ((SLIST_FIRST(head1) = SLIST_FIRST(head2)) != NULL) \
        SLIST_INIT(head2); \
    } else if (SLIST_FIRST(head2) != NULL) { \
      while (SLIST_NEXT(curelm, field) != NULL) \
        curelm = SLIST_NEXT(curelm, field); \
      SLIST_NEXT(curelm, field) = SLIST_FIRST(head2); \
      SLIST_INIT(head2); \
    } \
  } while ((void)0, 0)
#endif

/*
 * Singly-linked Tail queue definitions.
 */
#ifndef STAILQ_HEAD
#define STAILQ_HEAD(name, type) \
  struct name { \
    struct type *stqh_first; \
    struct type **stqh_last; \
  }
#endif

#ifndef STAILQ_HEAD_INITIALIZER
#define STAILQ_HEAD_INITIALIZER(head) {NULL, &(head).stqh_first}
#endif

#ifndef STAILQ_ENTRY
#define STAILQ_ENTRY(type) \
  struct { \
    struct type *stqe_next; \
  }
#endif

#ifndef STAILQ_EMPTY
#define STAILQ_EMPTY(head) ((head)->stqh_first == NULL)
#endif

#ifndef STAILQ_FIRST
#define STAILQ_FIRST(head) ((head)->stqh_first)
#endif

#ifndef STAILQ_NEXT
#define STAILQ_NEXT(elm, field) ((elm)->field.stqe_next)
#endif

#ifndef STAILQ_INIT
#define STAILQ_INIT(head) \
  do { \
    STAILQ_FIRST((head)) = NULL; \
    (head)->stqh_last = &STAILQ_FIRST((head)); \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_INSERT_AFTER
#define STAILQ_INSERT_AFTER(head, tqelm, elm, field) \
  do { \
    if ((STAILQ_NEXT((elm), field) = STAILQ_NEXT((tqelm), field)) == NULL) \
      (head)->stqh_last = &STAILQ_NEXT((elm), field); \
    STAILQ_NEXT((tqelm), field) = (elm); \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_INSERT_HEAD
#define STAILQ_INSERT_HEAD(head, elm, field) \
  do { \
    if ((STAILQ_NEXT((elm), field) = STAILQ_FIRST((head))) == NULL) \
      (head)->stqh_last = &STAILQ_NEXT((elm), field); \
    STAILQ_FIRST((head)) = (elm); \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_INSERT_TAIL
#define STAILQ_INSERT_TAIL(head, elm, field) \
  do { \
    STAILQ_NEXT((elm), field) = NULL; \
    *(head)->stqh_last = (elm); \
    (head)->stqh_last = &STAILQ_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_REMOVE_HEAD
#define STAILQ_REMOVE_HEAD(head, field) \
  do { \
    if ((STAILQ_FIRST((head)) = STAILQ_NEXT(STAILQ_FIRST((head)), field)) == \
        NULL) \
      (head)->stqh_last = &STAILQ_FIRST((head)); \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_REMOVE_AFTER
#define STAILQ_REMOVE_AFTER(head, elm, field) \
  do { \
    if ((STAILQ_NEXT(elm, field) = \
             STAILQ_NEXT(STAILQ_NEXT(elm, field), field)) == NULL) \
      (head)->stqh_last = &STAILQ_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_REMOVE
#define STAILQ_REMOVE(head, elm, type, field) \
  do { \
    if (STAILQ_FIRST((head)) == (elm)) { \
      STAILQ_REMOVE_HEAD((head), field); \
    } else { \
      struct type *curelm = STAILQ_FIRST((head)); \
      while (STAILQ_NEXT(curelm, field) != (elm)) \
        curelm = STAILQ_NEXT(curelm, field); \
      STAILQ_REMOVE_AFTER(head, curelm, field); \
    } \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_LAST
#define STAILQ_LAST(head, type, field) \
  (STAILQ_EMPTY((head)) \
       ? NULL \
       : ((struct type *)(void *)((char *)((head)->stqh_last) - \
                                  offsetof(struct type, field))))
#endif

#ifndef STAILQ_FOREACH
#define STAILQ_FOREACH(var, head, field) \
  for ((var) = STAILQ_FIRST((head)); (var); (var) = STAILQ_NEXT((var), field))
#endif

#ifndef STAILQ_FOREACH_FROM
#define STAILQ_FOREACH_FROM(var, head, field) \
  for ((var) = ((var) ? (var) : STAILQ_FIRST((head))); (var); \
       (var) = STAILQ_NEXT((var), field))
#endif

#ifndef STAILQ_FOREACH_SAFE
#define STAILQ_FOREACH_SAFE(var, head, field, tvar) \
  for ((var) = STAILQ_FIRST((head)); \
       (var) && ((tvar) = STAILQ_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef STAILQ_FOREACH_FROM_SAFE
#define STAILQ_FOREACH_FROM_SAFE(var, head, field, tvar) \
  for ((var) = ((var) ? (var) : STAILQ_FIRST((head))); \
       (var) && ((tvar) = STAILQ_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef STAILQ_CONCAT
#define STAILQ_CONCAT(head1, head2) \
  do { \
    if (!STAILQ_EMPTY((head2))) { \
      *(head1)->stqh_last = (head2)->stqh_first; \
      (head1)->stqh_last = (head2)->stqh_last; \
      STAILQ_INIT((head2)); \
    } \
  } while ((void)0, 0)
#endif

#ifndef STAILQ_SWAP
#define STAILQ_SWAP(head1, head2, type) \
  do { \
    struct type *swap_first = STAILQ_FIRST(head1); \
    struct type **swap_last = (head1)->stqh_last; \
    STAILQ_FIRST(head1) = STAILQ_FIRST(head2); \
    (head1)->stqh_last = (head2)->stqh_last; \
    STAILQ_FIRST(head2) = swap_first; \
    (head2)->stqh_last = swap_last; \
    if (STAILQ_EMPTY(head1)) \
      (head1)->stqh_last = &STAILQ_FIRST(head1); \
    if (STAILQ_EMPTY(head2)) \
      (head2)->stqh_last = &STAILQ_FIRST(head2); \
  } while ((void)0, 0)
#endif

/*
 * List definitions.
 */
#ifndef LIST_HEAD
#define LIST_HEAD(name, type) \
  struct name { \
    struct type *lh_first; \
  }
#endif

#ifndef LIST_HEAD_INITIALIZER
#define LIST_HEAD_INITIALIZER(head) {NULL}
#endif

#ifndef LIST_ENTRY
#define LIST_ENTRY(type) \
  struct { \
    struct type *le_next; \
    struct type **le_prev; \
  }
#endif

#ifndef LIST_EMPTY
#define LIST_EMPTY(head) ((head)->lh_first == NULL)
#endif

#ifndef LIST_FIRST
#define LIST_FIRST(head) ((head)->lh_first)
#endif

#ifndef LIST_NEXT
#define LIST_NEXT(elm, field) ((elm)->field.le_next)
#endif

#ifndef LIST_PREV
#define LIST_PREV(elm, head, type, field) \
  ((elm)->field.le_prev == &LIST_FIRST((head)) \
       ? NULL \
       : ((struct type *)(void *)((char *)((elm)->field.le_prev) - \
                                  offsetof(struct type, field))))
#endif

#ifndef LIST_INIT
#define LIST_INIT(head) \
  do { \
    LIST_FIRST((head)) = NULL; \
  } while ((void)0, 0)
#endif

#ifndef LIST_INSERT_AFTER
#define LIST_INSERT_AFTER(listelm, elm, field) \
  do { \
    if ((LIST_NEXT((elm), field) = LIST_NEXT((listelm), field)) != NULL) \
      LIST_NEXT((listelm), field)->field.le_prev = &LIST_NEXT((elm), field); \
    LIST_NEXT((listelm), field) = (elm); \
    (elm)->field.le_prev = &LIST_NEXT((listelm), field); \
  } while ((void)0, 0)
#endif

#ifndef LIST_INSERT_BEFORE
#define LIST_INSERT_BEFORE(listelm, elm, field) \
  do { \
    (elm)->field.le_prev = (listelm)->field.le_prev; \
    LIST_NEXT((elm), field) = (listelm); \
    *(listelm)->field.le_prev = (elm); \
    (listelm)->field.le_prev = &LIST_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef LIST_INSERT_HEAD
#define LIST_INSERT_HEAD(head, elm, field) \
  do { \
    if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL) \
      LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field); \
    LIST_FIRST((head)) = (elm); \
    (elm)->field.le_prev = &LIST_FIRST((head)); \
  } while ((void)0, 0)
#endif

#ifndef LIST_REMOVE
#define LIST_REMOVE(elm, field) \
  do { \
    if (LIST_NEXT((elm), field) != NULL) \
      LIST_NEXT((elm), field)->field.le_prev = (elm)->field.le_prev; \
    *(elm)->field.le_prev = LIST_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef LIST_FOREACH
#define LIST_FOREACH(var, head, field) \
  for ((var) = LIST_FIRST((head)); (var); (var) = LIST_NEXT((var), field))
#endif

#ifndef LIST_FOREACH_FROM
#define LIST_FOREACH_FROM(var, head, field) \
  for ((var) = ((var) ? (var) : LIST_FIRST((head))); (var); \
       (var) = LIST_NEXT((var), field))
#endif

#ifndef LIST_FOREACH_SAFE
#define LIST_FOREACH_SAFE(var, head, field, tvar) \
  for ((var) = LIST_FIRST((head)); \
       (var) && ((tvar) = LIST_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef LIST_FOREACH_FROM_SAFE
#define LIST_FOREACH_FROM_SAFE(var, head, field, tvar) \
  for ((var) = ((var) ? (var) : LIST_FIRST((head))); \
       (var) && ((tvar) = LIST_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef LIST_SWAP
#define LIST_SWAP(head1, head2, type, field) \
  do { \
    struct type *swap_tmp = LIST_FIRST((head1)); \
    LIST_FIRST((head1)) = LIST_FIRST((head2)); \
    LIST_FIRST((head2)) = swap_tmp; \
    if ((swap_tmp = LIST_FIRST((head1))) != NULL) \
      swap_tmp->field.le_prev = &LIST_FIRST((head1)); \
    if ((swap_tmp = LIST_FIRST((head2))) != NULL) \
      swap_tmp->field.le_prev = &LIST_FIRST((head2)); \
  } while ((void)0, 0)
#endif

/*
 * Tail queue definitions.
 */
#ifndef TAILQ_HEAD
#define TAILQ_HEAD(name, type) \
  struct name { \
    struct type *tqh_first; \
    struct type **tqh_last; \
  }
#endif

#ifndef TAILQ_HEAD_INITIALIZER
#define TAILQ_HEAD_INITIALIZER(head) {NULL, &(head).tqh_first}
#endif

#ifndef TAILQ_ENTRY
#define TAILQ_ENTRY(type) \
  struct { \
    struct type *tqe_next; \
    struct type **tqe_prev; \
  }
#endif

#ifndef TAILQ_EMPTY
#define TAILQ_EMPTY(head) ((head)->tqh_first == NULL)
#endif

#ifndef TAILQ_FIRST
#define TAILQ_FIRST(head) ((head)->tqh_first)
#endif

#ifndef TAILQ_NEXT
#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)
#endif

#ifndef TAILQ_LAST
#define TAILQ_LAST(head, headname) \
  (*(((struct headname *)((head)->tqh_last))->tqh_last))
#endif

#ifndef TAILQ_PREV
#define TAILQ_PREV(elm, headname, field) \
  (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))
#endif

#ifndef TAILQ_INIT
#define TAILQ_INIT(head) \
  do { \
    TAILQ_FIRST((head)) = NULL; \
    (head)->tqh_last = &TAILQ_FIRST((head)); \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_INSERT_AFTER
#define TAILQ_INSERT_AFTER(head, listelm, elm, field) \
  do { \
    if ((TAILQ_NEXT((elm), field) = TAILQ_NEXT((listelm), field)) != NULL) \
      TAILQ_NEXT((elm), field)->field.tqe_prev = &TAILQ_NEXT((elm), field); \
    else \
      (head)->tqh_last = &TAILQ_NEXT((elm), field); \
    TAILQ_NEXT((listelm), field) = (elm); \
    (elm)->field.tqe_prev = &TAILQ_NEXT((listelm), field); \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_INSERT_BEFORE
#define TAILQ_INSERT_BEFORE(listelm, elm, field) \
  do { \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev; \
    TAILQ_NEXT((elm), field) = (listelm); \
    *(listelm)->field.tqe_prev = (elm); \
    (listelm)->field.tqe_prev = &TAILQ_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_INSERT_HEAD
#define TAILQ_INSERT_HEAD(head, elm, field) \
  do { \
    if ((TAILQ_NEXT((elm), field) = TAILQ_FIRST((head))) != NULL) \
      TAILQ_FIRST((head))->field.tqe_prev = &TAILQ_NEXT((elm), field); \
    else \
      (head)->tqh_last = &TAILQ_NEXT((elm), field); \
    TAILQ_FIRST((head)) = (elm); \
    (elm)->field.tqe_prev = &TAILQ_FIRST((head)); \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_INSERT_TAIL
#define TAILQ_INSERT_TAIL(head, elm, field) \
  do { \
    TAILQ_NEXT((elm), field) = NULL; \
    (elm)->field.tqe_prev = (head)->tqh_last; \
    *(head)->tqh_last = (elm); \
    (head)->tqh_last = &TAILQ_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_REMOVE
#define TAILQ_REMOVE(head, elm, field) \
  do { \
    if ((TAILQ_NEXT((elm), field)) != NULL) \
      TAILQ_NEXT((elm), field)->field.tqe_prev = (elm)->field.tqe_prev; \
    else \
      (head)->tqh_last = (elm)->field.tqe_prev; \
    *(elm)->field.tqe_prev = TAILQ_NEXT((elm), field); \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_FOREACH
#define TAILQ_FOREACH(var, head, field) \
  for ((var) = TAILQ_FIRST((head)); (var); (var) = TAILQ_NEXT((var), field))
#endif

#ifndef TAILQ_FOREACH_FROM
#define TAILQ_FOREACH_FROM(var, head, field) \
  for ((var) = ((var) ? (var) : TAILQ_FIRST((head))); (var); \
       (var) = TAILQ_NEXT((var), field))
#endif

#ifndef TAILQ_FOREACH_SAFE
#define TAILQ_FOREACH_SAFE(var, head, field, tvar) \
  for ((var) = TAILQ_FIRST((head)); \
       (var) && ((tvar) = TAILQ_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef TAILQ_FOREACH_FROM_SAFE
#define TAILQ_FOREACH_FROM_SAFE(var, head, field, tvar) \
  for ((var) = ((var) ? (var) : TAILQ_FIRST((head))); \
       (var) && ((tvar) = TAILQ_NEXT((var), field), 1); (var) = (tvar))
#endif

#ifndef TAILQ_FOREACH_REVERSE
#define TAILQ_FOREACH_REVERSE(var, head, headname, field) \
  for ((var) = TAILQ_LAST((head), headname); (var); \
       (var) = TAILQ_PREV((var), headname, field))
#endif

#ifndef TAILQ_FOREACH_REVERSE_FROM
#define TAILQ_FOREACH_REVERSE_FROM(var, head, headname, field) \
  for ((var) = ((var) ? (var) : TAILQ_LAST((head), headname)); (var); \
       (var) = TAILQ_PREV((var), headname, field))
#endif

#ifndef TAILQ_FOREACH_REVERSE_SAFE
#define TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, tvar) \
  for ((var) = TAILQ_LAST((head), headname); \
       (var) && ((tvar) = TAILQ_PREV((var), headname, field), 1); \
       (var) = (tvar))
#endif

#ifndef TAILQ_FOREACH_REVERSE_FROM_SAFE
#define TAILQ_FOREACH_REVERSE_FROM_SAFE(var, head, headname, field, tvar) \
  for ((var) = ((var) ? (var) : TAILQ_LAST((head), headname)); \
       (var) && ((tvar) = TAILQ_PREV((var), headname, field), 1); \
       (var) = (tvar))
#endif

#ifndef TAILQ_CONCAT
#define TAILQ_CONCAT(head1, head2, field) \
  do { \
    if (!TAILQ_EMPTY(head2)) { \
      *(head1)->tqh_last = (head2)->tqh_first; \
      (head2)->tqh_first->field.tqe_prev = (head1)->tqh_last; \
      (head1)->tqh_last = (head2)->tqh_last; \
      TAILQ_INIT((head2)); \
    } \
  } while ((void)0, 0)
#endif

#ifndef TAILQ_SWAP
#define TAILQ_SWAP(head1, head2, type, field) \
  do { \
    struct type *swap_first = (head1)->tqh_first; \
    struct type **swap_last = (head1)->tqh_last; \
    (head1)->tqh_first = (head2)->tqh_first; \
    (head1)->tqh_last = (head2)->tqh_last; \
    (head2)->tqh_first = swap_first; \
    (head2)->tqh_last = swap_last; \
    if ((swap_first = (head1)->tqh_first) != NULL) \
      swap_first->field.tqe_prev = &(head1)->tqh_first; \
    else \
      (head1)->tqh_last = &(head1)->tqh_first; \
    if ((swap_first = (head2)->tqh_first) != NULL) \
      swap_first->field.tqe_prev = &(head2)->tqh_first; \
    else \
      (head2)->tqh_last = &(head2)->tqh_first; \
  } while ((void)0, 0)
#endif

/**
 * @brief Coverage helper for bsd-sys-queue.
 * @return Always 0.
 */
int sys_queue_dummy_for_coverage(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_QUEUE_H_ */
/* clang-format on */

/* API Contract symbols: __include_next__ */
