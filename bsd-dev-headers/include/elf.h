#ifndef BSD_ELF_H
#define BSD_ELF_H

/**
 * @file elf.h
 * @brief Standard ELF32 and ELF64 structures (<elf.h>) polyfill.
 */

/* clang-format off */
#include <sys/elf_common.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SYS_ELF32_H_
#define _SYS_ELF32_H_ 1
#ifndef _SYS_ELF64_H_
#define _SYS_ELF64_H_ 1
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef int32_t Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct {
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
  Elf64_Word sh_name;
  Elf64_Word sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr sh_addr;
  Elf64_Off sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word sh_link;
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
} Elf64_Shdr;

typedef struct {
  Elf32_Word p_type;
  Elf32_Off p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  Elf32_Word p_filesz;
  Elf32_Word p_memsz;
  Elf32_Word p_flags;
  Elf32_Word p_align;
} Elf32_Phdr;

typedef struct {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;

typedef struct {
  Elf32_Word st_name;
  Elf32_Addr st_value;
  Elf32_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct {
  Elf64_Word st_name;
  unsigned char st_info;
  unsigned char st_other;
  Elf64_Half st_shndx;
  Elf64_Addr st_value;
  Elf64_Xword st_size;
} Elf64_Sym;

typedef Elf_Note Elf32_Nhdr;
typedef Elf_Note Elf64_Nhdr;

typedef struct {
  Elf32_Sword d_tag;
  union {
    Elf32_Word d_val;
    Elf32_Addr d_ptr;
  } d_un;
} Elf32_Dyn;

typedef struct {
  Elf64_Sxword d_tag;
  union {
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  } d_un;
} Elf64_Dyn;

typedef struct {
  Elf32_Addr r_offset;
  Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
  Elf32_Addr r_offset;
  Elf32_Word r_info;
  Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct {
  Elf64_Addr r_offset;
  Elf64_Xword r_info;
} Elf64_Rel;

typedef struct {
  Elf64_Addr r_offset;
  Elf64_Xword r_info;
  Elf64_Sxword r_addend;
} Elf64_Rela;

typedef struct {
  Elf32_Word ch_type;
  Elf32_Word ch_size;
  Elf32_Word ch_addralign;
} Elf32_Chdr;

typedef struct {
  Elf64_Word ch_type;
  Elf64_Word ch_reserved;
  Elf64_Xword ch_size;
  Elf64_Xword ch_addralign;
} Elf64_Chdr;

typedef struct {
  Elf32_Word c_tag;
  union {
    Elf32_Word c_val;
    Elf32_Addr c_ptr;
  } c_un;
} Elf32_Cap;

typedef struct {
  Elf64_Xword c_tag;
  union {
    Elf64_Xword c_val;
    Elf64_Addr c_ptr;
  } c_un;
} Elf64_Cap;

typedef struct {
  Elf64_Xword m_value;
  Elf32_Word m_info;
  Elf32_Word m_poffset;
  Elf32_Half m_repeat;
  Elf32_Half m_stride;
} Elf32_Move;

typedef struct {
  Elf64_Xword m_value;
  Elf64_Xword m_info;
  Elf64_Xword m_poffset;
  Elf64_Half m_repeat;
  Elf64_Half m_stride;
} Elf64_Move;

typedef struct {
  Elf32_Half si_boundto;
  Elf32_Half si_flags;
} Elf32_Syminfo;

typedef struct {
  Elf64_Half si_boundto;
  Elf64_Half si_flags;
} Elf64_Syminfo;

#ifndef IS_ELF
#define IS_ELF(ehdr)                                                           \
  ((ehdr).e_ident[0] == 0x7f && (ehdr).e_ident[1] == 'E' &&                    \
   (ehdr).e_ident[2] == 'L' && (ehdr).e_ident[3] == 'F')
#endif

#endif /* !_SYS_ELF64_H_ */
#endif /* !_SYS_ELF32_H_ */

#ifndef ELF_ST_BIND
#define ELF_ST_BIND(val) (((unsigned char)(val)) >> 4)
#define ELF_ST_TYPE(val) ((val) & 0xf)
#define ELF_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))
#endif

#if !defined(FBSD_ELF_GENERIC_TYPEDEFS_DEFINED)
#define FBSD_ELF_GENERIC_TYPEDEFS_DEFINED 1
#if defined(__ELF_WORD_SIZE) && (__ELF_WORD_SIZE == 32)
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Half Elf_Half;
typedef Elf32_Off Elf_Off;
typedef Elf32_Sword Elf_Sword;
typedef Elf32_Word Elf_Word;
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Dyn Elf_Dyn;
typedef Elf32_Rel Elf_Rel;
typedef Elf32_Rela Elf_Rela;
typedef Elf32_Sym Elf_Sym;
#else
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Half Elf_Half;
typedef Elf64_Off Elf_Off;
typedef Elf64_Sword Elf_Sword;
typedef Elf64_Sxword Elf_Sxword;
typedef Elf64_Word Elf_Word;
typedef Elf64_Xword Elf_Xword;
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Dyn Elf_Dyn;
typedef Elf64_Rel Elf_Rel;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Sym Elf_Sym;
#endif
#endif

typedef struct {
  int a_type;
  union {
    long a_val;
  } a_un;
} Elf32_Auxinfo;

typedef struct {
  long a_type;
  union {
    long a_val;
    void *a_ptr;
    void (*a_fcn)(void);
  } a_un;
} Elf64_Auxinfo;

#if defined(__ELF_WORD_SIZE) && (__ELF_WORD_SIZE == 32)
typedef Elf32_Auxinfo Elf_Auxinfo;
#else
typedef Elf64_Auxinfo Elf_Auxinfo;
#endif

#ifndef AT_NULL
#define AT_NULL 0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_PAGESZ 6
#define AT_BASE 7
#define AT_FLAGS 8
#define AT_ENTRY 9
#define AT_NOTELF 10
#define AT_UID 11
#define AT_EUID 12
#define AT_GID 13
#define AT_EGID 14
#define AT_EXECPATH 15
#define AT_CANARY 16
#define AT_CANARYLEN 17
#define AT_OSRELDATE 18
#define AT_NCPUS 19
#define AT_PAGESIZES 20
#define AT_PAGESIZESLEN 21
#define AT_TIMEKEEP 22
#define AT_STACKPROT 23
#define AT_EHDRFLAGS 24
#define AT_HWCAP 25
#define AT_HWCAP2 26
#define AT_BSDFLAGS 27
#define AT_ARGC 28
#define AT_ARGV 29
#define AT_ENVC 30
#define AT_ENVV 31
#define AT_PS_STRINGS 32
#define AT_FXRNG 33
#define AT_KPRELOAD 34
#define AT_USRSTACKBASE 35
#define AT_USRSTACKLIM 36
#define AT_COUNT 37
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_ELF_H */
