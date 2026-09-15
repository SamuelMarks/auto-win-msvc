#ifndef POSIX_PWDGRP_PWD_H
#if defined(__GNUC__)
#pragma GCC system_header
#endif
#define POSIX_PWDGRP_PWD_H

/**
 * @file pwd.h
 * @brief POSIX pwd.h redirection header.
 */

/* clang-format off */
#if !defined(_WIN32)
#if defined(__GNUC__) || defined(__clang__)
#include_next <pwd.h>
#else
#include <pwd.h>
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

#endif /* POSIX_PWDGRP_PWD_H */
