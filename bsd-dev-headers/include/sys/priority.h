#ifndef SYS_PRIORITY_H
#define SYS_PRIORITY_H

/**
 * @file priority.h
 * @brief FreeBSD scheduler priority class definitions (<sys/priority.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PRI_ITHD 1
#define PRI_REALTIME 2
#define PRI_TIMESHARE 3
#define PRI_IDLE 4
#define PRI_FIFO_BIT 8
#define PRI_FIFO (PRI_FIFO_BIT | PRI_REALTIME)
#define PRI_BASE(P) ((P) & ~PRI_FIFO_BIT)
#define PRI_IS_REALTIME(P) (PRI_BASE(P) == PRI_REALTIME)
#define PRI_NEED_RR(P) ((P) != PRI_FIFO)
#define PRI_MIN 0
#define PRI_MAX 255
#define PRI_MIN_ITHD 0
#define PRI_MIN_REALTIME 8
#define PRI_MIN_KERN 40
#define PZERO (PRI_MIN_KERN + 5)
#define PRI_MIN_TIMESHARE 56
#define PUSER PRI_MIN_TIMESHARE
#define PRI_MIN_IDLE 224

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PRIORITY_H */
