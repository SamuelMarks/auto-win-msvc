/* clang-format off */
#include "greatest.h"
#include <sys/backlight.h>
#include <sys/soundcard.h>
#include <dev/speaker/speaker.h>
#include <dev/usb/usbhid.h>
#include <sys/mtio.h>
#include <sys/disk.h>
#include <sys/consio.h>
#include <sys/filio.h>
#include <sys/sockio.h>
#include <sys/fnv_hash.h>
#include <sys/mac.h>
#include <sys/linker.h>
#include <xlocale.h>
#include <ar.h>
#include <elf.h>
#include <cam/cam.h>
#include <usbhid.h>
#include <sysdecode.h>
#include <netipsec/ipsec.h>
#include <hesiod.h>
#include <mqueue.h>
#include <semaphore.h>
#include <bluetooth.h>
#include <sdp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern int bsd_dev_headers_dummy_for_coverage(void);
extern void *(*g_bsd_dev_calloc)(size_t, size_t);
extern void *(*g_bsd_dev_malloc)(size_t);
extern void bsd_dev_headers_reset_alloc_hooks(void);

static int g_dev_calloc_fail = 0;
static int g_dev_malloc_fail = 0;

static void *mock_dev_calloc(size_t nmemb, size_t size) {
  if (g_dev_calloc_fail) {
    return NULL;
  }
  return calloc(nmemb, size);
}

static void *mock_dev_malloc(size_t size) {
  if (g_dev_malloc_fail) {
    return NULL;
  }
  return malloc(size);
}

