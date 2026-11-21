#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file posix-langinfo.c
 * @brief Implementation of posix-langinfo polyfills.
 */

/* clang-format off */
#include "posix-langinfo.h"
#include <stddef.h>
#include <stdio.h>
/* clang-format on */

#if defined(_WIN32) || defined(_WIN64)

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int UINT;
__declspec(dllimport) UINT __stdcall GetACP(void);

#ifdef __cplusplus
}
#endif

#endif /* defined(_WIN32) || defined(_WIN64) */

/**
 * @brief Return language information for a specified item.
 */
char *posix_langinfo(nl_item item) {
  static char lang_buf[64];
  const char *s = "";
#if defined(_WIN32) || defined(_WIN64)
  static char codeset_buf[32] = {0};
#endif

  switch (item) {
  case CODESET:
#if defined(_WIN32) || defined(_WIN64)
    if (codeset_buf[0] == '\0') {
      UINT acp = GetACP();
#if defined(_MSC_VER)
      sprintf_s(codeset_buf, sizeof(codeset_buf), "CP%u", acp);
#else
      sprintf(codeset_buf, "CP%u", acp);
#endif
    }
    return codeset_buf;
#else
    s = "UTF-8";
    break;
#endif
  case D_T_FMT:
    s = "%a %b %e %H:%M:%S %Y";
    break;
  case D_FMT:
    s = "%m/%d/%y";
    break;
  case T_FMT:
    s = "%H:%M:%S";
    break;
  case T_FMT_AMPM:
    s = "%I:%M:%S %p";
    break;
  case AM_STR:
    s = "AM";
    break;
  case PM_STR:
    s = "PM";
    break;
  case DAY_1:
    s = "Sunday";
    break;
  case DAY_2:
    s = "Monday";
    break;
  case DAY_3:
    s = "Tuesday";
    break;
  case DAY_4:
    s = "Wednesday";
    break;
  case DAY_5:
    s = "Thursday";
    break;
  case DAY_6:
    s = "Friday";
    break;
  case DAY_7:
    s = "Saturday";
    break;
  case ABDAY_1:
    s = "Sun";
    break;
  case ABDAY_2:
    s = "Mon";
    break;
  case ABDAY_3:
    s = "Tue";
    break;
  case ABDAY_4:
    s = "Wed";
    break;
  case ABDAY_5:
    s = "Thu";
    break;
  case ABDAY_6:
    s = "Fri";
    break;
  case ABDAY_7:
    s = "Sat";
    break;
  case MON_1:
    s = "January";
    break;
  case MON_2:
    s = "February";
    break;
  case MON_3:
    s = "March";
    break;
  case MON_4:
    s = "April";
    break;
  case MON_5:
    s = "May";
    break;
  case MON_6:
    s = "June";
    break;
  case MON_7:
    s = "July";
    break;
  case MON_8:
    s = "August";
    break;
  case MON_9:
    s = "September";
    break;
  case MON_10:
    s = "October";
    break;
  case MON_11:
    s = "November";
    break;
  case MON_12:
    s = "December";
    break;
  case ABMON_1:
    s = "Jan";
    break;
  case ABMON_2:
    s = "Feb";
    break;
  case ABMON_3:
    s = "Mar";
    break;
  case ABMON_4:
    s = "Apr";
    break;
  case ABMON_5:
    s = "May";
    break;
  case ABMON_6:
    s = "Jun";
    break;
  case ABMON_7:
    s = "Jul";
    break;
  case ABMON_8:
    s = "Aug";
    break;
  case ABMON_9:
    s = "Sep";
    break;
  case ABMON_10:
    s = "Oct";
    break;
  case ABMON_11:
    s = "Nov";
    break;
  case ABMON_12:
    s = "Dec";
    break;
  case ERA:
  case ERA_D_FMT:
  case ERA_D_T_FMT:
  case ERA_T_FMT:
  case ALT_DIGITS:
  case THOUSEP:
    s = "";
    break;
  case RADIXCHAR:
    s = ".";
    break;
  case YESEXPR:
    s = "^[yY]";
    break;
  case NOEXPR:
    s = "^[nN]";
    break;
  case CRNCYSTR:
    s = "-";
    break;
  default:
    s = "";
    break;
  }
  {
    size_t i = 0;
    while (s[i] != '\0' && i + 1 < sizeof(lang_buf)) {
      lang_buf[i] = s[i];
      i++;
    }
    lang_buf[i] = '\0';
  }
  return lang_buf;
}

/**
 * @brief Retrieves information on posix-langinfo availability.
 */
auto_win_msvc_error_t posix_langinfo_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_langinfo;

#endif
typedef int make_iso_compilers_happy_mega_batch3_5;
