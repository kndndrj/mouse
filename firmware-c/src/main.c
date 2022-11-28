#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdlib.h>

#include "drivers/sensors/pmw3360.h"
#include "hal/delay.h"
#include "hal/spi.h"
#include "hal/usb.h"

// static void gpio_setup(void) {
//   // set gpio inputs with pullup
//   gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO4);
//   gpio_set(GPIOB, GPIO4);
//   gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO5);
//   gpio_set(GPIOB, GPIO5);
//   gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO6);
//   gpio_set(GPIOB, GPIO6);
//   gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO7);
//   gpio_set(GPIOB, GPIO7);
// }

int main(void) {
  // clock setup
  rcc_clock_setup_in_hsi_out_48mhz();

  /* Enable GPIOA clock. */
  rcc_periph_clock_enable(RCC_GPIOA);
  // gpio_setup();

  DELAY_init();

  SPI_init();

  USB_init();

  // sensor reset not active
  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
  gpio_set(GPIOA, GPIO10);

  // start at 800 dpi
  bool isvalid = PMW3360_init(0x32);

  while (1) {
    if (!isvalid) {
      gpio_toggle(GPIOB, GPIO12);
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
