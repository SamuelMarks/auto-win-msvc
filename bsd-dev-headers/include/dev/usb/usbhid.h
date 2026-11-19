#ifndef BSD_DEV_USB_USBHID_H
#define BSD_DEV_USB_USBHID_H

/**
 * @file usbhid.h
 * @brief FreeBSD USB HID item flag definitions (<dev/usb/usbhid.h>).
 */

/* clang-format off */
#include <sys/cdefs.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define HIO_CONST 0x001
#define HIO_VARIABLE 0x002
#define HIO_RELATIVE 0x004
#define HIO_WRAP 0x008
#define HIO_NONLINEAR 0x010
#define HIO_NOPREF 0x020
#define HIO_NULLSTATE 0x040
#define HIO_VOLATILE 0x080
#define HIO_BUFFERED 0x100

#define UHID_INPUT_REPORT 0x01
#define UHID_OUTPUT_REPORT 0x02
#define UHID_FEATURE_REPORT 0x03

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_DEV_USB_USBHID_H */
