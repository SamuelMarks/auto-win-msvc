/* clang-format off */
#include "greatest.h"
#include <sys/backlight.h>
#include <sys/soundcard.h>
#include <dev/speaker/speaker.h>
#include <sys/mtio.h>
#include <sys/disk.h>
#include <sys/consio.h>
#include <sys/filio.h>
/* clang-format on */

extern int bsd_dev_headers_dummy_for_coverage(void);

TEST test_bsd_dev_headers_ioctl(void) {
  struct backlight_props bp;
  tone_t tn;
  struct mtop mo;
  bp.brightness = 50;
  tn.frequency = 440;
  tn.duration = 10;
  mo.mt_op = MTREW;
  mo.mt_count = 1;
  ASSERT_EQ(50U, bp.brightness);
  ASSERT_EQ(440, tn.frequency);
  ASSERT_EQ(MTREW, mo.mt_op);
  ASSERT_EQ(0, bsd_dev_headers_dummy_for_coverage());
  PASS();
}

SUITE(suite_bsd_dev_headers_core) { RUN_TEST(test_bsd_dev_headers_ioctl); }
