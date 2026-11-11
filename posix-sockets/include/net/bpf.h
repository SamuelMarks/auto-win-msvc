#ifndef NET_BPF_H
#define NET_BPF_H

/**
 * @file bpf.h
 * @brief BSD Packet Filter (<net/bpf.h>) definitions.
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

struct bpf_insn {
  unsigned short code;
  unsigned char jt;
  unsigned char jf;
  uint32_t k;
};

struct bpf_program {
  unsigned int bf_len;
  struct bpf_insn *bf_insns;
};

struct bpf_stat {
  unsigned int bs_recv;
  unsigned int bs_drop;
};

struct bpf_version {
  unsigned short bv_major;
  unsigned short bv_minor;
};

#define DLT_NULL 0
#define DLT_EN10MB 1
#define DLT_RAW 12

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NET_BPF_H */
