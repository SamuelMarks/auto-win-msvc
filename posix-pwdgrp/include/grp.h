#ifndef POSIX_PWDGRP_GRP_H
#if defined(__GNUC__)
#endif
#define POSIX_PWDGRP_GRP_H

/**
 * @file grp.h
 * @brief POSIX grp.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <grp.h>
#else
#include <grp.h>
#endif
#endif
#include "posix-pwdgrp.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_PWDGRP_GRP_H */
