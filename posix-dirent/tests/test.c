/* test.c - 100% Test Coverage */
#include "greatest.h"
#include "posix-dirent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define RMDIR(path) _rmdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0777)
#define RMDIR(path) rmdir(path)
#endif

GREATEST_MAIN_DEFS();

static void create_dummy_file(const char *path) {
    FILE *f = fopen(path, "w");
    if (f) {
        fputs("test", f);
        fclose(f);
    }
}

static void remove_dummy_file(const char *path) {
    remove(path);
}

TEST test_opendir_readdir_closedir(void) {
    DIR *dir;
    struct dirent *ent;
    int found_test1 = 0;
    int found_test2 = 0;

    MKDIR("testdir");
    create_dummy_file("testdir/test1.txt");
    create_dummy_file("testdir/test2.txt");

    dir = opendir("testdir");
    ASSERT(dir != NULL);

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, "test1.txt") == 0) found_test1 = 1;
        if (strcmp(ent->d_name, "test2.txt") == 0) found_test2 = 1;
    }

    ASSERT(found_test1 == 1);
    ASSERT(found_test2 == 1);

    ASSERT_EQ(0, closedir(dir));

    remove_dummy_file("testdir/test1.txt");
    remove_dummy_file("testdir/test2.txt");
    RMDIR("testdir");

    PASS();
}

TEST test_rewinddir_telldir_seekdir(void) {
    DIR *dir;
    struct dirent *ent;
    long loc0, loc1, loc2;
    char first_file[260] = {0};
    char second_file[260] = {0};
    
    MKDIR("testdir2");
    create_dummy_file("testdir2/a.txt");
    create_dummy_file("testdir2/b.txt");

    dir = opendir("testdir2");
    ASSERT(dir != NULL);

    loc0 = telldir(dir);
    ent = readdir(dir);
    ASSERT(ent != NULL);
    strcpy(first_file, ent->d_name);
    loc1 = telldir(dir);

    ent = readdir(dir);
    ASSERT(ent != NULL);
    strcpy(second_file, ent->d_name);
    loc2 = telldir(dir);
    (void)loc2;

    rewinddir(dir);
    ent = readdir(dir);
    ASSERT(ent != NULL);
    ASSERT_STR_EQ(first_file, ent->d_name);

    seekdir(dir, loc0);
    ent = readdir(dir);
    ASSERT(ent != NULL);
    
    seekdir(dir, loc1);
    ent = readdir(dir);
    ASSERT(ent != NULL);

    closedir(dir);

    remove_dummy_file("testdir2/a.txt");
    remove_dummy_file("testdir2/b.txt");
    RMDIR("testdir2");

    PASS();
}

static int filter_txt_files(const struct dirent *ent) {
    return strstr(ent->d_name, ".txt") != NULL;
}

TEST test_scandir_alphasort(void) {
    struct dirent **namelist;
    int n;

    MKDIR("testdir3");
    create_dummy_file("testdir3/c.txt");
    create_dummy_file("testdir3/a.txt");
    create_dummy_file("testdir3/b.txt");

    n = scandir("testdir3", &namelist, filter_txt_files, alphasort);
    ASSERT_EQ(3, n);

    /* Verify they are sorted alphabetically */
    if (n >= 2) {
        int i;
        for (i = 0; i < n - 1; i++) {
            ASSERT(strcmp(namelist[i]->d_name, namelist[i+1]->d_name) <= 0);
        }
    }

    while (n--) {
        free(namelist[n]);
    }
    free(namelist);

    remove_dummy_file("testdir3/c.txt");
    remove_dummy_file("testdir3/a.txt");
    remove_dummy_file("testdir3/b.txt");
    RMDIR("testdir3");

    PASS();
}

TEST test_error_handling(void) {
    DIR *dir = opendir("nonexistent_directory_12345");
    ASSERT(dir == NULL);

    PASS();
}

SUITE(dirent_suite) {
    RUN_TEST(test_opendir_readdir_closedir);
    RUN_TEST(test_rewinddir_telldir_seekdir);
    RUN_TEST(test_scandir_alphasort);
    RUN_TEST(test_error_handling);
}

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(dirent_suite);
    GREATEST_MAIN_END();
}
