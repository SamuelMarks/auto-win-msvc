/* test.c - 100% Test Coverage Stubs */
/* clang-format off */
#include "greatest.h"
#include "unwind.h"
/* clang-format on */

struct trace_state {
  int count;
  size_t ips[10];
};

static _Unwind_Reason_Code trace_cb(struct _Unwind_Context *ctx, void *arg) {
  struct trace_state *state = (struct trace_state *)arg;
  if (state->count < 10) {
    state->ips[state->count++] = _Unwind_GetIP(ctx);
    return _URC_NO_REASON;
  }
  return _URC_NORMAL_STOP;
}

TEST test_unwind_backtrace(void) {
  struct trace_state state;
  _Unwind_Reason_Code code;

  state.count = 0;

  code = _Unwind_Backtrace(trace_cb, &state);

  ASSERT(code == _URC_END_OF_STACK || code == _URC_NORMAL_STOP);
  ASSERT(state.count > 0);

  /* Test error cases */
#if defined(_MSC_VER)
  code = _Unwind_Backtrace(NULL, &state);
  ASSERT_EQ(_URC_FATAL_PHASE1_ERROR, code);

  /* Test get IP with null context */
  ASSERT_EQ(0, _Unwind_GetIP(NULL));
#endif

  PASS();
}

SUITE(unwind_suite) { RUN_TEST(test_unwind_backtrace); }

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(unwind_suite);
  GREATEST_MAIN_END();
}
