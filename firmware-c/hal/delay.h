#ifndef HAL_DELAY_H
#define HAL_DELAY_H

#include <stdint.h>

// 
// Initialize the timers used for delays.
// 
void DELAY_init(void);

// 
// Busy wait for a number of micro seconds.
// 
void DELAY_us(uint32_t us);

#endif
