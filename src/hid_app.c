#include "tusb.h"
#include "config.h"

void init_mouse();
void process_mouse_report(hid_mouse_report_t const *report);

void init_gamepad();
void process_gamepad_report(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len);

void hid_app_init() {
#if MOUSE_ENABLED
  init_mouse();
#endif
#if GAMEPAD_ENABLED
  init_gamepad();
#endif
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch (itf_protocol) {
    case HID_ITF_PROTOCOL_MOUSE:
#if MOUSE_ENABLED
      process_mouse_report((hid_mouse_report_t const *) report);
#endif
      break;

    case HID_ITF_PROTOCOL_NONE:
#if GAMEPAD_ENABLED
      process_gamepad_report(dev_addr, instance, report, len);
#endif
      break;
  }

  tuh_hid_receive_report(dev_addr, instance);
}
