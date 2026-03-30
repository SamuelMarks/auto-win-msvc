/* rdma/rdma_cma.h - Strict C89 Header */
#ifndef RDMA_RDMA_CMA_H
#define RDMA_RDMA_CMA_H

#ifdef __cplusplus
extern "C" {
#endif

struct rdma_cm_event {
  int event;
};
struct rdma_cm_id {
  void *context;
};
struct sockaddr;

int rdma_create_event_channel(void);
int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr);
int rdma_listen(struct rdma_cm_id *id, int backlog);
int rdma_accept(struct rdma_cm_id *id, void *conn_param);

#ifdef __cplusplus
}
#endif

#endif /* RDMA_RDMA_CMA_H */
