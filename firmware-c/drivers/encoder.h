#ifndef DRIVER_ENCODER_H
#define DRIVER_ENCODER_H

#include <stdint.h>

#include "pins.h"

typedef enum {
  ENCODER_CLOCKWISE,
  ENCODER_COUNTER_CLOCKWISE,
  ENCODER_NONE,
} encoder_state_t;

void ENCODER_init(io_t channel_a, io_t channel_b);

encoder_state_t ENCODER_read(io_t channel_a, io_t channel_b);

#endif
