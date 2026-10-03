#ifndef GETOPT_H_
#define GETOPT_H_

/**
 * @file getopt.h
 * @brief Forwarding header to linux-getopt.h.
 */

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#include "linux-getopt.h"
#else
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <getopt.h>
#else
#include <getopt.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GETOPT_H_ */
