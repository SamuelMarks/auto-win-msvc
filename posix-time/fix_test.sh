sed -i '' 's/FILE \*f = fopen(test_file, "w");/FILE *f = NULL;\n#if defined(_MSC_VER)\n    fopen_s(\&f, test_file, "w");\n#else\n    f = fopen(test_file, "w");\n#endif/g' tests/test.c
