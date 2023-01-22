#include <libopencm3/stm32/rcc.h>

#include "drivers/button.h"
#include "drivers/encoder.h"
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

  usb_driver_t usb_driver = USB_init();

  ENCODER_init(IO_B6, IO_B7);

  // status LED
  GPIO_init(IO_A8, GPIO_MODE_OUTPUT_PUSHPULL);
  GPIO_clear(IO_A8);

  BUTTON_init(IO_A0);
  BUTTON_init(IO_A1);
  BUTTON_init(IO_A2);

  // start at 800 dpi
  bool isvalid = PMW3360_init(0x32);

  while (1) {
    if (!isvalid) {
      GPIO_toggle(IO_A8);
      DELAY_us(60000);
      isvalid = PMW3360_init(0x32);
    }
    USB_poll(usb_driver);

    uint8_t buf[4] = {0, 0, 0, 0};

    // Buttons
    bool button_left = BUTTON_read(IO_A0);
    bool button_right = BUTTON_read(IO_A1);
    bool button_middle = BUTTON_read(IO_A2);

    if (!button_left) {
      buf[0] |= 1 << 0;
    }
    if (!button_right) {
      buf[0] |= 1 << 1;
    }
    if (!button_middle) {
      buf[0] |= 1 << 2;
    }

    // Wheel
    encoder_state_t encoder = ENCODER_read(IO_B6, IO_B7);

    if (encoder == ENCODER_CLOCKWISE) {
      buf[3] = (uint8_t)1;
    } else if (encoder == ENCODER_COUNTER_CLOCKWISE) {
      buf[3] = (uint8_t)-1;
    }

    // Sensor
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
    }

    USB_write_packet(usb_driver, buf);
  }
}
