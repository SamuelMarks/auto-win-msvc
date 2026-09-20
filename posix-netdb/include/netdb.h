#ifndef POSIX_NETDB_NETDB_H
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#define POSIX_NETDB_NETDB_H

/**
 * @file netdb.h
 * @brief POSIX netdb.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <netdb.h>
#else
#include <netdb.h>
#endif
#else
#include "posix-netdb.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EAI_SYSTEM
/** \brief System error returned in errno. */
#define EAI_SYSTEM 11
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_NETDB_NETDB_H */
