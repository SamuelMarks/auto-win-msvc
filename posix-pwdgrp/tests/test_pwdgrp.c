/* clang-format off */
#include "greatest.h"
#if defined(_WIN32)
#include <winsock2.h>
#include <lm.h>
#endif
#include "posix-pwdgrp.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

extern int dummy_posix_pwdgrp(void);

#if defined(_WIN32)
static int g_pw_oom_countdown = 0;
static int g_sid_valid = 1;
static int g_sid_count_null = 0;
static unsigned char g_sid_count_val = 1;
static unsigned long g_sid_rid_val = 1001UL;
static unsigned long g_last_err_val = ERROR_INSUFFICIENT_BUFFER;
static int g_lookup_second_ok = 1;
static int g_lookup_pe_use = (int)SidTypeUser;
static int g_lookup_fail_all = 0;
static int g_wide_fail = 0;
static int g_utf8_to_wide_res = 5;
static int g_enum_mode = 0;
static int g_enum_step = 0;
static int g_uinfo_mode = 0;

static unsigned short g_w_alice[6] = {'a', 'l', 'i', 'c', 'e', 0};
static unsigned short g_w_bob[4] = {'b', 'o', 'b', 0};
static unsigned short g_w_home[6] = {'C', ':', '/', 'h', 'm', 0};
static unsigned short g_w_full[6] = {'A', 'l', 'i', 'c', 'e', 0};

static void *mock_pw_malloc(size_t sz) {
  if (g_pw_oom_countdown > 0) {
    g_pw_oom_countdown--;
    if (g_pw_oom_countdown == 0) {
      return NULL;
    }
  }
  return malloc(sz);
}

static int mock_is_valid_sid(void *sid) {
  (void)sid;
  return g_sid_valid;
}

static unsigned char *mock_sid_sub_auth_count(void *sid) {
  (void)sid;
  return g_sid_count_null ? NULL : &g_sid_count_val;
}

static unsigned long *mock_sid_sub_auth(void *sid, unsigned long idx) {
  (void)sid;
  (void)idx;
  return &g_sid_rid_val;
}

static int mock_lookup_account(const char *name, void *sid,
                               unsigned long *cb_sid, char *dom,
                               unsigned long *cb_dom, int *pe_use) {
  (void)dom;
  if (g_lookup_fail_all || strcmp(name, "missing_acct") == 0) {
    g_last_err_val = ERROR_NONE_MAPPED;
    return 0;
  }
  if (sid == NULL) {
    *cb_sid = 16UL;
    *cb_dom = 16UL;
    return 0;
  }
  if (!g_lookup_second_ok) {
    return 0;
  }
  *pe_use = g_lookup_pe_use;
  if (strcmp(name, "bob") == 0) {
    g_sid_rid_val = 2002UL;
  }
  return 1;
}

static unsigned long mock_get_last_error(void) { return g_last_err_val; }

static int mock_wide_to_utf8(const unsigned short *wstr, char *out,
                             int out_sz) {
  int i = 0;
  if (g_wide_fail > 0) {
    g_wide_fail--;
    if (g_wide_fail == 0) {
      return 0;
    }
  }
  while (wstr[i] != 0) {
    if (out != NULL && i + 1 < out_sz) {
      out[i] = (char)wstr[i];
    }
    i++;
  }
  if (out != NULL && out_sz > 0) {
    out[(i < out_sz) ? i : (out_sz - 1)] = '\0';
  }
  return i + 1;
}

static int mock_utf8_to_wide(const char *str, unsigned short *out, int out_sz) {
  int i = 0;
  if (g_utf8_to_wide_res <= 0) {
    return g_utf8_to_wide_res;
  }
  if (out != NULL && out_sz > 0) {
    while (str[i] != '\0' && i + 1 < out_sz) {
      out[i] = (unsigned short)(unsigned char)str[i];
      i++;
    }
    out[i] = 0;
  }
  return g_utf8_to_wide_res;
}

