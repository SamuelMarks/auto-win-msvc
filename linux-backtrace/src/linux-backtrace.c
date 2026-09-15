/* clang-format off */
#include "linux-backtrace.h"
#include <stddef.h>
/* clang-format on */

struct backtrace_state {
  int initialized;
};

static struct backtrace_state g_default_state = {1};

/** @brief Creates a backtrace state. */
struct backtrace_state *
backtrace_create_state(const char *filename, int threaded,
                       backtrace_error_callback error_callback, void *data) {
  (void)filename;
  (void)threaded;
  (void)error_callback;
  (void)data;
  return &g_default_state;
}

/** @brief Retrieves symbol and file information for a program counter. */
int backtrace_pcinfo(struct backtrace_state *state, uintptr_t pc,
                     backtrace_full_callback callback,
                     backtrace_error_callback error_callback, void *data) {
  (void)state;
  (void)pc;
  (void)error_callback;
  if (callback != NULL) {
    return callback(data, pc, "unknown", 0, "unknown");
  }
  return 0;
}

/** @brief Initializes and validates the linux-backtrace module. */
enum linux_backtrace_error_code linux_backtrace_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_BACKTRACE_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_BACKTRACE_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_backtrace;
