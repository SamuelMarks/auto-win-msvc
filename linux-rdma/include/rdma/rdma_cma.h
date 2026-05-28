
#ifdef __cplusplus
extern "C" {
#endif

/* rdma/rdma_cma.h - Strict C89 Header */
#ifndef RDMA_RDMA_CMA_H
#define RDMA_RDMA_CMA_H
#include "auto_win_msvc_export.h"

struct rdma_cm_event {
  int event;
};
struct rdma_cm_id {
  void *context;
};
struct sockaddr;

AUTO_WIN_MSVC_EXPORT int rdma_create_event_channel(void);
AUTO_WIN_MSVC_EXPORT int rdma_bind_addr(struct rdma_cm_id *id,
                                        struct sockaddr *addr);
AUTO_WIN_MSVC_EXPORT int rdma_listen(struct rdma_cm_id *id, int backlog);
AUTO_WIN_MSVC_EXPORT int rdma_accept(struct rdma_cm_id *id, void *conn_param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RDMA_RDMA_CMA_H */
