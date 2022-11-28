#include "gpio.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "pins.h"

void GPIO_init(io_t pin, gpio_mode_t mode) {
  rcc_periph_clock_enable(RCC_PORT(pin));

  switch (mode) {
    case GPIO_MODE_INPUT_PULLDOWN:
      gpio_mode_setup(PORT(pin), GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, PIN(pin));
      break;
    case GPIO_MODE_INPUT_PULLUP:
      gpio_mode_setup(PORT(pin), GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, PIN(pin));
      break;
    case GPIO_MODE_INPUT_NONE:
      gpio_mode_setup(PORT(pin), GPIO_MODE_INPUT, GPIO_PUPD_NONE, PIN(pin));
      break;
    case GPIO_MODE_OUTPUT_PUSHPULL:
    case GPIO_MODE_OUTPUT_OPENDRAIN:
      gpio_mode_setup(PORT(pin), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN(pin));
      break;
  }
}

void GPIO_set(io_t pin) { gpio_set(PORT(pin), PIN(pin)); }

void GPIO_clear(io_t pin) { gpio_clear(PORT(pin), PIN(pin)); }

void GPIO_toggle(io_t pin) { gpio_toggle(PORT(pin), PIN(pin)); }

bool GPIO_status(io_t pin) { return (bool)gpio_get(PORT(pin), PIN(pin)); }
