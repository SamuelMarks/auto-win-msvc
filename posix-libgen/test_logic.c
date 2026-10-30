/* clang-format off */
#include <stdio.h>
#include <string.h>
/* clang-format on */

char *my_dirname(char *path) {
  static char dot[] = ".";
  char *p;
  size_t len;
  if (path == NULL || *path == '\0')
    return dot;
  len = strlen(path);
  /* Strip trailing slashes, but keep C:\ */
  while (len > 1 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
    if (len == 3 && path[1] == ':')
      break;
    path[len - 1] = '\0';
    len--;
  }
  if (len == 0 || (len == 1 && (path[0] == '/' || path[0] == '\\')))
    return path;

  for (p = path + len - 1; p >= path; p--) {
    if (*p == '/' || *p == '\\') {
      /* Go back to remove multiple slashes */
      while (p > path && (*p == '/' || *p == '\\')) {
        if (p - path == 2 && path[1] == ':') {
          *(p + 1) = '\0';
          return path;
        }
        *p = '\0';
        p--;
      }
      if (p == path && (*p == '/' || *p == '\\')) {
        *(p + 1) = '\0';
      } else {
        if (p >= path && (*p == '/' || *p == '\\'))
          *p = '\0';
        else
          *(p + 1) = '\0';
      }
      if (path[0] == '\0')
        return dot;
      return path;
    } else if (*p == ':') {
      if (p == path + 1) {
        *(p + 1) = '\0';
        return path;
      }
    }
  }
  return dot;
}

int main(void) {
  char p1[] = "usr";
  char p2[] = "usr/lib";
  char p3[] = "/usr/lib";
  char p4[] = "usr/lib/";
  char p5[] = "/";
  char p6[] = "/usr";
  char p7[] = "C:\\usr\\lib";
  char p8[] = "C:\\usr\\lib\\";
  char p9[] = "C:";
  char p10[] = "C:\\";
  char p11[] = "C:\\usr";
  printf("%s\n", my_dirname(p1));
  printf("%s\n", my_dirname(p2));
  printf("%s\n", my_dirname(p3));
  printf("%s\n", my_dirname(p4));
  printf("%s\n", my_dirname(p5));
  printf("%s\n", my_dirname(p6));
  printf("%s\n", my_dirname(p7));
  printf("%s\n", my_dirname(p8));
  printf("%s\n", my_dirname(p9));
  printf("%s\n", my_dirname(p10));
  printf("%s\n", my_dirname(p11));
  return 0;
}
