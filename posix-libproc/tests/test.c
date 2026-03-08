#include "greatest.h"

TEST dummy_test(void) {
    PASS();
}

SUITE(suite) {
    RUN_TEST(dummy_test);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(suite);
    GREATEST_MAIN_END();
}
