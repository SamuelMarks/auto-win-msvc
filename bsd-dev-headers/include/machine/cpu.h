#ifndef MACHINE_CPU_H
#define MACHINE_CPU_H

/**
 * @file cpu.h
 * @brief FreeBSD <machine/cpu.h> compatibility header.
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACHINE_CPU_H */
