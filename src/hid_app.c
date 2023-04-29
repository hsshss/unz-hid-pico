#include <hardware/gpio.h>
#include "tusb.h"
#include "config.h"

static void hid_app_init_mouse();
static void strobe_irq_callback(uint gpio, uint32_t event_mask);

static int mouse_x = 0;
static int mouse_y = 0;
static uint64_t before_strobe = 0;
static uint16_t mouse_data = 0;
static int mouse_data_pos = 0;

void hid_app_init() {
  hid_app_init_mouse();
}

static void hid_app_init_mouse() {
  gpio_init(MOUSE_COM_PIN);
  gpio_set_dir(MOUSE_COM_PIN, GPIO_IN);
  gpio_pull_up(MOUSE_COM_PIN);
  gpio_set_input_hysteresis_enabled(MOUSE_COM_PIN, true);

  for (int i = 0; i < 4; i++) {
    gpio_init(MOUSE_DATA_PIN_BASE + i);
    gpio_set_dir(MOUSE_DATA_PIN_BASE + i, GPIO_OUT);
  }

  gpio_init(MOUSE_TRIG1_PIN);
  gpio_set_dir(MOUSE_TRIG1_PIN, GPIO_OUT);

  gpio_init(MOUSE_TRIG2_PIN);
  gpio_set_dir(MOUSE_TRIG2_PIN, GPIO_OUT);

  gpio_set_irq_enabled_with_callback(
      MOUSE_COM_PIN,
      GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
      true,
      strobe_irq_callback);
}

static void strobe_irq_callback(uint gpio, uint32_t event_mask) {
  uint64_t now = time_us_64();
  if (event_mask == GPIO_IRQ_EDGE_FALL && (now - before_strobe) >= 300) {
    // timed out
    mouse_data_pos = 0;
  }
  before_strobe = now;
  
  if (mouse_data_pos == 0) {
    int x = -mouse_x * 200 / MOUSE_DPI;
    int y = -mouse_y * 200 / MOUSE_DPI;
    x = MIN(x, 127);
    x = MAX(x, -128);
    y = MIN(y, 127);
    y = MAX(y, -128);
    mouse_data = ((x & 0xff) << 8) | (y & 0xff);
    mouse_x = 0;
    mouse_y = 0;
  }

  uint16_t data = (mouse_data >> (3 - mouse_data_pos) * 4) & 0xf;
#ifdef MOUSE_DATA_REVERSE
  data = ((data & 0x1) << 3) | ((data & 0x2) << 1) | ((data & 0x4) >> 1) | ((data & 0x8) >> 3);
#endif
  gpio_put_masked(0xf << MOUSE_DATA_PIN_BASE, ~(data << MOUSE_DATA_PIN_BASE));
  mouse_data_pos++;

  if (mouse_data_pos == 4) {
    mouse_data_pos = 0;
  }
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
}

static void process_mouse_report(hid_mouse_report_t const *report) {
  gpio_put(MOUSE_TRIG1_PIN, report->buttons & MOUSE_BUTTON_LEFT);
  gpio_put(MOUSE_TRIG2_PIN, report->buttons & MOUSE_BUTTON_RIGHT);

  uint32_t int_status = save_and_disable_interrupts();
  mouse_x += report->x;
  mouse_y += report->y;
  restore_interrupts(int_status);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch (itf_protocol) {
    case HID_ITF_PROTOCOL_MOUSE:
      process_mouse_report((hid_mouse_report_t const *) report);
      break;
  }

  tuh_hid_receive_report(dev_addr, instance);
}
