#ifndef HAL_SPI_PRESETS_H
#define HAL_SPI_PRESETS_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

// TODO: better pin name macros e.g. PA4

//
// Pin definitions
//
// Chip select
#ifndef SPI_CS_PORT
	#define SPI_CS_PORT GPIOA
#endif
#ifndef SPI_CS_PIN
	#define SPI_CS_PIN GPIO4
#endif

// Clock
#ifndef SPI_SCK_PORT
	#define SPI_SCK_PORT GPIOA
#endif
#ifndef SPI_SCK_PIN
	#define SPI_SCK_PIN GPIO5
#endif

// MISO
#ifndef SPI_MISO_PORT
	#define SPI_MISO_PORT GPIOA
#endif
#ifndef SPI_MISO_PIN
	#define SPI_MISO_PIN GPIO6
#endif

// MOSI
#ifndef SPI_MOSI_PORT
	#define SPI_MOSI_PORT GPIOA
#endif
#ifndef SPI_MOSI_PIN
	#define SPI_MOSI_PIN GPIO7
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


#endif
