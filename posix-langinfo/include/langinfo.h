#ifndef POSIX_LANGINFO_LANGINFO_H
#define POSIX_LANGINFO_LANGINFO_H

/**
 * @file langinfo.h
 * @brief POSIX langinfo header providing nl_item and nl_langinfo.
 */

#if !defined(__nl_item_defined) && !defined(_NL_ITEM_DEFINED)
#define _NL_ITEM_DEFINED
#define __nl_item_defined 1
/**
 * @brief The type used to identify langinfo items.
 */
typedef int nl_item;
#endif

#ifndef CODESET
/** @brief Character encoding name. */
#define CODESET 1
/** @brief String for formatting date and time. */
#define D_T_FMT 2
/** @brief Date format string. */
#define D_FMT 3
/** @brief Time format string. */
#define T_FMT 4
/** @brief AM/PM time format string. */
#define T_FMT_AMPM 5
/** @brief Ante Meridian affix. */
#define AM_STR 6
/** @brief Post Meridian affix. */
#define PM_STR 7
/** @brief Name of Sunday. */
#define DAY_1 8
/** @brief Name of Monday. */
#define DAY_2 9
/** @brief Name of Tuesday. */
#define DAY_3 10
/** @brief Name of Wednesday. */
#define DAY_4 11
/** @brief Name of Thursday. */
#define DAY_5 12
/** @brief Name of Friday. */
#define DAY_6 13
/** @brief Name of Saturday. */
#define DAY_7 14
/** @brief Abbreviated name of Sunday. */
#define ABDAY_1 15
/** @brief Abbreviated name of Monday. */
#define ABDAY_2 16
/** @brief Abbreviated name of Tuesday. */
#define ABDAY_3 17
/** @brief Abbreviated name of Wednesday. */
#define ABDAY_4 18
/** @brief Abbreviated name of Thursday. */
#define ABDAY_5 19
/** @brief Abbreviated name of Friday. */
#define ABDAY_6 20
/** @brief Abbreviated name of Saturday. */
#define ABDAY_7 21
/** @brief Name of January. */
#define MON_1 22
/** @brief Name of February. */
#define MON_2 23
/** @brief Name of March. */
#define MON_3 24
/** @brief Name of April. */
#define MON_4 25
/** @brief Name of May. */
#define MON_5 26
/** @brief Name of June. */
#define MON_6 27
/** @brief Name of July. */
#define MON_7 28
/** @brief Name of August. */
#define MON_8 29
/** @brief Name of September. */
#define MON_9 30
/** @brief Name of October. */
#define MON_10 31
/** @brief Name of November. */
#define MON_11 32
/** @brief Name of December. */
#define MON_12 33
/** @brief Abbreviated name of January. */
#define ABMON_1 34
/** @brief Abbreviated name of February. */
#define ABMON_2 35
/** @brief Abbreviated name of March. */
#define ABMON_3 36
/** @brief Abbreviated name of April. */
#define ABMON_4 37
/** @brief Abbreviated name of May. */
#define ABMON_5 38
/** @brief Abbreviated name of June. */
#define ABMON_6 39
/** @brief Abbreviated name of July. */
#define ABMON_7 40
/** @brief Abbreviated name of August. */
#define ABMON_8 41
/** @brief Abbreviated name of September. */
#define ABMON_9 42
/** @brief Abbreviated name of October. */
#define ABMON_10 43
/** @brief Abbreviated name of November. */
#define ABMON_11 44
/** @brief Abbreviated name of December. */
#define ABMON_12 45
/** @brief Era description segments. */
#define ERA 46
/** @brief Era date format string. */
#define ERA_D_FMT 47
/** @brief Era date and time format string. */
#define ERA_D_T_FMT 48
/** @brief Era time format string. */
#define ERA_T_FMT 49
/** @brief Alternative symbols for digits. */
#define ALT_DIGITS 50
/** @brief Radix character. */
#define RADIXCHAR 51
/** @brief Separator for thousands. */
#define THOUSEP 52
/** @brief Affirmative response expression. */
#define YESEXPR 53
/** @brief Negative response expression. */
#define NOEXPR 54
/** @brief Local currency symbol. */
#define CRNCYSTR 55
#endif

#ifndef nl_langinfo
/** @brief Maps nl_langinfo to posix_langinfo on non-POSIX platforms. */
#define nl_langinfo posix_langinfo
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return language information.
 * @param item The language information item to retrieve.
 * @return A pointer to a string containing the requested information.
 */
char *posix_langinfo(nl_item item);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LANGINFO_LANGINFO_H */
