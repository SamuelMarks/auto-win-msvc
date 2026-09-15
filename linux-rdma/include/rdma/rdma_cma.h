#ifndef RDMA_RDMA_CMA_H
#define RDMA_RDMA_CMA_H

/**
 * @file rdma/rdma_cma.h
 * @brief Polyfill for Linux librdmacm <rdma/rdma_cma.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-rdma functions.
 */
enum linux_rdma_error_code {
  /** @brief Successful operation. */
  LINUX_RDMA_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_RDMA_ERROR_NULL_POINTER = 1,
  /** @brief Operation not supported on this platform. */
  LINUX_RDMA_ERROR_NOT_SUPPORTED = 2
};

/**
 * @brief Initializes and validates the linux-rdma module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_RDMA_SUCCESS on success, or an error code on failure.
 */
enum linux_rdma_error_code linux_rdma_init(int *out_status);

/**
 * @brief Structure describing an RDMA communication event.
 */
struct rdma_cm_event {
  /** @brief Event identifier. */
  int event;
};

/**
 * @brief Structure representing an RDMA communication identifier.
 */
struct rdma_cm_id {
  /** @brief Context pointer. */
  void *context;
};

struct sockaddr;

/**
 * @brief Creates an event channel used to report RDMA events.
 * @return 0 on success, or -1 on failure with errno set.
 */
int rdma_create_event_channel(void);

/**
 * @brief Binds an RDMA communication identifier to a local address.
 * @param id Communication identifier.
 * @param addr Local sockaddr address.
 * @return 0 on success, or -1 on failure with errno set.
 */
int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr);

/**
 * @brief Listens for incoming connection requests.
 * @param id Communication identifier.
 * @param backlog Backlog queue size.
 * @return 0 on success, or -1 on failure with errno set.
 */
int rdma_listen(struct rdma_cm_id *id, int backlog);

/**
 * @brief Accepts a connection request.
 * @param id Communication identifier.
 * @param conn_param Connection parameters pointer.
 * @return 0 on success, or -1 on failure with errno set.
 */
int rdma_accept(struct rdma_cm_id *id, void *conn_param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RDMA_RDMA_CMA_H */
