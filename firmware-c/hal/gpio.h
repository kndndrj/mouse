#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#include "pins.h"

typedef enum {
  GPIO_MODE_INPUT_PULLUP,
  GPIO_MODE_INPUT_PULLDOWN,
  GPIO_MODE_INPUT_NONE,
  GPIO_MODE_OUTPUT_PUSHPULL,
  GPIO_MODE_OUTPUT_OPENDRAIN,
} gpio_mode_t;

void GPIO_init(io_t pin, gpio_mode_t mode);

void GPIO_set(io_t pin);

void GPIO_clear(io_t pin);

void GPIO_toggle(io_t pin);

bool GPIO_status(io_t pin);

#endif
