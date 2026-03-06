/* test.c - 100% Test Coverage */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greatest.h"
#include "posix-glob.h"

/* Greatest setup */
GREATEST_MAIN_DEFS();

TEST test_fnmatch_basic(void) {
    ASSERT_EQ(0, fnmatch("*.c", "test.c", 0));
    ASSERT_EQ(FNM_NOMATCH, fnmatch("*.h", "test.c", 0));
    ASSERT_EQ(0, fnmatch("test.?", "test.c", 0));
    ASSERT_EQ(0, fnmatch("t*t.c", "test.c", 0));
    ASSERT_EQ(FNM_NOMATCH, fnmatch(NULL, "test.c", 0));
    ASSERT_EQ(FNM_NOMATCH, fnmatch("*.c", NULL, 0));
    PASS();
}

TEST test_fnmatch_flags(void) {
    ASSERT_EQ(0, fnmatch("src/*.c", "src/test.c", FNM_PATHNAME));
    ASSERT_EQ(FNM_NOMATCH, fnmatch("src/*.c", "src/dir/test.c", FNM_PATHNAME));
    
    ASSERT_EQ(FNM_NOMATCH, fnmatch("*.c", ".test.c", FNM_PERIOD));
    ASSERT_EQ(0, fnmatch(".*.c", ".test.c", FNM_PERIOD));

    ASSERT_EQ(0, fnmatch("test\\*.c", "test\\*.c", FNM_NOESCAPE));
    PASS();
}

TEST test_glob_basic(void) {
    glob_t g;
    int ret;
    FILE *f = fopen("test_dummy.txt", "w");
    if (f) {
        fputs("dummy", f);
        fclose(f);
    }

    ret = glob("test_dummy.*", 0, NULL, &g);
    ASSERT_EQ(0, ret);
    ASSERT(g.gl_pathc > 0);
    ASSERT(strstr(g.gl_pathv[0], "test_dummy.txt") != NULL);
    
    globfree(&g);
    remove("test_dummy.txt");
    PASS();
}

TEST test_glob_nocheck(void) {
    glob_t g;
    int ret;
    ret = glob("nonexistent_file_12345.xyz", GLOB_NOCHECK, NULL, &g);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(1, g.gl_pathc);
    ASSERT_STR_EQ("nonexistent_file_12345.xyz", g.gl_pathv[0]);
    globfree(&g);
    PASS();
}

TEST test_glob_nomatch(void) {
    glob_t g;
    int ret;
    ret = glob("nonexistent_file_12345.xyz", 0, NULL, &g);
    ASSERT_EQ(GLOB_NOMATCH, ret);
    PASS();
}

TEST test_glob_append(void) {
    glob_t g;
    int ret;
    FILE *f1 = fopen("test_dummy1.txt", "w");
    FILE *f2 = fopen("test_dummy2.txt", "w");
    if (f1) { fputs("dummy", f1); fclose(f1); }
    if (f2) { fputs("dummy", f2); fclose(f2); }

    ret = glob("test_dummy1.txt", 0, NULL, &g);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(1, g.gl_pathc);

    ret = glob("test_dummy2.txt", GLOB_APPEND, NULL, &g);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(2, g.gl_pathc);
    
    globfree(&g);
    remove("test_dummy1.txt");
    remove("test_dummy2.txt");
    PASS();
}

TEST test_glob_dooffs(void) {
    glob_t g;
    int ret;
    FILE *f;
    g.gl_offs = 2;
    f = fopen("test_dummy_offs.txt", "w");
    if (f) { fputs("dummy", f); fclose(f); }

    ret = glob("test_dummy_offs.txt", GLOB_DOOFFS, NULL, &g);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(1, g.gl_pathc);
    ASSERT_EQ(NULL, g.gl_pathv[0]);
    ASSERT_EQ(NULL, g.gl_pathv[1]);
    ASSERT(g.gl_pathv[2] != NULL);
    
    globfree(&g);
    remove("test_dummy_offs.txt");
    PASS();
}

TEST test_wordexp_basic(void) {
    wordexp_t w;
    int ret;
    ret = wordexp("hello world", &w, 0);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(2, w.we_wordc);
    ASSERT_STR_EQ("hello", w.we_wordv[0]);
    ASSERT_STR_EQ("world", w.we_wordv[1]);
    wordfree(&w);
    PASS();
}

TEST test_wordexp_append(void) {
    wordexp_t w;
    int ret;
    ret = wordexp("hello", &w, 0);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(1, w.we_wordc);

    ret = wordexp("world", &w, WRDE_APPEND);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(2, w.we_wordc);
    ASSERT_STR_EQ("hello", w.we_wordv[0]);
    ASSERT_STR_EQ("world", w.we_wordv[1]);
    
    wordfree(&w);
    PASS();
}

TEST test_wordexp_dooffs(void) {
    wordexp_t w;
    int ret;
    w.we_offs = 1;
    ret = wordexp("hello", &w, WRDE_DOOFFS);
    ASSERT_EQ(0, ret);
    ASSERT_EQ(1, w.we_wordc);
    ASSERT_EQ(NULL, w.we_wordv[0]);
    ASSERT_STR_EQ("hello", w.we_wordv[1]);
    wordfree(&w);
    PASS();
}

SUITE(fnmatch_suite) {
    RUN_TEST(test_fnmatch_basic);
    RUN_TEST(test_fnmatch_flags);
}

SUITE(glob_suite) {
    RUN_TEST(test_glob_basic);
    RUN_TEST(test_glob_nocheck);
    RUN_TEST(test_glob_nomatch);
    RUN_TEST(test_glob_append);
    RUN_TEST(test_glob_dooffs);
}

SUITE(wordexp_suite) {
    RUN_TEST(test_wordexp_basic);
    RUN_TEST(test_wordexp_append);
    RUN_TEST(test_wordexp_dooffs);
}

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(fnmatch_suite);
    RUN_SUITE(glob_suite);
    RUN_SUITE(wordexp_suite);
    GREATEST_MAIN_END();
}
