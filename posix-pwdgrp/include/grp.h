#ifndef POSIX_PWDGRP_GRP_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_PWDGRP_GRP_H

/**
 * @file grp.h
 * @brief POSIX grp.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <grp.h>
#else
#include <grp.h>
#endif
#else
#include "posix-pwdgrp.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_PWDGRP_GRP_H */
