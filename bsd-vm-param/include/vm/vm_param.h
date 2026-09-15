#ifndef VM_VM_PARAM_H
#define VM_VM_PARAM_H

/**
 * @file vm_param.h
 * @brief Polyfill for BSD <vm/vm_param.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HOST_VM_INFO
/** @brief Host VM info constant. */
#define HOST_VM_INFO 2
#endif

/**
 * @brief Error codes returned by bsd-vm-param functions.
 */
enum bsd_vm_param_error_code {
  /** @brief Successful operation. */
  BSD_VM_PARAM_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_VM_PARAM_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the HOST_VM_INFO constant.
 * @param[out] out_info Pointer to an int that receives the host vm info value.
 * @return BSD_VM_PARAM_SUCCESS on success, or an error code on failure.
 */
enum bsd_vm_param_error_code bsd_vm_param_get_host_vm_info(int *out_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VM_VM_PARAM_H */
