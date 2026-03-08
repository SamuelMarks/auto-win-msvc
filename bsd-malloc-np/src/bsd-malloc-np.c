#include "bsd-malloc-np.h"
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief je_malloc_stats_print function. */
void je_malloc_stats_print(void (*write_cb)(void *, const char *), void *cbopaque, const char *opts) {
    (void)cbopaque; (void)opts;
    if (write_cb) write_cb(cbopaque, "je_malloc_stats_print stub\n");
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_malloc_np(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_malloc_np;

