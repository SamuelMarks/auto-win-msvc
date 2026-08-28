#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

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
}
