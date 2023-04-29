#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG                0
#endif

#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN            __attribute__ ((aligned(4)))
#endif

#define CFG_TUH_ENABLED               1

#define CFG_TUH_RPI_PIO_USB           0
#define BOARD_TUH_RHPORT              CFG_TUH_RPI_PIO_USB

#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED           OPT_MODE_DEFAULT_SPEED
#endif

#define CFG_TUH_MAX_SPEED             BOARD_TUH_MAX_SPEED

#define CFG_TUH_HUB                   1
#define CFG_TUH_CDC                   0
#define CFG_TUH_HID                   4
#define CFG_TUH_MSC                   0
#define CFG_TUH_VENDOR                0

#define CFG_TUH_DEVICE_MAX            (CFG_TUH_HUB ? 4 : 1)

#define CFG_TUH_HID_EPIN_BUFSIZE      64
#define CFG_TUH_HID_EPOUT_BUFSIZE     64
#define CFG_TUH_ENUMERATION_BUFSIZE   256

#ifdef __cplusplus
}
#endif

#endif /* TUSB_CONFIG_H */
