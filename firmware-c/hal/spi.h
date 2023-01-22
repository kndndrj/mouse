#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>

void SPI_init(void);

// write the pulse sequence and return value from results register
uint8_t SPI_transfer(uint8_t data);

void SPI_cs_set(void);

void SPI_cs_clear(void);

#endif
