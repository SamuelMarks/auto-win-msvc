#ifndef BSD_USBHID_H
#define BSD_USBHID_H

/**
 * @file usbhid.h
 * @brief BSD USB Human Interface Device (<usbhid.h>) descriptor and data API.
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct report_desc *report_desc_t;
typedef struct hid_data *hid_data_t;

typedef enum hid_kind {
  hid_input = 0,
  hid_output = 1,
  hid_feature = 2,
  hid_collection = 3,
  hid_endcollection = 4
} hid_kind_t;

#define NO_REPORT_ID 0
#define HID_PAGE(u) ((((uint32_t)(u)) >> 16) & 0xffffU)
#define HID_USAGE(u) ((u) & 0xffffU)
#define HID_HAS_GET_SET_REPORT 1

#if defined(__I86__)
typedef unsigned long awm_hid_usage_t;
typedef long awm_hid_susage_t;
#else
typedef unsigned int awm_hid_usage_t;
typedef int awm_hid_susage_t;
#endif

typedef struct hid_item {
  uint32_t _usage_page;
  int32_t logical_minimum;
  int32_t logical_maximum;
  int32_t physical_minimum;
  int32_t physical_maximum;
  int32_t unit_exponent;
  int32_t unit;
  int32_t report_size;
  int32_t report_ID;
  int32_t report_count;
  uint32_t usage;
  int32_t usage_minimum;
  int32_t usage_maximum;
  int32_t designator_index;
  int32_t designator_minimum;
  int32_t designator_maximum;
  int32_t string_index;
  int32_t string_minimum;
  int32_t string_maximum;
  int32_t set_delimiter;
  int32_t collection;
  int collevel;
  enum hid_kind kind;
  uint32_t flags;
  uint32_t pos;
  struct hid_item *next;
} hid_item_t;

/** @brief Obtain USB HID report descriptor from an open device descriptor. */
report_desc_t hid_get_report_desc(int file);
/** @brief Wrap raw USB HID report descriptor bytes in a report_desc_t. */
report_desc_t hid_use_report_desc(unsigned char *data, unsigned int size);
/** @brief Free a USB HID report descriptor handle. */
void hid_dispose_report_desc(report_desc_t d);
/** @brief Query the active USB HID report ID on a device descriptor. */
int hid_get_report_id(int file);
/** @brief Enable or disable immediate USB HID read mode on fd. */
int hid_set_immed(int fd, int enable);
/** @brief Start iterating items in a USB HID report descriptor. */
hid_data_t hid_start_parse(report_desc_t d, int kindset, int id);
/** @brief End iteration over a USB HID report descriptor. */
void hid_end_parse(hid_data_t s);
/** @brief Fetch next item from an active USB HID parser state. */
int hid_get_item(hid_data_t s, hid_item_t *h);
/** @brief Compute total size in bytes of a USB HID report. */
int hid_report_size(report_desc_t d, enum hid_kind k, int id);
/** @brief Locate a specific usage item inside a USB HID report descriptor. */
int hid_locate(report_desc_t d, unsigned int usage, enum hid_kind k,
               hid_item_t *h, int id);
/** @brief Format a USB HID usage page number as a human-readable string. */
const char *hid_usage_page(int i);
/** @brief Format a combined USB HID usage code as a human-readable string. */
const char *hid_usage_in_page(awm_hid_usage_t u);
/** @brief Initialize USB HID usage table from an optional table path. */
void hid_init(const char *file);
/** @brief Parse a symbolic usage-in-page name into a 32-bit usage value. */
awm_hid_susage_t hid_parse_usage_in_page(const char *name);
/** @brief Parse a symbolic usage page name into a usage page number. */
int hid_parse_usage_page(const char *name);
/** @brief Extract an integer field value from a raw USB HID report buffer. */
int32_t hid_get_data(const void *p, const hid_item_t *h);
/** @brief Encode an integer field value into a raw USB HID report buffer. */
void hid_set_data(void *p, const hid_item_t *h, int32_t data);
/** @brief Read a USB HID report from an open device descriptor. */
int hid_get_report(int fd, enum hid_kind k, unsigned char *data,
                   unsigned int size);
/** @brief Send a USB HID report to an open device descriptor. */
int hid_set_report(int fd, enum hid_kind k, unsigned char *data,
                   unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_USBHID_H */
