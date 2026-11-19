#ifndef POSIX_CORE_PATHS_H
#define POSIX_CORE_PATHS_H

#if defined(__GNUC__)
#endif

/* clang-format off */
#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__WATCOMC__) &&         \
    !defined(__MSDOS__) && !defined(__DOS__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <paths.h>
#else
#include <paths.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file paths.h
 * @brief Standard path definitions for POSIX environments and Windows polyfill.
 */

#if defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) ||            \
    defined(__MSDOS__) || defined(__DOS__)

/** @brief Default search path. */
#ifndef _PATH_DEFPATH
#define _PATH_DEFPATH "C:\\Windows\\system32;C:\\Windows"
#endif

/** @brief Standard system path. */
#ifndef _PATH_STDPATH
#define _PATH_STDPATH "C:\\Windows\\system32;C:\\Windows"
#endif

/** @brief Path to the default shell executable. */
#ifndef _PATH_BSHELL
#define _PATH_BSHELL "dash.exe"
#endif

/** @brief Path to the null device. */
#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL "NUL"
#endif

/** @brief Path to the controlling terminal. */
#ifndef _PATH_TTY
#define _PATH_TTY "CON"
#endif

/** @brief Path to the temporary directory. */
#ifndef _PATH_TMP
#define _PATH_TMP "C:\\Windows\\Temp\\"
#endif

/** @brief Path to persistent temporary directory. */
#ifndef _PATH_VARTMP
#define _PATH_VARTMP "C:\\Windows\\Temp\\"
#endif

/** @brief Path to C shell executable. */
#ifndef _PATH_CSHELL
#define _PATH_CSHELL "cmd.exe"
#endif

/** @brief Path to device directory. */
#ifndef _PATH_DEV
#define _PATH_DEV "\\\\.\\"
#endif

/** @brief Path to default vi editor. */
#ifndef _PATH_VI
#define _PATH_VI "vi.exe"
#endif

/** @brief Path to system mail spool directory. */
#ifndef _PATH_MAILDIR
#define _PATH_MAILDIR "C:\\Windows\\Temp"
#endif

/** @brief Path to default pager executable. */
#ifndef _PATH_PAGER
#define _PATH_PAGER "more.exe"
#endif

/** @brief Path to locale database directory. */
#ifndef _PATH_LOCALE
#define _PATH_LOCALE "C:\\usr\\share\\locale"
#endif

/** @brief Path to dictionary words file. */
#ifndef _PATH_WORDS
#define _PATH_WORDS "C:\\usr\\share\\dict\\words"
#endif

#else /* POSIX fallback defaults when omitted by host <paths.h> */

#ifndef _PATH_VARTMP
#define _PATH_VARTMP "/var/tmp/"
#endif
#ifndef _PATH_CSHELL
#define _PATH_CSHELL "/bin/csh"
#endif
#ifndef _PATH_DEV
#define _PATH_DEV "/dev/"
#endif
#ifndef _PATH_VI
#define _PATH_VI "/usr/bin/vi"
#endif
#ifndef _PATH_MAILDIR
#define _PATH_MAILDIR "/var/mail"
#endif
#ifndef _PATH_PAGER
#define _PATH_PAGER "/usr/bin/more"
#endif
#ifndef _PATH_LOCALE
#define _PATH_LOCALE "/usr/share/locale"
#endif
#ifndef _PATH_WORDS
#define _PATH_WORDS "/usr/share/dict/words"
#endif

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_PATHS_H */
