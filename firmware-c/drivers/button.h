#ifndef DRIVER_BUTTON_H
#define DRIVER_BUTTON_H

#include <stdbool.h>

#include "pins.h"

void BUTTON_init(io_t button);

bool BUTTON_read(io_t button);

#endif
