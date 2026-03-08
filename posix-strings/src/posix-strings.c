#include <posix-strings.h>

#if defined(_MSC_VER)

#include <intrin.h>

#pragma intrinsic(_BitScanForward)

#if defined(_WIN64)
#pragma intrinsic(_BitScanForward64)
#endif

/** \brief ffs function. */
int ffs(int i) {
    unsigned long index;
    if (i == 0) return 0;
    if (_BitScanForward(&index, (unsigned long)i)) {
        return (int)index + 1;
    }
    return 0;
}

/** \brief ffsl function. */
int ffsl(long i) {
    unsigned long index;
    if (i == 0) return 0;
    if (_BitScanForward(&index, (unsigned long)i)) {
        return (int)index + 1;
    }
    return 0;
}

/** \brief ffsll function. */
int ffsll(long long i) {
    unsigned long index;
    if (i == 0) return 0;
#if defined(_WIN64)
    if (_BitScanForward64(&index, (unsigned __int64)i)) {
        return (int)index + 1;
    }
#else
    if ((unsigned long)i != 0) {
        if (_BitScanForward(&index, (unsigned long)i)) {
            return (int)index + 1;
        }
    } else {
        if (_BitScanForward(&index, (unsigned long)(i >> 32))) {
            return (int)index + 33;
        }
    }
#endif
    return 0;
}

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_posix_strings(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_strings;

