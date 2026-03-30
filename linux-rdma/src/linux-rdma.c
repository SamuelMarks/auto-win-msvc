/* linux-rdma.c - Strict C89 Implementation */

/* clang-format off */
#include "rdma/rdma_cma.h"
/* clang-format on */

/** \brief Polyfill for rdma_create_event_channel
 * \return 0 or
 * appropriate default value
 */
int rdma_create_event_channel(void) { return 0; }

/** \brief Polyfill for rdma_bind_addr
 * \return 0 or appropriate
 * default value
 */
int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr) {
  (void)id;
  (void)addr;
  return 0;
}

/** \brief Polyfill for rdma_listen
 * \return 0 or appropriate default
 * value

 */
int rdma_listen(struct rdma_cm_id *id, int backlog) {
  (void)id;
  (void)backlog;
  return 0;
}

/** \brief Polyfill for rdma_accept
 * \return 0 or appropriate default
 * value

 */
int rdma_accept(struct rdma_cm_id *id, void *conn_param) {
  (void)id;
  (void)conn_param;
  return 0;
}
