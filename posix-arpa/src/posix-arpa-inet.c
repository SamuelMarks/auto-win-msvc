#include "posix-arpa-inet.h"

#if defined(_MSC_VER) || defined(_WIN32)

/** \brief posix_inet_aton function. */
int posix_inet_aton(const char *cp, struct in_addr *inp) {
    unsigned long val = 0;
    int base = 10;
    char c;
    unsigned int parts[4];
    int pp = 0;

    if (!cp || !inp) return 0;

    for (;;) {
        val = 0;
        base = 10;
        if (*cp == '0') {
            if (*++cp == 'x' || *cp == 'X') {
                base = 16;
                cp++;
            } else {
                base = 8;
            }
        }
        while ((c = *cp) != '\0') {
            if (c >= '0' && c <= '9' && (c - '0') < base) {
                val = (val * base) + (c - '0');
                cp++;
            } else if (base == 16 && c >= 'a' && c <= 'f') {
                val = (val << 4) | (c - 'a' + 10);
                cp++;
            } else if (base == 16 && c >= 'A' && c <= 'F') {
                val = (val << 4) | (c - 'A' + 10);
                cp++;
            } else {
                break;
            }
        }
        if (*cp == '.') {
            if (pp >= 3) return 0;
            parts[pp++] = val;
            cp++;
        } else {
            break;
        }
    }

    if (*cp != '\0') return 0;

    if (pp == 0) {
        val = val;
    } else if (pp == 1) {
        if (val > 0xffffff || parts[0] > 0xff) return 0;
        val = (parts[0] << 24) | (val & 0xffffff);
    } else if (pp == 2) {
        if (val > 0xffff || parts[0] > 0xff || parts[1] > 0xff) return 0;
        val = (parts[0] << 24) | (parts[1] << 16) | (val & 0xffff);
    } else if (pp == 3) {
        if (val > 0xff || parts[0] > 0xff || parts[1] > 0xff || parts[2] > 0xff) return 0;
        val = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | (val & 0xff);
    }
    
    inp->s_addr = htonl(val);
    return 1;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_arpa_inet(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_arpa_inet;