TEST test_bsd_dev_headers_ioctl(void) {
  struct backlight_props bp;
  tone_t tn;
  struct mtop mo;
  struct ar_hdr ah;
  Elf64_Ehdr eh;
  unsigned char raw_hid[4] = {0x05, 0x01, 0x09, 0x01};
  unsigned char report_buf[2] = {0, 0};
  hid_item_t item;
  report_desc_t rdesc;
  hid_data_t hdata;
  struct cam_device *fake_cam;
  FILE *tmp_fp;
  int rem = -1;

  bp.brightness = 50;
  tn.frequency = 440;
  tn.duration = 10;
  mo.mt_op = MTREW;
  mo.mt_count = 1;
  memset(&ah, 0, sizeof(ah));
  memset(&eh, 0, sizeof(eh));
  eh.e_machine = EM_X86_64;

  ASSERT_EQ(50U, bp.brightness);
  ASSERT_EQ(440, tn.frequency);
  ASSERT_EQ(MTREW, mo.mt_op);
  ASSERT_EQ(8, SARMAG);
  ASSERT_EQ(EM_X86_64, eh.e_machine);
  ASSERT_EQ(0, bsd_dev_headers_dummy_for_coverage());

  ASSERT_EQ(NULL, cam_open_device("/dev/pass0", 0));
  ASSERT_EQ(NULL, cam_open_spec_device("da", 0, 0, NULL));
  cam_close_device(NULL);
  fake_cam = (struct cam_device *)calloc(1, sizeof(*fake_cam));
  ASSERT(fake_cam != NULL);
  cam_close_device(fake_cam);

  hid_init(NULL);
  ASSERT_EQ(NULL, hid_get_report_desc(-1));
  ASSERT_EQ(NULL, hid_use_report_desc(NULL, 4));
  ASSERT_EQ(NULL, hid_use_report_desc(raw_hid, 0));
  hid_dispose_report_desc(NULL);

  rdesc = hid_use_report_desc(raw_hid, 4);
  ASSERT(rdesc != NULL);
  ASSERT_EQ(0, hid_report_size(NULL, hid_input, 0));
  ASSERT_EQ(4, hid_report_size(rdesc, hid_input, 0));
  ASSERT_EQ(NULL, hid_start_parse(NULL, 1, 0));
  hdata = hid_start_parse(rdesc, 1, 0);
  ASSERT(hdata != NULL);
  ASSERT_EQ(0, hid_get_item(hdata, &item));
  hid_end_parse(hdata);
  ASSERT_EQ(0, hid_locate(rdesc, 0, hid_input, &item, 0));
  ASSERT_EQ(0, hid_get_report_id(-1));
  ASSERT_EQ(0, hid_set_immed(-1, 1));
  ASSERT_STR_EQ("Generic_Desktop", hid_usage_page(1));
  ASSERT_STR_EQ("Vendor", hid_usage_page(2));
  ASSERT_STR_EQ("Pointer", hid_usage_in_page((awm_hid_usage_t)0x00010001UL));
  ASSERT_STR_EQ("Usage", hid_usage_in_page((awm_hid_usage_t)0x00010002UL));
  ASSERT_EQ(-1, hid_parse_usage_page(NULL));
  ASSERT_EQ(-1, hid_parse_usage_page("Vendor"));
  ASSERT_EQ(1, hid_parse_usage_page("Generic_Desktop"));
  ASSERT_EQ((awm_hid_susage_t)-1, hid_parse_usage_in_page(NULL));
  ASSERT_EQ((awm_hid_susage_t)-1, hid_parse_usage_in_page("Other"));
  ASSERT_EQ((awm_hid_susage_t)0x00010001UL, hid_parse_usage_in_page("Pointer"));

  memset(&item, 0, sizeof(item));
  ASSERT_EQ(0, hid_get_data(NULL, &item));
  ASSERT_EQ(0, hid_get_data(report_buf, NULL));
  ASSERT_EQ(0, hid_get_data(report_buf, &item));
  hid_set_data(NULL, &item, 5);
  hid_set_data(report_buf, NULL, 5);
  hid_set_data(report_buf, &item, 5);

  item.pos = 2;
  item.report_size = 3;
  hid_set_data(report_buf, &item, 5);
  ASSERT_EQ(5, hid_get_data(report_buf, &item));
  ASSERT_EQ(-1, hid_get_report(-1, hid_input, report_buf, 2));
  ASSERT_EQ(-1, hid_set_report(-1, hid_output, report_buf, 2));

  /* OOM branches in hid_use_report_desc and hid_start_parse */
  g_bsd_dev_calloc = mock_dev_calloc;
  g_bsd_dev_malloc = mock_dev_malloc;

  g_dev_calloc_fail = 1;
  ASSERT_EQ(NULL, hid_use_report_desc(raw_hid, 4));
  ASSERT_EQ(NULL, hid_start_parse(rdesc, 1, 0));
  g_dev_calloc_fail = 0;

  g_dev_malloc_fail = 1;
  ASSERT_EQ(NULL, hid_use_report_desc(raw_hid, 4));
  g_dev_malloc_fail = 0;

  bsd_dev_headers_reset_alloc_hooks();
  hid_dispose_report_desc(rdesc);

  ASSERT_EQ(2, sysdecode_abi_to_freebsd_errno(SYSDECODE_ABI_FREEBSD, 2));
  ASSERT_EQ(2, sysdecode_freebsd_to_abi_errno(SYSDECODE_ABI_FREEBSD, 2));
  ASSERT_STR_EQ("syscall", sysdecode_syscallnames(SYSDECODE_ABI_FREEBSD, 0U));
  ASSERT_STR_EQ("exit", sysdecode_syscallnames(SYSDECODE_ABI_FREEBSD, 1U));
  ASSERT_EQ(NULL, sysdecode_syscallnames(SYSDECODE_ABI_FREEBSD, 9999U));
  ASSERT_EQ(NULL, sysdecode_ioctlname(0UL));
  ASSERT_STR_EQ("SIGKILL", sysdecode_signame(9));
  ASSERT_STR_EQ("SIG", sysdecode_signame(15));
  ASSERT_STR_EQ("F_DUPFD", sysdecode_fcntl_cmd(0));
  ASSERT_STR_EQ("F_CMD", sysdecode_fcntl_cmd(1));
  ASSERT_STR_EQ("SOL_SOCKET", sysdecode_sockopt_level(0));
  ASSERT_STR_EQ("SO_DEBUG", sysdecode_sockopt_name(0, 0));
  ASSERT_EQ(1, sysdecode_open_flags(NULL, 0, NULL));
  ASSERT_EQ(1, sysdecode_open_flags(NULL, 0, &rem));
  ASSERT_EQ(0, rem);
#if defined(_MSC_VER)
  if (fopen_s(&tmp_fp, "tsysdec.txt", "wb") != 0) {
    tmp_fp = NULL;
  }
#else
  tmp_fp = fopen("tsysdec.txt", "wb");
#endif
  ASSERT(tmp_fp != NULL);
  ASSERT_EQ(1, sysdecode_open_flags(tmp_fp, 3, &rem));
  fclose(tmp_fp);
  remove("tsysdec.txt");
  ASSERT_EQ(1, sysdecode_mmap_prot(NULL, 0, &rem));
  ASSERT_EQ(1, sysdecode_mmap_flags(NULL, 0, &rem));

  PASS();
}

