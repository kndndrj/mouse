#ifndef HAL_SPI_PRESETS_H
#define HAL_SPI_PRESETS_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "pins.h"

//
// Pin definitions
//
// Chip select
#ifndef SPI_CS_PIN
#define SPI_CS_PIN IO_A4
#endif

// Clock
#ifndef SPI_SCK_PIN
#define SPI_SCK_PIN IO_A5
#endif

// MISO
#ifndef SPI_MISO_PIN
#define SPI_MISO_PIN IO_A6
#endif

// MOSI
#ifndef SPI_MOSI_PIN
#define SPI_MOSI_PIN IO_A7
#endif

//
// Driver
//
// TODO: automate register names with macros
#ifndef SPI_DRIVER
#define SPI_DRIVER SPI1
#endif
#ifndef SPI_DRIVER_SR
#define SPI_DRIVER_SR SPI1_SR
#endif
#ifndef SPI_DRIVER_RCC
#define SPI_DRIVER_RCC RCC_SPI1
#endif

#endif
