#include <libopencm3/stm32/rcc.h>

#include "drivers/sensors/pmw3360.h"
#include "hal/delay.h"
#include "hal/gpio.h"
#include "hal/spi.h"
#include "hal/usb.h"

#include "pins.h"

int main(void) {
  // clock setup
  rcc_clock_setup_in_hsi_out_48mhz();

  DELAY_init();

  USB_init();

  // status LED
  GPIO_init(IO_A8, GPIO_MODE_OUTPUT_PUSHPULL);
  GPIO_clear(IO_A8);

  // start at 800 dpi
  bool isvalid = PMW3360_init(0x32);

  while (1) {
    if (!isvalid) {
      GPIO_toggle(IO_A8);
      DELAY_us(60000);
      isvalid = PMW3360_init(0x32);
    }
    USB_poll();

    uint8_t buf[4] = {0, 0, 0, 0};
    pmw3360_burst_data_t motion_data = PMW3360_read_burst();

    if (motion_data.motion && motion_data.on_surface) {
      if (motion_data.dx > 32767) {
        buf[1] = 65535 - motion_data.dx;
      } else {
        buf[1] = -motion_data.dx;
      }
      if (motion_data.dy > 32767) {
        buf[2] = 65535 - motion_data.dy;
      } else {
        buf[2] = -motion_data.dy;
      }
      USB_write_packet(buf);
    }
  }
}