TEST test_bsd_dev_mac_ipsec_hesiod_mq_bt_sdp_kld(void) {
  mac_t lbl = (mac_t)1;
  char *txt = (char *)1;
  char *pol;
  void *hctx = NULL;
  char *bind_str;
  bdaddr_t ba;
  char bt_buf[24];
  struct protoent *pe;
  static struct kld_file_stat kst;

  /* MAC shims */
  ASSERT_EQ(-1, mac_prepare_process_label(NULL));
  ASSERT_EQ(-1, mac_prepare_process_label(&lbl));
  ASSERT(lbl == NULL);
  ASSERT_EQ(-1, mac_get_proc(lbl));
  ASSERT_EQ(-1, mac_get_pid(0, lbl));
  ASSERT_EQ(-1, mac_to_text(lbl, NULL));
  ASSERT_EQ(-1, mac_to_text(lbl, &txt));
  ASSERT(txt == NULL);
  ASSERT_EQ(0, mac_free(lbl));

  /* IPsec shims + OOM */
  ASSERT(ipsec_set_policy(NULL, 4) == NULL);
  ASSERT(ipsec_set_policy("in bypass", 0) == NULL);
  pol = ipsec_set_policy("in bypass", 9);
  ASSERT(pol != NULL);
  ASSERT_EQ(8, ipsec_get_policylen(pol));
  ASSERT_EQ(-1, ipsec_get_policylen(NULL));
  ASSERT(ipsec_strerror() != NULL);
  free(pol);

  g_bsd_dev_calloc = mock_dev_calloc;
  g_dev_calloc_fail = 1;
  ASSERT(ipsec_set_policy("in bypass", 9) == NULL);
  g_dev_calloc_fail = 0;
  bsd_dev_headers_reset_alloc_hooks();

  /* Hesiod & hes_* shims + OOM */
  ASSERT_EQ(0, hesiod_init(NULL));
  ASSERT_EQ(0, hesiod_init(&hctx));
  ASSERT(hctx != NULL);
  ASSERT(hesiod_to_bind(hctx, NULL, "passwd") == NULL);
  ASSERT(hesiod_to_bind(hctx, "root", NULL) == NULL);
  bind_str = hesiod_to_bind(hctx, "root", "passwd");
  ASSERT(bind_str != NULL);
  ASSERT_STR_EQ("root.passwd.ns", bind_str);
  free(bind_str);

  g_bsd_dev_malloc = mock_dev_malloc;
  g_dev_malloc_fail = 1;
  ASSERT(hesiod_to_bind(hctx, "root", "passwd") == NULL);
  g_dev_malloc_fail = 0;
  bsd_dev_headers_reset_alloc_hooks();

  ASSERT(hesiod_resolve(hctx, "root", "passwd") == NULL);
  hesiod_free_list(hctx, NULL);
  hesiod_end(hctx);

  ASSERT_EQ(0, hes_init());
  bind_str = hes_to_bind("alice", "group");
  ASSERT(bind_str != NULL);
  ASSERT_STR_EQ("alice.group.ns", bind_str);
  free(bind_str);
  ASSERT(hes_resolve("alice", "group") == NULL);
  ASSERT_EQ(HES_ER_NOTFOUND, hes_error());
  hes_free(NULL);

#if !defined(__linux__) && !defined(__FreeBSD__)
  {
    mqd_t q;
    mqd_t q2;
    mqd_t qs[16];
    char qname[32];
    char rbuf[32];
    struct mq_attr attr;
    struct mq_attr oattr;
    unsigned int prio = 0;
    int i;

    ASSERT_EQ((mqd_t)-1, mq_open(NULL, O_CREAT));
    ASSERT_EQ((mqd_t)-1, mq_open("/awm_noexist_q", 0));

    q = mq_open("/awm_test_q0", O_CREAT | O_RDWR);
    ASSERT(q >= 1);
    /* Reopen existing without O_EXCL -> same slot; with O_CREAT|O_EXCL -> -1 */
    q2 = mq_open("/awm_test_q0", O_CREAT | O_RDWR);
    ASSERT_EQ(q, q2);
    ASSERT_EQ((mqd_t)-1, mq_open("/awm_test_q0", O_CREAT | O_EXCL | O_RDWR));

    ASSERT_EQ(-1, mq_close(0));
    ASSERT_EQ(-1, mq_close(17));
    ASSERT_EQ(0, mq_close(q));

    ASSERT_EQ(-1, mq_getattr(0, &attr));
    ASSERT_EQ(-1, mq_getattr(17, &attr));
    ASSERT_EQ(-1, mq_getattr(q, NULL));
    ASSERT_EQ(0, mq_getattr(q, &attr));

    ASSERT_EQ(-1, mq_setattr(0, &attr, &oattr));
    ASSERT_EQ(-1, mq_setattr(17, &attr, &oattr));
    ASSERT_EQ(0, mq_setattr(q, NULL, NULL));
    attr.mq_flags = O_NONBLOCK;
    ASSERT_EQ(0, mq_setattr(q, &attr, &oattr));

    /* Empty receive -> EAGAIN */
    ASSERT_EQ(-1, (int)mq_receive(0, rbuf, sizeof(rbuf), NULL));
    ASSERT_EQ(-1, (int)mq_receive(17, rbuf, sizeof(rbuf), NULL));
    ASSERT_EQ(-1, (int)mq_receive(q, NULL, sizeof(rbuf), NULL));
    ASSERT_EQ(-1, (int)mq_receive(q, rbuf, sizeof(rbuf), NULL));

    /* Send invalid & valid, then receive too-small, NULL prio, non-NULL prio */
    ASSERT_EQ(-1, mq_send(0, "ab", 2, 1));
    ASSERT_EQ(-1, mq_send(17, "ab", 2, 1));
    ASSERT_EQ(-1, mq_send(q, NULL, 2, 1));
    ASSERT_EQ(-1, mq_send(q, "ab", 257, 1));
    ASSERT_EQ(0, mq_send(q, "hello", 5, 7));

    ASSERT_EQ(-1, (int)mq_receive(q, rbuf, 2, &prio));
    ASSERT_EQ(5, (int)mq_receive(q, rbuf, sizeof(rbuf), &prio));
    ASSERT_EQ(7U, prio);

    ASSERT_EQ(0, mq_timedsend(q, "xy", 2, 3, NULL));
    ASSERT_EQ(2, (int)mq_timedreceive(q, rbuf, sizeof(rbuf), NULL, NULL));
    ASSERT_EQ(0, mq_notify(q, NULL));

    ASSERT_EQ(-1, mq_unlink(NULL));
    ASSERT_EQ(-1, mq_unlink("/awm_missing_q"));
    ASSERT_EQ(0, mq_unlink("/awm_test_q0"));

    /* Exhaust all 16 slots to cover free_idx < 0 when O_CREAT is set */
    for (i = 0; i < 16; ++i) {
#if defined(_MSC_VER)
      sprintf_s(qname, sizeof(qname), "/awm_fill_%d", i);
#else
      sprintf(qname, "/awm_fill_%d", i);
#endif
      qs[i] = mq_open(qname, O_CREAT | O_RDWR);
      ASSERT(qs[i] >= 1);
    }
    ASSERT_EQ((mqd_t)-1, mq_open("/awm_fill_overflow", O_CREAT | O_RDWR));
    for (i = 0; i < 16; ++i) {
#if defined(_MSC_VER)
      sprintf_s(qname, sizeof(qname), "/awm_fill_%d", i);
#else
      sprintf(qname, "/awm_fill_%d", i);
#endif
      ASSERT_EQ(0, mq_unlink(qname));
    }
  }
#endif

  /* Bluetooth & SDP shims */
  ASSERT_EQ(0, bt_aton(NULL, &ba));
  ASSERT_EQ(0, bt_aton("01:02:03:04:05:06", NULL));
  ASSERT_EQ(0, bt_aton("invalid", &ba));
  ASSERT_EQ(0, bt_aton("01:02:03:04:05:100", &ba));
  ASSERT_EQ(1, bt_aton("aa:bb:cc:dd:ee:ff", &ba));
  ASSERT(bt_ntoa(NULL, bt_buf) == NULL);
  ASSERT_STR_EQ("aa:bb:cc:dd:ee:ff", bt_ntoa(&ba, NULL));
  ASSERT_STR_EQ("aa:bb:cc:dd:ee:ff", bt_ntoa(&ba, bt_buf));
  ASSERT(bt_gethostbyname("bt-host") == NULL);
  ASSERT(bt_gethostbyaddr((const char *)&ba, sizeof(ba), 0) == NULL);
  ASSERT(bt_getprotobyname(NULL) == NULL);
  ASSERT(bt_getprotobyname("nonexistent_psm") == NULL);
  pe = bt_getprotobyname("rfcomm");
  ASSERT(pe != NULL);
  ASSERT_EQ(3, (int)pe->p_proto);
  ASSERT(bt_getprotobynumber(999) == NULL);
  pe = bt_getprotobynumber(3);
  ASSERT(pe != NULL);
  ASSERT_STR_EQ("rfcomm", pe->p_name);

  ASSERT(sdp_open(&fbsd_bt_addr_any, &ba) == NULL);
  ASSERT(sdp_open_local(NULL) == NULL);
  ASSERT_EQ(0, sdp_close(NULL));
  ASSERT_EQ(ENOSYS, sdp_error(NULL));
  ASSERT_EQ(-1, sdp_search(NULL, 0, NULL, 0, NULL, 0, NULL));
  ASSERT_EQ(-1, sdp_register_service(NULL, 0, &ba, NULL, 0, NULL));
  ASSERT_EQ(-1, sdp_unregister_service(NULL, 0));

  /* KLD shims */
  memset(&kst, 0, sizeof(kst));
  ASSERT_EQ(-1, kldload("dummy.ko"));
  ASSERT_EQ(-1, kldunload(1));
  ASSERT_EQ(-1, kldunloadf(1, 0));
  ASSERT_EQ(-1, kldfind("dummy.ko"));
  ASSERT_EQ(0, kldnext(0));
  ASSERT_EQ(-1, kldstat(1, &kst));
  ASSERT_EQ(0, kldfirstmod(1));
  ASSERT_EQ(-1, kldsym(1, 0, NULL));

  PASS();
}

