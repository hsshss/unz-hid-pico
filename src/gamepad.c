#include <hardware/gpio.h>
#include "tusb.h"
#include "config.h"

typedef enum {
  DSX_DPAD_N,
  DSX_DPAD_NE,
  DSX_DPAD_E,
  DSX_DPAD_SE,
  DSX_DPAD_S,
  DSX_DPAD_SW,
  DSX_DPAD_W,
  DSX_DPAD_NW,
  DSX_DPAD_NONE
} dsx_dpad_enum_t;

typedef struct TU_ATTR_PACKED {
  uint8_t lx;
  uint8_t ly;
  uint8_t rx;
  uint8_t ry;
  uint8_t dpad: 4;
  uint8_t square: 1;
  uint8_t cross: 1;
  uint8_t circle: 1;
  uint8_t triangle: 1;
  uint8_t l1: 1;
  uint8_t r1: 1;
  uint8_t l2: 1;
  uint8_t r2: 1;
  uint8_t share: 1;
  uint8_t option: 1;
  uint8_t l3: 1;
  uint8_t r3: 1;
  uint8_t ps: 1;
  uint8_t tpad: 1;
  uint8_t counter: 6;
} sony_ds4_report_t;

typedef struct TU_ATTR_PACKED {
  uint8_t lx;
  uint8_t ly;
  uint8_t rx;
  uint8_t ry;
  uint8_t l2_analog;
  uint8_t r2_analog;
  uint8_t counter;
  uint8_t dpad: 4;
  uint8_t square: 1;
  uint8_t cross: 1;
  uint8_t circle: 1;
  uint8_t triangle: 1;
  uint8_t l1: 1;
  uint8_t r1: 1;
  uint8_t l2: 1;
  uint8_t r2: 1;
  uint8_t share: 1;
  uint8_t option: 1;
  uint8_t l3: 1;
  uint8_t r3: 1;
  uint8_t ps: 1;
  uint8_t tpad: 1;
  uint8_t : 6;
} sony_ds5_report_t;

static uint32_t gamepad_mask = 0;
static uint32_t gamepad_data = 0;

static void set_gamepad_data(
    bool up, bool down, bool left, bool right,
    bool trig1, bool trig2, bool select, bool run) {
  gamepad_data =
      ((up    || select) ? 1 : 0) << GAMEPAD_UP_PIN |
      ((down  || select) ? 1 : 0) << GAMEPAD_DOWN_PIN |
      ((left  || run   ) ? 1 : 0) << GAMEPAD_LEFT_PIN |
      ((right || run   ) ? 1 : 0) << GAMEPAD_RIGHT_PIN |
      ( trig1            ? 1 : 0) << GAMEPAD_TRIG1_PIN |
      ( trig2            ? 1 : 0) << GAMEPAD_TRIG2_PIN;
}

static void gpio_put_gamepad() {
  gpio_put_masked(gamepad_mask, gamepad_data);
}

void init_gamepad() {
  const uint pins[6] = {
      GAMEPAD_UP_PIN, GAMEPAD_DOWN_PIN,
      GAMEPAD_LEFT_PIN, GAMEPAD_RIGHT_PIN,
      GAMEPAD_TRIG1_PIN, GAMEPAD_TRIG2_PIN
  };

  gpio_init(GAMEPAD_COM_PIN);
  gpio_set_dir(GAMEPAD_COM_PIN, GPIO_IN);
  gpio_pull_up(GAMEPAD_COM_PIN);
  gpio_set_input_hysteresis_enabled(GAMEPAD_COM_PIN, true);

  gamepad_mask = 0;
  gamepad_data = 0;

  for (int i = 0; i < TU_ARRAY_SIZE(pins); i++) {
    gpio_init(pins[i]);
    gpio_set_dir(pins[i], GPIO_OUT);
    gamepad_mask |= 1 << pins[i];
  }

  gpio_put_gamepad();
}

