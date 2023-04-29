#include "bsp/board.h"
#include "tusb.h"

void hid_app_init();

int main(void) {
  board_init();
  hid_app_init();
  tuh_init(BOARD_TUH_RHPORT);

  while (1) {
    tuh_task();
  }

  return 0;
}
