/* clang-format off */
#include "greatest.h"
#include "sys/queue.h"
/* clang-format on */

struct q_node {
  int val;
  SLIST_ENTRY(q_node) sl_entry;
  STAILQ_ENTRY(q_node) stq_entry;
  LIST_ENTRY(q_node) l_entry;
  TAILQ_ENTRY(q_node) tq_entry;
};

SLIST_HEAD(slist_head_t, q_node);
STAILQ_HEAD(stailq_head_t, q_node);
LIST_HEAD(list_head_t, q_node);
TAILQ_HEAD(tailq_head_t, q_node);

TEST test_sys_queue_dummy(void) {
  struct slist_head_t slh;
  struct stailq_head_t stqh;
  struct list_head_t lh;
  struct tailq_head_t tqh;
  struct q_node n1, n2, *np, *tmp;

  n1.val = 1;
  n2.val = 2;

  SLIST_INIT(&slh);
  SLIST_INSERT_HEAD(&slh, &n1, sl_entry);
  SLIST_INSERT_AFTER(&n1, &n2, sl_entry);
  SLIST_FOREACH_SAFE(np, &slh, sl_entry, tmp) { ASSERT(np->val > 0); }

  STAILQ_INIT(&stqh);
  STAILQ_INSERT_HEAD(&stqh, &n1, stq_entry);
  STAILQ_INSERT_TAIL(&stqh, &n2, stq_entry);
  ASSERT_EQ(&n2, STAILQ_LAST(&stqh, q_node, stq_entry));

  LIST_INIT(&lh);
  LIST_INSERT_HEAD(&lh, &n1, l_entry);
  LIST_INSERT_AFTER(&n1, &n2, l_entry);
  ASSERT_EQ(&n1, LIST_PREV(&n2, &lh, q_node, l_entry));

  TAILQ_INIT(&tqh);
  TAILQ_INSERT_HEAD(&tqh, &n1, tq_entry);
  TAILQ_INSERT_TAIL(&tqh, &n2, tq_entry);
  TAILQ_FOREACH_REVERSE_SAFE(np, &tqh, tailq_head_t, tq_entry, tmp) {
    ASSERT(np->val > 0);
  }

  ASSERT_EQ(0, sys_queue_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_sys_queue_core) { RUN_TEST(test_sys_queue_dummy); }
