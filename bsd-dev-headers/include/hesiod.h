#ifndef BSD_HESIOD_H
#define BSD_HESIOD_H

/**
 * @file hesiod.h
 * @brief Hesiod name service (<hesiod.h>) compatibility interface.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define HES_ER_UNINIT (-1)
#define HES_ER_OK 0
#define HES_ER_NOTFOUND 1
#define HES_ER_CONFIG 2
#define HES_ER_NET 3

/** @brief Initialize a Hesiod library context. */
int hesiod_init(void **context);
/** @brief Free resources associated with a Hesiod context. */
void hesiod_end(void *context);
/** @brief Convert Hesiod name and type into a DNS domain name. */
char *hesiod_to_bind(void *context, const char *hesiod_name,
                     const char *hesiod_type);
/** @brief Resolve a Hesiod name and type into a NUL-terminated string array. */
char **hesiod_resolve(void *context, const char *hesiod_name,
                      const char *hesiod_type);
/** @brief Free a string array returned by hesiod_resolve. */
void hesiod_free_list(void *context, char **list);

/** @brief Initialize the global legacy Hesiod context. */
int hes_init(void);
/** @brief Convert Hesiod name and type into a DNS name using global state. */
char *hes_to_bind(const char *hesiod_name, const char *hesiod_type);
/** @brief Resolve Hesiod name and type using global state. */
char **hes_resolve(const char *hesiod_name, const char *hesiod_type);
/** @brief Return last error code from global Hesiod operations. */
int hes_error(void);
/** @brief Free a string array returned by hes_resolve. */
void hes_free(char **hp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_HESIOD_H */
