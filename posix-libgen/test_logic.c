#include <stdio.h>
#include <string.h>

char *my_dirname(char *path) {
  char *p;
  size_t len;
  if (path == NULL || *path == '\0')
    return ".";
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
        return ".";
      return path;
    } else if (*p == ':') {
      if (p == path + 1) {
        *(p + 1) = '\0';
        return path;
      }
    }
  }
  return ".";
}

int main() {
  char p1[] = "usr";
  printf("%s\n", my_dirname(p1));
  char p2[] = "usr/lib";
  printf("%s\n", my_dirname(p2));
  char p3[] = "/usr/lib";
  printf("%s\n", my_dirname(p3));
  char p4[] = "usr/lib/";
  printf("%s\n", my_dirname(p4));
  char p5[] = "/";
  printf("%s\n", my_dirname(p5));
  char p6[] = "/usr";
  printf("%s\n", my_dirname(p6));
  char p7[] = "C:\\usr\\lib";
  printf("%s\n", my_dirname(p7));
  char p8[] = "C:\\usr\\lib\\";
  printf("%s\n", my_dirname(p8));
  char p9[] = "C:";
  printf("%s\n", my_dirname(p9));
  char p10[] = "C:\\";
  printf("%s\n", my_dirname(p10));
  char p11[] = "C:\\usr";
  printf("%s\n", my_dirname(p11));
  return 0;
}