static unsigned long mock_net_group_enum(unsigned char **buf,
                                         unsigned long *entries_read,
                                         unsigned long *total_entries,
                                         size_t *resume) {
  LPLOCALGROUP_INFO_0 arr;
  (void)resume;
  if (g_enum_mode == 1) {
    *buf = NULL;
    *entries_read = 0;
    *total_entries = 0;
    return 5UL;
  }
  if (g_enum_mode == 2) {
    *buf = NULL;
    *entries_read = 0;
    *total_entries = 0;
    return (unsigned long)NERR_Success;
  }
  if (g_enum_mode == 3) {
    arr = (LPLOCALGROUP_INFO_0)calloc(2, sizeof(LOCALGROUP_INFO_0));
    arr[0].lgrpi0_name = (LPWSTR)g_w_alice;
    arr[1].lgrpi0_name = (LPWSTR)g_w_bob;
    *buf = (unsigned char *)arr;
    *entries_read = 2;
    *total_entries = 2;
    return (unsigned long)NERR_Success;
  }
  if (g_enum_mode == 4) {
    g_enum_step++;
    if (g_enum_step == 1) {
      arr = (LPLOCALGROUP_INFO_0)calloc(4, sizeof(LOCALGROUP_INFO_0));
      arr[0].lgrpi0_name = NULL;
      arr[1].lgrpi0_name = (LPWSTR)g_w_alice;
      arr[2].lgrpi0_name = (LPWSTR)g_w_bob;
      *buf = (unsigned char *)arr;
      *entries_read = 3;
      *total_entries = 4;
      return (unsigned long)ERROR_MORE_DATA;
    }
    *buf = NULL;
    *entries_read = 0;
    *total_entries = 0;
    return (unsigned long)NERR_Success;
  }
  *buf = NULL;
  *entries_read = 0;
  *total_entries = 0;
  return 5UL;
}

static unsigned long mock_net_user_enum(unsigned char **buf,
                                        unsigned long *entries_read,
                                        unsigned long *total_entries,
                                        size_t *resume) {
  LPUSER_INFO_0 arr;
  (void)resume;
  if (g_enum_mode == 1) {
    *buf = NULL;
    *entries_read = 0;
    *total_entries = 0;
    return 5UL;
  }
  if (g_enum_mode == 2) {
    *buf = NULL;
    *entries_read = 0;
    *total_entries = 0;
    return (unsigned long)NERR_Success;
  }
  if (g_enum_mode == 3) {
    arr = (LPUSER_INFO_0)calloc(2, sizeof(USER_INFO_0));
    arr[0].usri0_name = (LPWSTR)g_w_alice;
    arr[1].usri0_name = (LPWSTR)g_w_bob;
    *buf = (unsigned char *)arr;
    *entries_read = 2;
    *total_entries = 2;
    return (unsigned long)NERR_Success;
  }
  if (g_enum_mode == 4) {
    g_enum_step++;
    if (g_enum_step == 1) {
      arr = (LPUSER_INFO_0)calloc(3, sizeof(USER_INFO_0));
      arr[0].usri0_name = NULL;
      arr[1].usri0_name = (LPWSTR)g_w_alice;
      arr[2].usri0_name = (LPWSTR)g_w_bob;
      *buf = (unsigned char *)arr;
      *entries_read = 3;
      *total_entries = 3;
      return (unsigned long)ERROR_MORE_DATA;
    }
    *buf = NULL;
    *entries_read = 0;
    *total_entries = 0;
    return (unsigned long)NERR_Success;
  }
  *buf = NULL;
  *entries_read = 0;
  *total_entries = 0;
  return 5UL;
}

static unsigned long mock_net_user_get_info(const unsigned short *wname,
                                            unsigned char **buf) {
  LPUSER_INFO_4 ui;
  (void)wname;
  if (g_uinfo_mode == 0) {
    *buf = NULL;
    return 5UL;
  }
  if (g_uinfo_mode == 4) {
    *buf = NULL;
    return (unsigned long)NERR_UserNotFound;
  }
  ui = (LPUSER_INFO_4)calloc(1, sizeof(USER_INFO_4));
  if (g_uinfo_mode == 1) {
    ui->usri4_home_dir = (LPWSTR)g_w_home;
    ui->usri4_full_name = (LPWSTR)g_w_full;
  } else if (g_uinfo_mode == 2) {
    ui->usri4_home_dir = NULL;
    ui->usri4_full_name = (LPWSTR)g_w_full;
  } else if (g_uinfo_mode == 3) {
    ui->usri4_home_dir = NULL;
    ui->usri4_full_name = NULL;
  }
  *buf = (unsigned char *)ui;
  return 0UL;
}

