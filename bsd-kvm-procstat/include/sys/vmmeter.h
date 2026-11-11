#ifndef SYS_VMMETER_H
#define SYS_VMMETER_H

/**
 * @file vmmeter.h
 * @brief FreeBSD virtual memory meter (<sys/vmmeter.h>) structure.
 */

/* clang-format off */
#include <sys/types.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief System-wide virtual memory and paging statistics. */
struct vmmeter {
  unsigned int v_swtch;
  unsigned int v_trap;
  unsigned int v_syscall;
  unsigned int v_intr;
  unsigned int v_soft;
  unsigned int v_vm_faults;
  unsigned int v_io_faults;
  unsigned int v_cow_faults;
  unsigned int v_cow_optim;
  unsigned int v_zfod;
  unsigned int v_ozfod;
  unsigned int v_swapin;
  unsigned int v_swapout;
  unsigned int v_swappgsin;
  unsigned int v_swappgsout;
  unsigned int v_vnodein;
  unsigned int v_vnodeout;
  unsigned int v_vnodepgsin;
  unsigned int v_vnodepgsout;
  unsigned int v_intrans;
  unsigned int v_reactivated;
  unsigned int v_pdwakeups;
  unsigned int v_pdpages;
  unsigned int v_pdshortfalls;
  unsigned int v_dfree;
  unsigned int v_pfree;
  unsigned int v_tfree;
  unsigned int v_page_size;
  unsigned int v_page_count;
  unsigned int v_free_reserved;
  unsigned int v_free_target;
  unsigned int v_free_min;
  unsigned int v_free_count;
  unsigned int v_wire_count;
  unsigned int v_active_count;
  unsigned int v_inactive_target;
  unsigned int v_inactive_count;
  unsigned int v_laundry_count;
  unsigned int v_Forks;
  unsigned int v_vforks;
  unsigned int v_rforks;
  unsigned int v_kthreads;
  unsigned int v_forkpages;
  unsigned int v_vforkpages;
  unsigned int v_rforkpages;
  unsigned int v_kthreadpages;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_VMMETER_H */
