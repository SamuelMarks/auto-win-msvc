#ifndef BSD_NLIST_H
#define BSD_NLIST_H

/**
 * @file nlist.h
 * @brief BSD symbol table name list (<nlist.h>) structure.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Symbol table entry for kvm_nlist / nlist. */
struct nlist {
  const char *n_name;
  unsigned long n_value;
  short n_type;
  char n_other;
  short n_desc;
};

#define N_UNDF 0x00
#define N_ABS 0x02
#define N_TEXT 0x04
#define N_DATA 0x06
#define N_BSS 0x08
#define N_EXT 0x01

/** @brief Populate symbol table entries from an executable file. */
int nlist(const char *filename, struct nlist *nl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_NLIST_H */