static inline bool is_sony_ds4(uint8_t dev_addr) {
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  return (vid == 0x054c && pid == 0x09cc);
}

static inline bool is_sony_ds5(uint8_t dev_addr) {
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  return (vid == 0x054c && pid == 0x0ce6);
}

static void process_sony_ds4(uint8_t const *report, uint16_t len) {
  uint8_t const report_id = report[0];
  if (report_id != 1) {
    return;
  }

  bool up, down, left, right;
  sony_ds4_report_t const *dsx_report = (sony_ds4_report_t const *) (report + 1);

  switch (dsx_report->dpad) {
    case DSX_DPAD_N:  up = 1; down = 0; left = 0; right = 0; break;
    case DSX_DPAD_NE: up = 1; down = 0; left = 0; right = 1; break;
    case DSX_DPAD_E:  up = 0; down = 0; left = 0; right = 1; break;
    case DSX_DPAD_SE: up = 0; down = 1; left = 0; right = 1; break;
    case DSX_DPAD_S:  up = 0; down = 1; left = 0; right = 0; break;
    case DSX_DPAD_SW: up = 0; down = 1; left = 1; right = 0; break;
    case DSX_DPAD_W:  up = 0; down = 0; left = 1; right = 0; break;
    case DSX_DPAD_NW: up = 1; down = 0; left = 1; right = 0; break;
    default:          up = 0; down = 0; left = 0; right = 0; break;
  }

  set_gamepad_data(
      up    || dsx_report->ly < (128 - GAMEPAD_DEAD_ZONE),
      down  || dsx_report->ly > (128 + GAMEPAD_DEAD_ZONE),
      left  || dsx_report->lx < (128 - GAMEPAD_DEAD_ZONE),
      right || dsx_report->lx > (128 + GAMEPAD_DEAD_ZONE),
      dsx_report->circle || dsx_report->square,
      dsx_report->cross  || dsx_report->triangle,
      dsx_report->share,
      dsx_report->option);

  gpio_put_gamepad();
}

static void process_sony_ds5(uint8_t const *report, uint16_t len) {
  uint8_t const report_id = report[0];
  if (report_id != 1) {
    return;
  }

  bool up, down, left, right;
  sony_ds5_report_t const *dsx_report = (sony_ds5_report_t const *) (report + 1);

  switch (dsx_report->dpad) {
    case DSX_DPAD_N:  up = 1; down = 0; left = 0; right = 0; break;
    case DSX_DPAD_NE: up = 1; down = 0; left = 0; right = 1; break;
    case DSX_DPAD_E:  up = 0; down = 0; left = 0; right = 1; break;
    case DSX_DPAD_SE: up = 0; down = 1; left = 0; right = 1; break;
    case DSX_DPAD_S:  up = 0; down = 1; left = 0; right = 0; break;
    case DSX_DPAD_SW: up = 0; down = 1; left = 1; right = 0; break;
    case DSX_DPAD_W:  up = 0; down = 0; left = 1; right = 0; break;
    case DSX_DPAD_NW: up = 1; down = 0; left = 1; right = 0; break;
    default:          up = 0; down = 0; left = 0; right = 0; break;
  }

  set_gamepad_data(
      up    || dsx_report->ly < (128 - GAMEPAD_DEAD_ZONE),
      down  || dsx_report->ly > (128 + GAMEPAD_DEAD_ZONE),
      left  || dsx_report->lx < (128 - GAMEPAD_DEAD_ZONE),
      right || dsx_report->lx > (128 + GAMEPAD_DEAD_ZONE),
      dsx_report->circle || dsx_report->square,
      dsx_report->cross  || dsx_report->triangle,
      dsx_report->share,
      dsx_report->option);

  gpio_put_gamepad();
}

void process_gamepad_report(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  if (is_sony_ds4(dev_addr)) {
    process_sony_ds4(report, len);
  } else if (is_sony_ds5(dev_addr)) {
    process_sony_ds5(report, len);
  }
}
