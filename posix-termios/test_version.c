#include <stdio.h>
int main() {
#if _MSC_VER >= 1900
  printf(">=1900\n");
#else
  printf("<1900\n");
#endif
  return 0;
}
