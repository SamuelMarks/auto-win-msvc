/* clang-format off */
#ifndef GETOPT_H_
#define GETOPT_H_

#if defined(__GNUC__) || defined(__clang__)
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The getopt() function is a command-line parser that shall follow  Utility Syntax Guidelines 3, 4, 5, 6, 7, 9, and 10 in the Base  Definitions volume of POSIX.1‐2017, Section 12.2, Utility Syntax  Guidelines.
 */
int getopt(int argc, char * const argv[], const char *optstring);

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_H_ */
/* clang-format on */
