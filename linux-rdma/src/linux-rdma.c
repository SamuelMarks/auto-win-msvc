/* clang-format off */
#include "rdma/rdma_cma.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#ifndef ENOSYS
#define ENOSYS 38
#endif

/** @brief Polyfill for rdma_create_event_channel. */
int rdma_create_event_channel(void) { return 0; }

/** @brief Polyfill for rdma_bind_addr. */
int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr) {
  (void)id;
  (void)addr;
  errno = ENOSYS;
  return -1;
}

/** @brief Polyfill for rdma_listen. */
int rdma_listen(struct rdma_cm_id *id, int backlog) {
  (void)id;
  (void)backlog;
  errno = ENOSYS;
  return -1;
}

/** @brief Polyfill for rdma_accept. */
int rdma_accept(struct rdma_cm_id *id, void *conn_param) {
  (void)id;
  (void)conn_param;
  errno = ENOSYS;
  return -1;
}

/** @brief Initializes and validates the linux-rdma module. */
enum linux_rdma_error_code linux_rdma_init(int *out_status) {
  if (out_status == NULL) {
    return LINUX_RDMA_ERROR_NULL_POINTER;
  }
  *out_status = 1;
  return LINUX_RDMA_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_linux_rdma;
