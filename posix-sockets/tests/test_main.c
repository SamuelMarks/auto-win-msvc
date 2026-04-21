#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"

SUITE_EXTERN(suite_posix_sockets_core);
SUITE_EXTERN(suite_posix_sockets_conn);
SUITE_EXTERN(suite_posix_sockets_sockopt);
SUITE_EXTERN(suite_posix_sockets_io);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_sockets_core);
  RUN_SUITE(suite_posix_sockets_conn);
  RUN_SUITE(suite_posix_sockets_sockopt);
  RUN_SUITE(suite_posix_sockets_io);
  GREATEST_MAIN_END();
  return 0;
}
