#ifndef BSD_SYS_ELF_COMMON_H
#define BSD_SYS_ELF_COMMON_H

/**
 * @file elf_common.h
 * @brief Common ELF constants, machine IDs, and section/program types.
 */

/* clang-format off */
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFMAG "\177ELF"
#define SELFMAG 4

#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ELFOSABI_NONE 0
#define ELFOSABI_SYSV 0
#define ELFOSABI_LINUX 3
#define ELFOSABI_FREEBSD 9
#define ELFOSABI_STANDALONE 255

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4

#define EM_NONE 0
#define EM_386 3
#define EM_PPC 20
#define EM_PPC64 21
#define EM_ARM 40
#define EM_X86_64 62
#define EM_AMD64 EM_X86_64
#define EM_AARCH64 183
#define EM_RISCV 243

#define EV_NONE 0
#define EV_CURRENT 1

#define SHN_UNDEF 0
#define SHN_ABS 0xfff1
#define SHN_COMMON 0xfff2
#define SHN_XINDEX 0xffff

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_DYNSYM 11
#define SHT_GNU_VERDEF 0x6ffffffdU
#define SHT_GNU_VERNEED 0x6ffffffeU
#define SHT_GNU_VERSYM 0x6fffffffU

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MERGE 0x10
#define SHF_STRINGS 0x20
#define SHF_INFO_LINK 0x40
#define SHF_GROUP 0x200
#define SHF_TLS 0x400
#define SHF_COMPRESSED 0x800

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7
#define PT_GNU_EH_FRAME 0x6474e550U
#define PT_GNU_STACK 0x6474e551U
#define PT_GNU_RELRO 0x6474e552U

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_COMMON 5
#define STT_TLS 6
#define STT_GNU_IFUNC 10

#define STV_DEFAULT 0
#define STV_INTERNAL 1
#define STV_HIDDEN 2
#define STV_PROTECTED 3

#define ELF32_ST_BIND(val) (((unsigned char)(val)) >> 4)
#define ELF32_ST_TYPE(val) ((val) & 0xf)
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))
#define ELF64_ST_BIND(val) ELF32_ST_BIND(val)
#define ELF64_ST_TYPE(val) ELF32_ST_TYPE(val)
#define ELF64_ST_INFO(bind, type) ELF32_ST_INFO(bind, type)
#define ELF32_ST_VISIBILITY(o) ((o) & 0x03)
#define ELF64_ST_VISIBILITY(o) ELF32_ST_VISIBILITY(o)

#define NT_FREEBSD_ABI_TAG 1
#define NT_FREEBSD_NOINIT_TAG 2
#define NT_FREEBSD_ARCH_TAG 3
#define NT_FREEBSD_FEATURE_CTL 4

#define NT_FREEBSD_FCTL_ASLR_DISABLE 0x00000001U
#define NT_FREEBSD_FCTL_PROTMAX_DISABLE 0x00000002U
#define NT_FREEBSD_FCTL_STKGAP_DISABLE 0x00000004U
#define NT_FREEBSD_FCTL_WXNEEDED 0x00000008U
#define NT_FREEBSD_FCTL_LA48 0x00000010U
#define NT_FREEBSD_FCTL_LA57 0x00000040U

typedef struct {
  uint32_t n_namesz;
  uint32_t n_descsz;
  uint32_t n_type;
} Elf_Note;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_ELF_COMMON_H */
