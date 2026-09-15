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
    return "UTF-8";
#endif
  case D_T_FMT:
    return "%a %b %e %H:%M:%S %Y";
  case D_FMT:
    return "%m/%d/%y";
  case T_FMT:
    return "%H:%M:%S";
  case T_FMT_AMPM:
    return "%I:%M:%S %p";
  case AM_STR:
    return "AM";
  case PM_STR:
    return "PM";
  case DAY_1:
    return "Sunday";
  case DAY_2:
    return "Monday";
  case DAY_3:
    return "Tuesday";
  case DAY_4:
    return "Wednesday";
  case DAY_5:
    return "Thursday";
  case DAY_6:
    return "Friday";
  case DAY_7:
    return "Saturday";
  case ABDAY_1:
    return "Sun";
  case ABDAY_2:
    return "Mon";
  case ABDAY_3:
    return "Tue";
  case ABDAY_4:
    return "Wed";
  case ABDAY_5:
    return "Thu";
  case ABDAY_6:
    return "Fri";
  case ABDAY_7:
    return "Sat";
  case MON_1:
    return "January";
  case MON_2:
    return "February";
  case MON_3:
    return "March";
  case MON_4:
    return "April";
  case MON_5:
    return "May";
  case MON_6:
    return "June";
  case MON_7:
    return "July";
  case MON_8:
    return "August";
  case MON_9:
    return "September";
  case MON_10:
    return "October";
  case MON_11:
    return "November";
  case MON_12:
    return "December";
  case ABMON_1:
    return "Jan";
  case ABMON_2:
    return "Feb";
  case ABMON_3:
    return "Mar";
  case ABMON_4:
    return "Apr";
  case ABMON_5:
    return "May";
  case ABMON_6:
    return "Jun";
  case ABMON_7:
    return "Jul";
  case ABMON_8:
    return "Aug";
  case ABMON_9:
    return "Sep";
  case ABMON_10:
    return "Oct";
  case ABMON_11:
    return "Nov";
  case ABMON_12:
    return "Dec";
  case ERA:
    return "";
  case ERA_D_FMT:
    return "";
  case ERA_D_T_FMT:
    return "";
  case ERA_T_FMT:
    return "";
  case ALT_DIGITS:
    return "";
  case RADIXCHAR:
    return ".";
  case THOUSEP:
    return "";
  case YESEXPR:
    return "^[yY]";
  case NOEXPR:
    return "^[nN]";
  case CRNCYSTR:
    return "-";
  default:
    return "";
  }
}

/**
 * @brief Retrieves information on posix-langinfo availability.
 */
enum posix_langinfo_error_code posix_langinfo_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_LANGINFO_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_LANGINFO_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_langinfo;
