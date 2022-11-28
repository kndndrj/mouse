#include "button.h"

#include "hal/gpio.h"

void BUTTON_init(io_t button) {
  // TODO: also allow internal pullup and pulldown
  GPIO_init(button, GPIO_MODE_INPUT_NONE);
}

bool BUTTON_read(io_t button) { return GPIO_status(button); }
