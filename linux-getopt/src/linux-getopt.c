/* clang-format off */
#include <linux-getopt.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER)

char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;

static char *nextchar = NULL;

/** \brief getopt function. */
int getopt(int argc, char *const argv[], const char *optstring) {
  char c;
  char *cp;

  if (optind == 0) {
    optind = 1;
    nextchar = NULL;
  }

  if (nextchar == NULL || *nextchar == '\0') {
    if (optind >= argc || argv[optind] == NULL || argv[optind][0] != '-' ||
        argv[optind][1] == '\0') {
      return -1;
    }
    if (strcmp(argv[optind], "--") == 0) {
      optind++;
      return -1;
    }
    nextchar = argv[optind] + 1;
  }

  c = *nextchar++;
  cp = strchr(optstring, c);

  if (cp == NULL || c == ':') {
    if (opterr && *optstring != ':') {
      fprintf(stderr, "illegal option -- %c\n", c);
    }
    optopt = c;
    if (*nextchar == '\0')
      optind++;
    return '?';
  }

  if (cp[1] == ':') {
    if (*nextchar != '\0') {
      optarg = nextchar;
      optind++;
    } else {
      if (optind + 1 >= argc) {
        if (opterr && *optstring != ':') {
          fprintf(stderr, "option requires an argument -- %c\n", c);
        }
        optopt = c;
        optind++;
        return (*optstring == ':') ? ':' : '?';
      }
      optarg = argv[++optind];
      optind++;
    }
    nextchar = NULL;
  } else {
    if (*nextchar == '\0') {
      optind++;
    }
    optarg = NULL;
  }

  return c;
}

/** \brief getopt_long function. */
int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex) {
  if (optind >= argc || argv[optind] == NULL || argv[optind][0] != '-') {
    return -1;
  }

  if (argv[optind][1] == '-' && argv[optind][2] != '\0') {
    const char *name = argv[optind] + 2;
    const char *has_eq = strchr(name, '=');
    size_t namelen = has_eq ? (size_t)(has_eq - name) : strlen(name);
    int i;

    for (i = 0; longopts[i].name != NULL; i++) {
      if (strncmp(name, longopts[i].name, namelen) == 0 &&
          longopts[i].name[namelen] == '\0') {
        if (longindex)
          *longindex = i;

        if (longopts[i].has_arg == required_argument) {
          if (has_eq) {
            optarg = (char *)(has_eq + 1);
          } else if (optind + 1 < argc) {
            optarg = argv[++optind];
          } else {
            if (opterr)
              fprintf(stderr, "option '--%s' requires an argument\n",
                      longopts[i].name);
            return '?';
          }
        } else if (longopts[i].has_arg == optional_argument) {
          if (has_eq) {
            optarg = (char *)(has_eq + 1);
          } else {
            optarg = NULL;
          }
        } else {
          if (has_eq) {
            if (opterr)
              fprintf(stderr, "option '--%s' doesn't allow an argument\n",
                      longopts[i].name);
            return '?';
          }
          optarg = NULL;
        }

        optind++;

        if (longopts[i].flag) {
          *longopts[i].flag = longopts[i].val;
          return 0;
        }
        return longopts[i].val;
      }
    }

    if (opterr)
      fprintf(stderr, "unrecognized option '%s'\n", argv[optind]);
    optopt = '?';
    optind++;
    return '?';
  }

  return getopt(argc, argv, optstring);
}

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_linux_getopt(void) { return 0; }

typedef int make_iso_compilers_happy_tu_linux_getopt;
