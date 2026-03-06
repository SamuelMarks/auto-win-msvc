/* test.c - 100% Test Coverage Stubs */
#include <stdio.h>
#include "posix-termios.h"
#include "greatest.h"

SUITE(posix_termios_suite);

TEST test_cfgetispeed(void) {
    struct termios t;
    t.c_ispeed = B9600;
    ASSERT_EQ(B9600, cfgetispeed(&t));
    ASSERT_EQ(0, cfgetispeed(NULL));
    PASS();
}

TEST test_cfgetospeed(void) {
    struct termios t;
    t.c_ospeed = B9600;
    ASSERT_EQ(B9600, cfgetospeed(&t));
    ASSERT_EQ(0, cfgetospeed(NULL));
    PASS();
}

TEST test_cfsetispeed(void) {
    struct termios t;
    ASSERT_EQ(0, cfsetispeed(&t, B4800));
    ASSERT_EQ(B4800, t.c_ispeed);
    ASSERT_EQ(-1, cfsetispeed(NULL, B4800));
    PASS();
}

TEST test_cfsetospeed(void) {
    struct termios t;
    ASSERT_EQ(0, cfsetospeed(&t, B4800));
    ASSERT_EQ(B4800, t.c_ospeed);
    ASSERT_EQ(-1, cfsetospeed(NULL, B4800));
    PASS();
}

TEST test_tcdrain(void) {
    ASSERT_EQ(-1, tcdrain(-1));
    tcdrain(1);
    PASS();
}

TEST test_tcflow(void) {
    ASSERT_EQ(0, tcflow(1, TCOOFF));
    PASS();
}

TEST test_tcflush(void) {
    ASSERT_EQ(-1, tcflush(-1, TCIFLUSH));
    tcflush(1, TCIFLUSH);
    PASS();
}

TEST test_tcgetattr(void) {
    struct termios t;
    ASSERT_EQ(-1, tcgetattr(-1, &t));
    ASSERT_EQ(-1, tcgetattr(1, NULL));
    tcgetattr(1, &t);
    tcgetattr(0, &t);
    tcgetattr(2, &t);
    PASS();
}

TEST test_tcgetsid(void) {
    ASSERT_EQ(-1, tcgetsid(1));
    PASS();
}

TEST test_tcsendbreak(void) {
    ASSERT_EQ(0, tcsendbreak(1, 0));
    PASS();
}

TEST test_tcsetattr(void) {
    struct termios t;
    t.c_lflag = ECHO | ICANON | ISIG;
    t.c_oflag = OPOST;
    ASSERT_EQ(-1, tcsetattr(-1, TCSANOW, &t));
    ASSERT_EQ(-1, tcsetattr(1, TCSANOW, NULL));
    if (tcgetattr(1, &t) == 0) {
        tcsetattr(1, TCSANOW, &t);
    }
    if (tcgetattr(0, &t) == 0) {
        tcsetattr(0, TCSANOW, &t);
    }
    PASS();
}

SUITE(posix_termios_suite) {
    RUN_TEST(test_cfgetispeed);
    RUN_TEST(test_cfgetospeed);
    RUN_TEST(test_cfsetispeed);
    RUN_TEST(test_cfsetospeed);
    RUN_TEST(test_tcdrain);
    RUN_TEST(test_tcflow);
    RUN_TEST(test_tcflush);
    RUN_TEST(test_tcgetattr);
    RUN_TEST(test_tcgetsid);
    RUN_TEST(test_tcsendbreak);
    RUN_TEST(test_tcsetattr);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(posix_termios_suite);
    GREATEST_MAIN_END();
}

