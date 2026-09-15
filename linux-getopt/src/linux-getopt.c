/* clang-format off */
#include "linux-getopt.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

char *linux_optarg = NULL;
int linux_optind = 1;
int linux_opterr = 1;
int linux_optopt = 0;

static char *nextchar = NULL;

#if defined(_WIN32) || defined(_MSC_VER)
char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;
#endif

/** @brief Portable implementation of getopt. */
int linux_getopt(int argc, char *const argv[], const char *optstring) {
  char c;
  char *cp;

  if (linux_optind == 0) {
    linux_optind = 1;
    nextchar = NULL;
  }

  if (nextchar == NULL || *nextchar == '\0') {
    if (linux_optind >= argc || argv[linux_optind] == NULL ||
        argv[linux_optind][0] != '-' || argv[linux_optind][1] == '\0') {
      return -1;
    }
    if (strcmp(argv[linux_optind], "--") == 0) {
      linux_optind++;
      return -1;
    }
    nextchar = argv[linux_optind] + 1;
  }

  c = *nextchar++;
  cp = strchr(optstring, c);

  if (cp == NULL || c == ':') {
    if (linux_opterr && *optstring != ':') {
      fprintf(stderr, "illegal option -- %c\n", c);
    }
    linux_optopt = c;
    if (*nextchar == '\0')
      linux_optind++;
    return '?';
  }

  if (cp[1] == ':') {
    if (*nextchar != '\0') {
      linux_optarg = nextchar;
      linux_optind++;
    } else {
      if (linux_optind + 1 >= argc) {
        if (linux_opterr && *optstring != ':') {
          fprintf(stderr, "option requires an argument -- %c\n", c);
        }
        linux_optopt = c;
        linux_optind++;
        return (*optstring == ':') ? ':' : '?';
      }
      linux_optarg = argv[++linux_optind];
      linux_optind++;
    }
    nextchar = NULL;
  } else {
    if (*nextchar == '\0') {
      linux_optind++;
    }
    linux_optarg = NULL;
  }

  return c;
}

/** @brief Portable implementation of getopt_long. */
int linux_getopt_long(int argc, char *const argv[], const char *optstring,
                      const struct option *longopts, int *longindex) {
  if (linux_optind >= argc || argv[linux_optind] == NULL ||
      argv[linux_optind][0] != '-') {
    return -1;
  }

  if (argv[linux_optind][1] == '-' && argv[linux_optind][2] != '\0') {
    char *name = argv[linux_optind] + 2;
    char *has_eq = strchr(name, '=');
    size_t namelen = has_eq ? (size_t)(has_eq - name) : strlen(name);
    int i;

    for (i = 0; longopts[i].name != NULL; i++) {
      if (strncmp(name, longopts[i].name, namelen) == 0 &&
          longopts[i].name[namelen] == '\0') {
        if (longindex)
          *longindex = i;

        if (longopts[i].has_arg == required_argument) {
          if (has_eq) {
            linux_optarg = has_eq + 1;
          } else if (linux_optind + 1 < argc) {
            linux_optarg = argv[++linux_optind];
          } else {
            if (linux_opterr)
              fprintf(stderr, "option '--%s' requires an argument\n",
                      longopts[i].name);
            return '?';
          }
        } else if (longopts[i].has_arg == optional_argument) {
          if (has_eq) {
            linux_optarg = has_eq + 1;
          } else {
            linux_optarg = NULL;
          }
        } else {
          if (has_eq) {
            if (linux_opterr)
              fprintf(stderr, "option '--%s' doesn't allow an argument\n",
                      longopts[i].name);
            return '?';
          }
          linux_optarg = NULL;
        }

        linux_optind++;

        if (longopts[i].flag) {
          *longopts[i].flag = longopts[i].val;
          return 0;
        }
        return longopts[i].val;
      }
    }

    if (linux_opterr)
      fprintf(stderr, "unrecognized option '%s'\n", argv[linux_optind]);
    linux_optopt = '?';
    linux_optind++;
    return '?';
  }

  return linux_getopt(argc, argv, optstring);
}

#if defined(_WIN32) || defined(_MSC_VER)
int getopt(int argc, char *const argv[], const char *optstring) {
  int res;
  res = linux_getopt(argc, argv, optstring);
  optarg = linux_optarg;
  optind = linux_optind;
  opterr = linux_opterr;
  optopt = linux_optopt;
  return res;
}

int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex) {
  int res;
  res = linux_getopt_long(argc, argv, optstring, longopts, longindex);
  optarg = linux_optarg;
  optind = linux_optind;
  opterr = linux_opterr;
  optopt = linux_optopt;
  return res;
}
#endif

const char *linux_get_optarg(void) { return linux_optarg; }
int linux_get_optind(void) { return linux_optind; }
int linux_get_opterr(void) { return linux_opterr; }
int linux_get_optopt(void) { return linux_optopt; }

/** @brief Initializes and resets the linux-getopt module state. */
enum linux_getopt_error_code linux_getopt_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_GETOPT_ERROR_NULL_POINTER;
  }
  linux_optarg = NULL;
  linux_optind = 1;
  linux_opterr = 0;
  linux_optopt = 0;
  nextchar = NULL;
#if defined(_WIN32) || defined(_MSC_VER)
  optarg = NULL;
  optind = 1;
  opterr = 0;
  optopt = 0;
#endif
  *out_status = 1;
  return LINUX_GETOPT_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_getopt;
