#ifndef SYS_MTIO_H
#define SYS_MTIO_H

/**
 * @file mtio.h
 * @brief FreeBSD magnetic tape ioctl structures and commands (<sys/mtio.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/ioccom.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct mtop {
  short mt_op;
  int32_t mt_count;
};

#define MTWEOF 0
#define MTFSF 1
#define MTBSF 2
#define MTFSR 3
#define MTBSR 4
#define MTREW 5
#define MTOFFL 6
#define MTNOP 7
#define MTCACHE 8
#define MTNOCACHE 9
#define MTSETBSIZ 10
#define MTSETDNSTY 11
#define MTERASE 12
#define MTEOD 13
#define MTCOMP 14
#define MTRETENS 15
#define MTWSS 16
#define MTFSS 17
#define MTBSS 18
#define MTLOAD 19
#define MTWEOFI 20

#define MT_COMP_ENABLE 0xffffffffU
#define MT_COMP_DISABLED 0xfffffffeU
#define MT_COMP_UNSUPP 0xfffffffdU

#define MTIO_DSREG_NIL 0
#define MTIO_DSREG_REST 1
#define MTIO_DSREG_RBSY 2
#define MTIO_DSREG_WR 20
#define MTIO_DSREG_FMK 21
#define MTIO_DSREG_ZER 22
#define MTIO_DSREG_RD 30
#define MTIO_DSREG_FWD 40
#define MTIO_DSREG_REV 41
#define MTIO_DSREG_POS 42
#define MTIO_DSREG_REW 43
#define MTIO_DSREG_TEN 44
#define MTIO_DSREG_UNL 45
#define MTIO_DSREG_LD 46

struct mtget {
  short mt_type;
  short mt_dsreg;
  short mt_erreg;
  short mt_resid;
  int32_t mt_blksiz;
  int32_t mt_density;
  uint32_t mt_comp;
  int32_t mt_blksiz0;
  int32_t mt_blksiz1;
  int32_t mt_blksiz2;
  int32_t mt_blksiz3;
  int32_t mt_density0;
  int32_t mt_density1;
  int32_t mt_density2;
  int32_t mt_density3;
  uint32_t mt_comp0;
  uint32_t mt_comp1;
  uint32_t mt_comp2;
  uint32_t mt_comp3;
  int32_t mt_fileno;
  int32_t mt_blkno;
};

struct scsi_tape_errors {
  uint8_t io_sense[32];
  int32_t io_resid;
  uint8_t io_cdb[16];
  uint8_t ctl_sense[32];
  int32_t ctl_resid;
  uint8_t ctl_cdb[16];
  struct {
    uint32_t retries;
    uint32_t corrected;
    uint32_t processed;
    uint32_t failures;
    uint64_t nbytes;
  } wterr, rderr;
};

union mterrstat {
  struct scsi_tape_errors scsi_errstat;
  char _reserved_padding[256];
};

struct mtrblim {
  uint32_t granularity;
  uint32_t min_block_length;
  uint32_t max_block_length;
};

typedef enum {
  MT_LOCATE_DEST_OBJECT = 0x00,
  MT_LOCATE_DEST_FILE = 0x01,
  MT_LOCATE_DEST_SET = 0x02,
  MT_LOCATE_DEST_EOD = 0x03
} mt_locate_dest_type;

typedef enum {
  MT_LOCATE_BAM_IMPLICIT = 0x00,
  MT_LOCATE_BAM_EXPLICIT = 0x01
} mt_locate_bam;

typedef enum {
  MT_LOCATE_FLAG_IMMED = 0x01,
  MT_LOCATE_FLAG_CHANGE_PART = 0x02
} mt_locate_flags;

struct mtlocate {
  mt_locate_flags flags;
  mt_locate_dest_type dest_type;
  mt_locate_bam block_address_mode;
  int64_t partition;
  uint64_t logical_id;
  uint8_t reserved[64];
};

typedef enum {
  MT_EXT_GET_NONE,
  MT_EXT_GET_OK,
  MT_EXT_GET_NEED_MORE_SPACE,
  MT_EXT_GET_ERROR
} mt_ext_get_status;

struct mtextget {
  uint32_t alloc_len;
  char *status_xml;
  uint32_t fill_len;
  mt_ext_get_status status;
  char error_str[128];
  uint8_t reserved[64];
};

#define MT_EXT_GET_ROOT_NAME "mtextget"
#define MT_DENSITY_ROOT_NAME "mtdensity"
#define MT_MEDIA_DENSITY_NAME "media_density"
#define MT_DENSITY_REPORT_NAME "density_report"
#define MT_MEDIUM_TYPE_REPORT_NAME "medium_type_report"
#define MT_MEDIA_REPORT_NAME "media_report"
#define MT_DENSITY_ENTRY_NAME "density_entry"

#define MT_DENS_WRITE_OK 0x80
#define MT_DENS_DUP 0x40
#define MT_DENS_DEFLT 0x20

#define MT_PARAM_FIXED_STR_LEN 32
union mt_param_value {
  int64_t value_signed;
  uint64_t value_unsigned;
  char *value_var_str;
  char value_fixed_str[MT_PARAM_FIXED_STR_LEN];
  uint8_t reserved[64];
};

typedef enum {
  MT_PARAM_SET_NONE,
  MT_PARAM_SET_SIGNED,
  MT_PARAM_SET_UNSIGNED,
  MT_PARAM_SET_VAR_STR,
  MT_PARAM_SET_FIXED_STR
} mt_param_set_type;

typedef enum {
  MT_PARAM_STATUS_NONE,
  MT_PARAM_STATUS_OK,
  MT_PARAM_STATUS_ERROR
} mt_param_set_status;

#define MT_PARAM_VALUE_NAME_LEN 64
struct mtparamset {
  char value_name[MT_PARAM_VALUE_NAME_LEN];
  mt_param_set_type value_type;
  int value_len;
  union mt_param_value value;
  mt_param_set_status status;
  char error_str[128];
};

#define MT_PARAM_ROOT_NAME "mtparamget"
#define MT_PROTECTION_NAME "protection"

struct mtsetlist {
  int num_params;
  int param_len;
  struct mtparamset *params;
};

#define MT_ISTS 0x01
#define MT_ISHT 0x02
#define MT_ISTM 0x03
#define MT_ISMT 0x04
#define MT_ISUT 0x05
#define MT_ISCPC 0x06
#define MT_ISAR 0x07
#define MT_ISTMSCP 0x08
#define MT_ISCY 0x09
#define MT_ISCT 0x0a
#define MT_ISFHP 0x0b
#define MT_ISEXABYTE 0x0c
#define MT_ISEXA8200 0x0c
#define MT_ISEXA8500 0x0d
#define MT_ISVIPER1 0x0e
#define MT_ISPYTHON 0x0f
#define MT_ISHPDAT 0x10
#define MT_ISMFOUR 0x11
#define MT_ISTK50 0x12
#define MT_ISMT02 0x13

#define MTIOCTOP _IOW('m', 1, struct mtop)
#define MTIOCGET _IOR('m', 2, struct mtget)
#define MTIOCIEOT _IO('m', 3)
#define MTIOCEEOT _IO('m', 4)
#define MTIOCRDSPOS _IOR('m', 5, uint32_t)
#define MTIOCRDHPOS _IOR('m', 6, uint32_t)
#define MTIOCSLOCATE _IOW('m', 5, uint32_t)
#define MTIOCHLOCATE _IOW('m', 6, uint32_t)
#define MTIOCERRSTAT _IOR('m', 7, union mterrstat)
#define MTIOCSETEOTMODEL _IOW('m', 8, uint32_t)
#define MTIOCGETEOTMODEL _IOR('m', 8, uint32_t)
#define MTIOCRBLIM _IOR('m', 9, struct mtrblim)
#define MTIOCEXTLOCATE _IOW('m', 10, struct mtlocate)
#define MTIOCEXTGET _IOWR('m', 11, struct mtextget)
#define MTIOCPARAMGET _IOWR('m', 12, struct mtextget)
#define MTIOCPARAMSET _IOWR('m', 13, struct mtparamset)
#define MTIOCSETLIST _IOWR('m', 14, struct mtsetlist)

#ifndef DEFTAPE
#define DEFTAPE "/dev/nsa0"
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_MTIO_H */
