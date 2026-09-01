
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSD_MALLOC_NP_H
#define BSD_MALLOC_NP_H

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

#if defined(_MSC_VER) && !defined(__clang__)
/** \brief je_malloc_stats_print function. */
error_type_t je_malloc_stats_print(void (*write_cb)(void *, const char *),
                                   void *cbopaque, const char *opts);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
