
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSD_MALLOC_NP_H
#define BSD_MALLOC_NP_H
#include "auto_win_msvc_export.h"

#if defined(_MSC_VER) && !defined(__clang__)
/** \brief je_malloc_stats_print function. */
AUTO_WIN_MSVC_EXPORT void AUTO_WIN_MSVC_EXPORT je_malloc_stats_print(
    void (*write_cb)(void *, const char *), void *cbopaque, const char *opts);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
