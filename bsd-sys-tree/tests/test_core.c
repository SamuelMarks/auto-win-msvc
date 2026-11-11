/* clang-format off */
#include "greatest.h"
#include "sys/tree.h"
/* clang-format on */

struct t_node {
  int key;
  RB_ENTRY(t_node) rb_entry;
  SPLAY_ENTRY(t_node) sp_entry;
};

static int cmp_t_node(struct t_node *a, struct t_node *b) {
  return (a->key > b->key) - (a->key < b->key);
}

RB_HEAD(rb_tree_t, t_node);
RB_PROTOTYPE(rb_tree_t, t_node, rb_entry, cmp_t_node)
RB_GENERATE(rb_tree_t, t_node, rb_entry, cmp_t_node)

SPLAY_HEAD(sp_tree_t, t_node);
SPLAY_PROTOTYPE(sp_tree_t, t_node, sp_entry, cmp_t_node)
SPLAY_GENERATE(sp_tree_t, t_node, sp_entry, cmp_t_node)

TEST test_sys_tree_dummy(void) {
  struct rb_tree_t rbt = RB_INITIALIZER(&rbt);
  struct sp_tree_t spt = SPLAY_INITIALIZER(&spt);
  struct t_node n1, n2, n3, q, *res, *tmp;

  n1.key = 10;
  n2.key = 20;
  n3.key = 5;

  RB_INIT(&rbt);
  ASSERT(RB_INSERT(rb_tree_t, &rbt, &n1) == NULL);
  ASSERT(RB_INSERT(rb_tree_t, &rbt, &n2) == NULL);
  ASSERT(RB_INSERT(rb_tree_t, &rbt, &n3) == NULL);

  q.key = 10;
  res = RB_FIND(rb_tree_t, &rbt, &q);
  ASSERT_EQ(&n1, res);
  ASSERT_EQ(&n3, RB_MIN(rb_tree_t, &rbt));
  ASSERT_EQ(&n2, RB_MAX(rb_tree_t, &rbt));

  RB_FOREACH_SAFE(res, rb_tree_t, &rbt, tmp) {
    RB_REMOVE(rb_tree_t, &rbt, res);
  }
  ASSERT(RB_EMPTY(&rbt));

  SPLAY_INIT(&spt);
  ASSERT(SPLAY_INSERT(sp_tree_t, &spt, &n1) == NULL);
  ASSERT(SPLAY_INSERT(sp_tree_t, &spt, &n2) == NULL);
  res = SPLAY_FIND(sp_tree_t, &spt, &q);
  ASSERT_EQ(&n1, res);
  SPLAY_REMOVE(sp_tree_t, &spt, &n1);
  SPLAY_REMOVE(sp_tree_t, &spt, &n2);
  ASSERT(SPLAY_EMPTY(&spt));

  ASSERT_EQ(0, sys_tree_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_sys_tree_core) { RUN_TEST(test_sys_tree_dummy); }
