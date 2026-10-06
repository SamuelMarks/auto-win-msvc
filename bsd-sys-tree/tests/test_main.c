#include "greatest.h"

SUITE_EXTERN(suite_bsd_sys_tree_core);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_bsd_sys_tree_core);
  GREATEST_MAIN_END();
}
