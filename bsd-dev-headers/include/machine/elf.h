#ifndef MACHINE_ELF_H
#define MACHINE_ELF_H

/**
 * @file elf.h
 * @brief FreeBSD <machine/elf.h> compatibility definitions.
 */

/* clang-format off */
#include <elf.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ELF_WORD_SIZE
#if defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__) ||          \
    defined(_M_X64) || defined(_M_ARM64)
#define __ELF_WORD_SIZE 64
#else
#define __ELF_WORD_SIZE 32
#endif
#endif

#ifndef EM_ARCH
#define EM_ARCH EM_X86_64
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MACHINE_ELF_H */
