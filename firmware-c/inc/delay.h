#ifndef DELAY
#define DELAY

#include <stdint.h>

/**
 * Initialize the timers used for delays.
 */
void delay_setup(void);

/**
 * busy wait for a number of usecs.
 * @param us number of usecs to delay.
 */
void delay_us(uint16_t us);

#endif
