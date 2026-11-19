#ifndef SYS_FONT_H
#define SYS_FONT_H

/**
 * @file font.h
 * @brief FreeBSD vt(4) console font definitions (<sys/font.h>).
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
#include <sys/cdefs.h>
#include <sys/queue.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum vfnt_map_type {
  VFNT_MAP_NORMAL = 0,
  VFNT_MAP_NORMAL_RIGHT,
  VFNT_MAP_BOLD,
  VFNT_MAP_BOLD_RIGHT,
  VFNT_MAPS
};

struct font_info {
  int32_t fi_checksum;
  uint32_t fi_width;
  uint32_t fi_height;
  uint32_t fi_bitmap_size;
  uint32_t fi_map_count[VFNT_MAPS];
};

#if defined(_MSC_VER) || defined(__WATCOMC__)
#pragma pack(push, 1)
#endif
struct vfnt_map {
  uint32_t vfm_src;
  uint16_t vfm_dst;
  uint16_t vfm_len;
} __packed;
#if defined(_MSC_VER) || defined(__WATCOMC__)
#pragma pack(pop)
#endif
typedef struct vfnt_map vfnt_map_t;

struct vt_font {
  vfnt_map_t *vf_map[VFNT_MAPS];
  uint8_t *vf_bytes;
  uint32_t vf_height;
  uint32_t vf_width;
  uint32_t vf_map_count[VFNT_MAPS];
  uint32_t vf_refcount;
};

typedef struct vt_font_bitmap_data {
  uint32_t vfbd_width;
  uint32_t vfbd_height;
  uint32_t vfbd_compressed_size;
  uint32_t vfbd_uncompressed_size;
  uint8_t *vfbd_compressed_data;
  struct vt_font *vfbd_font;
} vt_font_bitmap_data_t;

typedef enum { FONT_AUTO, FONT_MANUAL, FONT_BUILTIN, FONT_RELOAD } FONT_FLAGS;

struct fontlist {
  char *font_name;
  FONT_FLAGS font_flags;
  vt_font_bitmap_data_t *font_data;
  vt_font_bitmap_data_t *(*font_load)(char *);
  STAILQ_ENTRY(fontlist) font_next;
};

typedef STAILQ_HEAD(font_list, fontlist) font_list_t;

#define FONT_HEADER_MAGIC "VFNT0002"
#if defined(_MSC_VER) || defined(__WATCOMC__)
#pragma pack(push, 1)
#endif
struct font_header {
  uint8_t fh_magic[8];
  uint8_t fh_width;
  uint8_t fh_height;
  uint16_t fh_pad;
  uint32_t fh_glyph_count;
  uint32_t fh_map_count[VFNT_MAPS];
} __packed;
#if defined(_MSC_VER) || defined(__WATCOMC__)
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_FONT_H */