TEST test_fnv_hash_and_xlocale(void) {
  Fnv32_t h32;
  Fnv64_t h64;

  /* fnv_32_buf & fnv_32_str: empty and non-empty */
  ASSERT_EQ(FNV1_32_INIT, fnv_32_buf("", 0, FNV1_32_INIT));
  h32 = fnv_32_buf("abc", 3, FNV1_32_INIT);
  ASSERT_NEQ(FNV1_32_INIT, h32);
  ASSERT_EQ(FNV1_32_INIT, fnv_32_str("", FNV1_32_INIT));
  ASSERT_EQ(h32, fnv_32_str("abc", FNV1_32_INIT));

  /* fnv_64_buf & fnv_64_str: empty and non-empty */
  ASSERT_EQ(FNV1_64_INIT, fnv_64_buf("", 0, FNV1_64_INIT));
  h64 = fnv_64_buf("abc", 3, FNV1_64_INIT);
  ASSERT_NEQ(FNV1_64_INIT, h64);
  ASSERT_EQ(FNV1_64_INIT, fnv_64_str("", FNV1_64_INIT));
  ASSERT_EQ(h64, fnv_64_str("abc", FNV1_64_INIT));

#if defined(_WIN32) || defined(__APPLE__) || defined(__DOS__) ||               \
    defined(__MSDOS__) || defined(__WATCOMC__)
  (void)0;
  {
    locale_t loc1 = newlocale(LC_ALL_MASK, "C", (locale_t)0);
    locale_t loc2 = newlocale(LC_ALL_MASK, "C", loc1);
    ASSERT(loc1 != (locale_t)0);
    ASSERT(loc2 != (locale_t)0);
    ASSERT(duplocale((locale_t)0) != (locale_t)0);
    {
      locale_t dup2 = duplocale(loc1);
      ASSERT(dup2 != (locale_t)0);
#if defined(__APPLE__)
      freelocale(dup2);
#endif
    }
    (void)uselocale((locale_t)0);
    (void)uselocale(loc1);
#if defined(_WIN32)
    freelocale((locale_t)0);
    freelocale(loc1);
#else
    (void)uselocale(LC_GLOBAL_LOCALE);
    freelocale(loc2);
#endif
  }
#endif

  PASS();
}

SUITE(suite_bsd_dev_headers_core) {
  RUN_TEST(test_bsd_dev_headers_ioctl);
  RUN_TEST(test_bsd_dev_mac_ipsec_hesiod_mq_bt_sdp_kld);
  RUN_TEST(test_fnv_hash_and_xlocale);
}
