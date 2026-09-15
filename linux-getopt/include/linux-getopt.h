#ifndef LINUX_GETOPT_H
#define LINUX_GETOPT_H

/**
 * @file linux-getopt.h
 * @brief Polyfill for GNU getopt and getopt_long command-line parser.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-getopt functions.
 */
enum linux_getopt_error_code {
  /** @brief Successful operation. */
  LINUX_GETOPT_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_GETOPT_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  LINUX_GETOPT_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and resets the linux-getopt module state.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_GETOPT_SUCCESS on success, or an error code on failure.
 */
enum linux_getopt_error_code linux_getopt_init(int *out_status);

#ifndef no_argument
/** @brief Option does not take an argument. */
#define no_argument 0
#endif
#ifndef required_argument
/** @brief Option requires an argument. */
#define required_argument 1
#endif
#ifndef optional_argument
/** @brief Option takes an optional argument. */
#define optional_argument 2
#endif

/**
 * @brief Structure describing a long option.
 */
struct option {
  /** @brief Name of long option. */
  const char *name;
  /** @brief One of no_argument, required_argument, optional_argument. */
  int has_arg;
  /** @brief Pointer to flag variable, or NULL. */
  int *flag;
  /** @brief Value to return or store in flag. */
  int val;
};

#if defined(_WIN32) || defined(_MSC_VER)
/** @brief Current option argument pointer. */
extern char *optarg;
/** @brief Current argv index. */
extern int optind;
/** @brief Non-zero to print error messages. */
extern int opterr;
/** @brief Unrecognized or missing option character. */
extern int optopt;

/**
 * @brief Parses command-line arguments.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param optstring Option characters string.
 * @return Option character, or -1 when done, or '?' on error.
 */
int getopt(int argc, char *const argv[], const char *optstring);

/**
 * @brief Parses long command-line options.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param optstring Short option string.
 * @param longopts Array of long option structures.
 * @param[out] longindex Pointer to index of matched long option.
 * @return Option value, or -1 when done, or '?' on error.
 */
int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex);
#endif

/** @brief Global optarg for linux_getopt. */
extern char *linux_optarg;
/** @brief Global optind for linux_getopt. */
extern int linux_optind;
/** @brief Global opterr for linux_getopt. */
extern int linux_opterr;
/** @brief Global optopt for linux_getopt. */
extern int linux_optopt;

/**
 * @brief Gets the current linux_optarg value.
 * @return Current optarg string.
 */
const char *linux_get_optarg(void);

/**
 * @brief Gets the current linux_optind value.
 * @return Current optind index.
 */
int linux_get_optind(void);

/**
 * @brief Gets the current linux_opterr value.
 * @return Current opterr flag.
 */
int linux_get_opterr(void);

/**
 * @brief Gets the current linux_optopt value.
 * @return Current optopt character.
 */
int linux_get_optopt(void);

/**
 * @brief Portable implementation of getopt.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param optstring Option string.
 * @return Option character or -1 when done.
 */
int linux_getopt(int argc, char *const argv[], const char *optstring);

/**
 * @brief Portable implementation of getopt_long.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param optstring Option string.
 * @param longopts Array of long options.
 * @param[out] longindex Matched long option index.
 * @return Option value or -1 when done.
 */
int linux_getopt_long(int argc, char *const argv[], const char *optstring,
                      const struct option *longopts, int *longindex);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_GETOPT_H */
