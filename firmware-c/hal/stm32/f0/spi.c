#include "spi.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "spi_presets.h"

// TODO: allow setting different parameters for spi
void SPI_init(void) {
  static bool initialised = false;
  if (initialised) {
    return;
  }

  rcc_periph_clock_enable(RCC_SPI1);

  // alternate functions for SCR, MISO, MOSI
  gpio_mode_setup(SPI_SCK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_SCK_PIN);
  gpio_mode_setup(SPI_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_MISO_PIN);
  gpio_mode_setup(SPI_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI_MOSI_PIN);
  gpio_set_af(SPI_SCK_PORT, GPIO_AF0, SPI_SCK_PIN);
  gpio_set_af(SPI_MISO_PORT, GPIO_AF0, SPI_MISO_PIN);
  gpio_set_af(SPI_MOSI_PORT, GPIO_AF0, SPI_MOSI_PIN);

  // normal output for SS pin and sensor reset pin
  gpio_mode_setup(SPI_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI_CS_PIN);

  // Reset SPI, SPI_CR1 register cleared, SPI is disabled
  spi_reset(SPI_DRIVER);

  // Set up SPI in Master mode with:
  // Clock baud rate: 1/64 of peripheral clock frequency
  // Clock polarity: Idle High
  // Clock phase: Data valid on 2nd clock pulse
  // Frame format: MSB First
  spi_init_master(SPI_DRIVER, SPI_CR1_BAUDRATE_FPCLK_DIV_64,
                  SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_MSBFIRST);

  // Data frame size: 8-bit
  spi_set_data_size(SPI_DRIVER, SPI_CR2_DS_8BIT);
  spi_fifo_reception_threshold_8bit(SPI_DRIVER);

  gpio_set(SPI_CS_PORT, SPI_CS_PIN);

  spi_enable(SPI_DRIVER);

  initialised = true;
}

uint8_t SPI_transfer(uint8_t data) {
  // empty the rx fifo (read until both flags from spi status register are
  // cleared)
  // TODO: remove?
  while ((SPI_DRIVER_SR & (1 << 10)) || (SPI_DRIVER_SR & (1 << 9))) {
    spi_read8(SPI_DRIVER);
  }
  spi_send8(SPI_DRIVER, data);

  return spi_read8(SPI_DRIVER);
}

void SPI_cs_set(void) { gpio_set(SPI_CS_PORT, SPI_CS_PIN); }

void SPI_cs_clear(void) { gpio_clear(SPI_CS_PORT, SPI_CS_PIN); }
