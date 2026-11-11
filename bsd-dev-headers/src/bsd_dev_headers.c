/**
 * @file bsd_dev_headers.c
 * @brief Compilation unit for bsd-dev-headers.
 */

/* clang-format off */
#include <sys/ioccom.h>
#include <sys/filio.h>
#include <sys/ttycom.h>
#include <sys/tty.h>
#include <sys/consio.h>
#include <sys/disk.h>
#include <sys/backlight.h>
#include <sys/soundcard.h>
#include <dev/speaker/speaker.h>
#include <sys/mtio.h>
/* clang-format on */

int bsd_dev_headers_dummy_for_coverage(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_dev_headers;
