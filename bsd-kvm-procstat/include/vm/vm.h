#ifndef VM_VM_H
#define VM_VM_H

/**
 * @file vm.h
 * @brief FreeBSD virtual memory protection flags (<vm/vm.h>).
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

typedef uint8_t vm_prot_t;

#ifndef VM_PROT_NONE
#define VM_PROT_NONE ((vm_prot_t)0x00)
#define VM_PROT_READ ((vm_prot_t)0x01)
#define VM_PROT_WRITE ((vm_prot_t)0x02)
#define VM_PROT_EXECUTE ((vm_prot_t)0x04)
#define VM_PROT_COPY ((vm_prot_t)0x08)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VM_VM_H */
