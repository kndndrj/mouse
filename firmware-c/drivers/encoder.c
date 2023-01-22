#include "encoder.h"

#include "hal/delay.h"
#include "hal/gpio.h"

void ENCODER_init(io_t channel_a, io_t channel_b) {
  // TODO: also allow internal pullup and pulldown
  GPIO_init(channel_a, GPIO_MODE_INPUT_NONE);
  GPIO_init(channel_b, GPIO_MODE_INPUT_NONE);
}

encoder_state_t ENCODER_read(io_t channel_a, io_t channel_b) {
  static int8_t prev = 0x00;

  // read current state
  uint8_t current = 0x00;
  // 0b00000011
  //         ^^
  //         ||
  //   cur_a/ \cur_b

  if (GPIO_status(channel_a)) {
    current |= 1 << 1;
  }
  if (GPIO_status(channel_b)) {
    current |= 1 << 0;
  }

  if (prev == current) {
    return ENCODER_NONE;
  }

  uint8_t state = (prev << 2) | current;
  // determine direction based on state
  // We only care about leading edges -> cw: a, c   ccw: d, b
  // prev_a\ /prev_b
  //       ||
  //       vv
  // 0b00001111
  //         ^^
  //         ||
  //   cur_a/ \cur_b
  //            _____       _____       _____
  //           |     |     |     |     |     |
  // ch A  ____|     |_____|     |_____|     |____
  //
  //           :  :  :  :  :  :  :  :  :  :  :  :
  //      __       _____       _____       _____
  //        |     |     |     |     |     |     |
  // ch B   |_____|     |_____|     |_____|     |__
  //
  //           :  :  :  :  :  :  :  :  :  :  :  :
  // event     a  b  c  d  a  b  c  d  a  b  c  d

  // Debounce delay 1ms
  DELAY_us(1000);

  prev = current;

  switch (state) {
    // cw
    case 0b0010: // a
    case 0b1101: // c
      return ENCODER_CLOCKWISE;
    // ccw
    case 0b0001: // d
    case 0b1110: // b
      return ENCODER_COUNTER_CLOCKWISE;
  }

  return ENCODER_NONE;
}