static unsigned long mock_net_buf_free(void *buf) {
  free(buf);
  return 0UL;
}

static void reset_mock_vars(void) {
  g_pw_oom_countdown = 0;
  g_sid_valid = 1;
  g_sid_count_null = 0;
  g_sid_count_val = 1;
  g_sid_rid_val = 1001UL;
  g_last_err_val = ERROR_INSUFFICIENT_BUFFER;
  g_lookup_second_ok = 1;
  g_lookup_pe_use = (int)SidTypeUser;
  g_lookup_fail_all = 0;
  g_wide_fail = 0;
  g_utf8_to_wide_res = 5;
  g_enum_mode = 0;
  g_enum_step = 0;
  g_uinfo_mode = 0;
}

TEST test_pwdgrp_win32_mocked_branches(void) {
  struct posix_pwdgrp_win32_hooks hooks;
  struct group grp;
  struct group *gres;
  struct passwd pwd;
  struct passwd *pres;
  union {
    void *align_ptr;
    char bytes[256];
  } aligned;
  gid_t glist[4];
  int ngroups;

  hooks.malloc_fn = mock_pw_malloc;
  hooks.is_valid_sid_fn = mock_is_valid_sid;
  hooks.sid_sub_auth_count_fn = mock_sid_sub_auth_count;
  hooks.sid_sub_auth_fn = mock_sid_sub_auth;
  hooks.lookup_account_fn = mock_lookup_account;
  hooks.get_last_error_fn = mock_get_last_error;
  hooks.wide_to_utf8_fn = mock_wide_to_utf8;
  hooks.utf8_to_wide_fn = mock_utf8_to_wide;
  hooks.net_group_enum_fn = mock_net_group_enum;
  hooks.net_user_enum_fn = mock_net_user_enum;
  hooks.net_user_get_info_fn = mock_net_user_get_info;
  hooks.net_buf_free_fn = mock_net_buf_free;

  posix_pwdgrp_set_win32_hooks(&hooks);

  /* 1. get_sid_from_name OOM & failure branches via getgrnam_r */
  reset_mock_vars();
  g_pw_oom_countdown = 1;
  ASSERT_EQ(ENOENT, getgrnam_r("alice", &grp, aligned.bytes,
                               sizeof(aligned.bytes), &gres));
  reset_mock_vars();
  g_pw_oom_countdown = 2;
  ASSERT_EQ(ENOENT, getgrnam_r("alice", &grp, aligned.bytes,
                               sizeof(aligned.bytes), &gres));
  reset_mock_vars();
  g_lookup_second_ok = 0;
  ASSERT_EQ(ENOENT, getgrnam_r("alice", &grp, aligned.bytes,
                               sizeof(aligned.bytes), &gres));

  /* 2. Group fallback ("root", "wheel", "users") + populate_group_record ERANGE
   */
  reset_mock_vars();
  g_lookup_fail_all = 1;
  ASSERT_EQ(ERANGE, getgrnam_r("root", &grp, aligned.bytes, 2, &gres));
  ASSERT_EQ(ERANGE, getgrnam_r("root", &grp, aligned.bytes, 6, &gres));
  /* "root\0*\0" = 7 bytes; on 8-byte aligned buffer align_offset=1, so 7 fails
   * align */
  ASSERT_EQ(ERANGE, getgrnam_r("root", &grp, aligned.bytes, 7, &gres));
  /* 8 bytes passes align (leaves 0 < sizeof(char*)) */
  ASSERT_EQ(ERANGE, getgrnam_r("root", &grp, aligned.bytes, 8, &gres));
  ASSERT_EQ(0, getgrnam_r("root", &grp, aligned.bytes, 64, &gres));
  ASSERT_EQ(0, getgrnam_r("wheel", &grp, aligned.bytes, 64, &gres));
  /* "users\0*\0" = 8 bytes -> align_offset == 0 on 8-byte & 4-byte aligned
   * buffer! */
  ASSERT_EQ(0, getgrnam_r("users", &grp, aligned.bytes, 64, &gres));
  ASSERT_EQ((gid_t)100, grp.gr_gid);

  /* 3. Group peUse types and sid_to_rid failure branches */
  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeUser;
  ASSERT_EQ(ENOENT, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));

  g_lookup_pe_use = (int)SidTypeAlias;
  ASSERT_EQ(0, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));
  g_lookup_pe_use = (int)SidTypeGroup;
  ASSERT_EQ(0, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));
  g_lookup_pe_use = (int)SidTypeWellKnownGroup;
  ASSERT_EQ(0, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));

  g_sid_valid = 0;
  ASSERT_EQ(EINVAL, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));
  g_sid_valid = 1;
  g_sid_count_null = 1;
  ASSERT_EQ(EINVAL, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));
  g_sid_count_null = 0;
  g_sid_count_val = 0;
  ASSERT_EQ(EINVAL, getgrnam_r("alice", &grp, aligned.bytes, 64, &gres));

  /* 4. setgrent / getgrent / endgrent branches */
  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeGroup;
  g_enum_mode = 1;
  setgrent();
  ASSERT(getgrent() == NULL);

  g_enum_mode = 2;
  ASSERT(getgrent() == NULL);

  g_enum_mode = 3;
  ASSERT(getgrent() != NULL);
  /* Unread entry still buffered -> endgrent frees current_grp_info */
  endgrent();

  g_enum_mode = 3;
  ASSERT(getgrent() != NULL);
  ASSERT(getgrent() != NULL);
  /* Buffer now exhausted, next getgrent frees old current_grp_info & re-enums
   */
  g_enum_mode = 2;
  ASSERT(getgrent() == NULL);

  /* utf16_to_utf8 failures inside getgrent (WideCharToMultiByte<=0 and OOM) */
  g_enum_mode = 3;
  g_wide_fail = 1;
  ASSERT(getgrent() == NULL);
  g_pw_oom_countdown = 1;
  ASSERT(getgrent() == NULL);
  endgrent();

  /* ERROR_MORE_DATA in getgrent */
  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeGroup;
  g_enum_mode = 4;
  ASSERT(getgrent() == NULL);
  endgrent();

  /* 5. getgrgid_r & getgrgid branches */
  ASSERT_EQ(EINVAL, getgrgid_r(0, NULL, aligned.bytes, 64, &gres));
  ASSERT_EQ(EINVAL, getgrgid_r(0, &grp, NULL, 64, &gres));
  ASSERT_EQ(EINVAL, getgrgid_r(0, &grp, aligned.bytes, 64, NULL));

  /* Enum failure (mode 1) */
  reset_mock_vars();
  g_enum_mode = 1;
  ASSERT_EQ(ENOENT, getgrgid_r((gid_t)70000U, &grp, aligned.bytes, 64, &gres));

  /* Mid-array match (1001 = "alice" at index 1 < entriesRead=3) */
  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeGroup;
  g_enum_mode = 4;
  ASSERT_EQ(0, getgrgid_r((gid_t)1001, &grp, aligned.bytes, 64, &gres));
  ASSERT_STR_EQ("alice", grp.gr_name);

  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeGroup;
  g_enum_mode = 4;
  /* Search for 2002 ("bob"): entry 0 NULL, entry 1 "alice" (1001 != 2002),
   * entry 2 "bob" (2002 == 2002)! */
  ASSERT_EQ(0, getgrgid_r((gid_t)2002, &grp, aligned.bytes, 64, &gres));
  ASSERT_STR_EQ("bob", grp.gr_name);

  /* Exercise get_sid_from_name fail and sid_to_rid fail inside getgrgid_r loop
   */
  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeGroup;
  g_enum_mode = 4;
  g_sid_valid = 0;
  ASSERT_EQ(ENOENT, getgrgid_r((gid_t)70000U, &grp, aligned.bytes, 64, &gres));

  reset_mock_vars();
  g_lookup_fail_all = 1;
  g_enum_mode = 4;
  ASSERT_EQ(0, getgrgid_r((gid_t)0, &grp, aligned.bytes, 64, &gres));
  ASSERT_EQ(0, getgrgid_r((gid_t)50, &grp, aligned.bytes, 64, &gres));
  ASSERT_EQ(ERANGE, getgrgid_r((gid_t)50, &grp, aligned.bytes, 2, &gres));
  ASSERT(getgrgid((gid_t)70000U) == NULL);

  /* 6. getpwnam_r & getpwnam branches */
  reset_mock_vars();
  g_lookup_fail_all = 1;
  /* Test all 5 ERANGE points in populate_passwd_record ("root", "*", "C:",
   * "root", "cmd.exe") */
  ASSERT_EQ(ERANGE, getpwnam_r("root", &pwd, aligned.bytes, 2, &pres));
  ASSERT_EQ(ERANGE, getpwnam_r("root", &pwd, aligned.bytes, 6, &pres));
  ASSERT_EQ(ERANGE, getpwnam_r("root", &pwd, aligned.bytes, 8, &pres));
  ASSERT_EQ(ERANGE, getpwnam_r("root", &pwd, aligned.bytes, 12, &pres));
  ASSERT_EQ(ERANGE, getpwnam_r("root", &pwd, aligned.bytes, 18, &pres));
  ASSERT_EQ(0, getpwnam_r("root", &pwd, aligned.bytes, 64, &pres));
  ASSERT_EQ(ENOENT, getpwnam_r("missing_acct", &pwd, aligned.bytes, 64, &pres));

  /* peUse != SidTypeUser */
  reset_mock_vars();
  g_lookup_pe_use = (int)SidTypeGroup;
  ASSERT_EQ(ENOENT, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  /* utf8_to_wide <= 0 and wname OOM */
  reset_mock_vars();
  g_utf8_to_wide_res = 0;
  ASSERT_EQ(0, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_pw_oom_countdown = 3;
  ASSERT_EQ(0, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  /* sid_to_rid failure with userInfo == NULL and userInfo != NULL */
  reset_mock_vars();
  g_sid_valid = 0;
  g_uinfo_mode = 0;
  ASSERT_EQ(EINVAL, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));
  g_uinfo_mode = 1;
  ASSERT_EQ(EINVAL, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  /* userInfo NERR_UserNotFound and UTF-16 conversion failures & combinations */
  reset_mock_vars();
  g_uinfo_mode = 4;
  ASSERT_EQ(ENOENT, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_uinfo_mode = 1;
  g_wide_fail = 1;
  ASSERT_EQ(ENOMEM, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_uinfo_mode = 1;
  g_wide_fail = 3;
  ASSERT_EQ(ENOMEM, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_uinfo_mode = 2;
  g_wide_fail = 1;
  ASSERT_EQ(ENOMEM, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_uinfo_mode = 1;
  ASSERT_EQ(0, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));
  ASSERT_STR_EQ("C:/hm", pwd.pw_dir);
  ASSERT_STR_EQ("Alice", pwd.pw_gecos);

  reset_mock_vars();
  g_uinfo_mode = 3;
  ASSERT_EQ(0, getpwnam_r("alice", &pwd, aligned.bytes, 64, &pres));

  /* 7. setpwent / getpwent / endpwent branches */
  reset_mock_vars();
  g_enum_mode = 1;
  setpwent();
  ASSERT(getpwent() == NULL);

  g_enum_mode = 2;
  ASSERT(getpwent() == NULL);

  g_enum_mode = 3;
  ASSERT(getpwent() != NULL);
  endpwent();

  g_enum_mode = 3;
  ASSERT(getpwent() != NULL);
  ASSERT(getpwent() != NULL);
  g_enum_mode = 2;
  ASSERT(getpwent() == NULL);

  g_enum_mode = 3;
  g_wide_fail = 1;
  ASSERT(getpwent() == NULL);
  endpwent();

  reset_mock_vars();
  g_enum_mode = 4;
  ASSERT(getpwent() == NULL);
  endpwent();

  /* 8. getpwuid_r & getpwuid branches */
  ASSERT_EQ(EINVAL, getpwuid_r(0, NULL, aligned.bytes, 64, &pres));
  ASSERT_EQ(EINVAL, getpwuid_r(0, &pwd, NULL, 64, &pres));
  ASSERT_EQ(EINVAL, getpwuid_r(0, &pwd, aligned.bytes, 64, NULL));

  reset_mock_vars();
  g_enum_mode = 1;
  ASSERT_EQ(ENOENT, getpwuid_r((uid_t)70000U, &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_enum_mode = 4;
  ASSERT_EQ(0, getpwuid_r((uid_t)1001, &pwd, aligned.bytes, 64, &pres));
  ASSERT_STR_EQ("alice", pwd.pw_name);

  reset_mock_vars();
  g_enum_mode = 4;
  ASSERT_EQ(0, getpwuid_r((uid_t)2002, &pwd, aligned.bytes, 64, &pres));
  ASSERT_STR_EQ("bob", pwd.pw_name);

  reset_mock_vars();
  g_enum_mode = 4;
  g_sid_valid = 0;
  ASSERT_EQ(ENOENT, getpwuid_r((uid_t)70000U, &pwd, aligned.bytes, 64, &pres));

  reset_mock_vars();
  g_lookup_fail_all = 1;
  g_enum_mode = 4;
  ASSERT_EQ(0, getpwuid_r((uid_t)50, &pwd, aligned.bytes, 64, &pres));
  ASSERT_EQ(ERANGE, getpwuid_r((uid_t)50, &pwd, aligned.bytes, 2, &pres));
  ASSERT(getpwuid((uid_t)70000U) == NULL);

  /* 9. getgrouplist branches */
  ngroups = 4;
  ASSERT_EQ(-1, getgrouplist(NULL, (gid_t)10, glist, &ngroups));
  ASSERT_EQ(-1, getgrouplist("alice", (gid_t)10, glist, NULL));
  ngroups = 0;
  ASSERT_EQ(-1, getgrouplist("alice", (gid_t)10, glist, &ngroups));
  ASSERT_EQ(1, ngroups);
  ngroups = 2;
  ASSERT_EQ(1, getgrouplist("alice", (gid_t)10, NULL, &ngroups));
  ngroups = 2;
  ASSERT_EQ(1, getgrouplist("alice", (gid_t)10, glist, &ngroups));
  ASSERT_EQ((gid_t)10, glist[0]);

  posix_pwdgrp_set_win32_hooks(NULL);
  PASS();
}
#endif

TEST test_pwdgrp(void) {
  struct passwd *pw;
  struct group *gr;
  uid_t uid = 0;
  gid_t gid = 0;

  ASSERT_EQ(0, dummy_posix_pwdgrp());

  setpwent();
  pw = getpwent();
  if (pw != NULL) {
    ASSERT(pw->pw_name != NULL);
    ASSERT(strlen(pw->pw_name) > 0);
  }
  endpwent();

  setgrent();
  gr = getgrent();
  if (gr != NULL) {
    ASSERT(gr->gr_name != NULL);
    ASSERT(strlen(gr->gr_name) > 0);
  }
  endgrent();

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  ASSERT_EQ(1, setpassent(1));
  endpwent();
  ASSERT_EQ(1, setgroupent(1));
  endgrent();

  ASSERT(user_from_uid((uid_t)0, 0) != NULL);
  ASSERT(user_from_uid((uid_t)99999999U, 1) == NULL);
  ASSERT_STR_EQ("99999999", user_from_uid((uid_t)99999999U, 0));

  ASSERT(group_from_gid((gid_t)0, 0) != NULL);
  ASSERT(group_from_gid((gid_t)99999999U, 1) == NULL);
  ASSERT_STR_EQ("99999999", group_from_gid((gid_t)99999999U, 0));
#endif

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
  ASSERT_EQ(-1, uid_from_user(NULL, &uid));
  ASSERT_EQ(-1, uid_from_user("root", NULL));
  ASSERT_EQ(-1, uid_from_user("nonexistent_awm_user_99999", &uid));
  pw = getpwuid((uid_t)0);
  if (pw != NULL && pw->pw_name != NULL) {
    ASSERT_EQ(0, uid_from_user(pw->pw_name, &uid));
    ASSERT_EQ((uid_t)0, uid);
  }

  ASSERT_EQ(-1, gid_from_group(NULL, &gid));
  ASSERT_EQ(-1, gid_from_group("wheel", NULL));
  ASSERT_EQ(-1, gid_from_group("nonexistent_awm_group_99999", &gid));
  gr = getgrgid((gid_t)0);
  if (gr != NULL && gr->gr_name != NULL) {
    ASSERT_EQ(0, gid_from_group(gr->gr_name, &gid));
    ASSERT_EQ((gid_t)0, gid);
  }

  ASSERT_EQ(0, pwcache_userdb(NULL, NULL, NULL, NULL));
  ASSERT_EQ(0, pwcache_groupdb(NULL, NULL, NULL, NULL));
#endif

  PASS();
}

SUITE(suite_posix_pwdgrp_pwdgrp) {
  RUN_TEST(test_pwdgrp);
#if defined(_WIN32)
  RUN_TEST(test_pwdgrp_win32_mocked_branches);
#endif
}
