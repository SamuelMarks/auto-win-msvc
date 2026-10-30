/* clang-format off */
#include <stdio.h>
/* clang-format on */
int main(void) {
#if defined(_MSC_VER) && _MSC_VER >= 1900
  printf(">=1900\n");
#else
  printf("<1900\n");
#endif
  return 0;
}
