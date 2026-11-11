#ifndef NETINET_TCP_FSM_H
#define NETINET_TCP_FSM_H

/**
 * @file tcp_fsm.h
 * @brief BSD TCP finite state machine constants (<netinet/tcp_fsm.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

#define TCPS_CLOSED 0
#define TCPS_LISTEN 1
#define TCPS_SYN_SENT 2
#define TCPS_SYN_RECEIVED 3
#define TCPS_ESTABLISHED 4
#define TCPS_CLOSE_WAIT 5
#define TCPS_FIN_WAIT_1 6
#define TCPS_CLOSING 7
#define TCPS_LAST_ACK 8
#define TCPS_FIN_WAIT_2 9
#define TCPS_TIME_WAIT 10

#define TCP_NSTATES 11

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETINET_TCP_FSM_H */
