#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

SUITE_EXTERN(suite_solaris_port_core);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_solaris_port_core);
  GREATEST_MAIN_END();
  return 0;
}
