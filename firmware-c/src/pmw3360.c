/*
  PMW3366.cpp - Library for interfacing PMW3360 motion sensor module

  Copyright (c) 2019, Sunjun Kim

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "pmw3360.h"

//================================================================================
// PMW3360 Motion Sensor Module

/*
begin: initalize variables, prepare the sensor to be init.

# parameter
ss_pin: The arduino pin that is connected to slave select on the module.
CPI: initial CPI. optional.
*/
bool pmw3360_setup(uint8_t CPI) {

  // reset the spi bus on the sensor
  gpio_set(GPIOA, GPIO4);
  delay_us(100);
  gpio_clear(GPIOA, GPIO4);
  delay_us(100);

  // Reset register
  pmw3360_reg_write(PMW3360_POWER_UP_RESET, 0x5A);
  // 50 ms delay
  delay_us(50000);

  // read registers 0x02 to 0x06 (and discard the data)
  pmw3360_reg_read(PMW3360_MOTION);
  pmw3360_reg_read(PMW3360_DELTA_X_L);
  pmw3360_reg_read(PMW3360_DELTA_X_H);
  pmw3360_reg_read(PMW3360_DELTA_Y_L);
  pmw3360_reg_read(PMW3360_DELTA_Y_H);

  // upload the firmware
  pmw3360_firmware_upload();

  delay_us(100);

  pmw3360_set_cpi(CPI);

  bool is_valid_signature = pmw3360_check_signature();

  return is_valid_signature;
}

/*
setCPI: set CPI level of the motion sensor.

# parameter
cpi: Count per Inch value
*/
void pmw3360_set_cpi(uint8_t cpi) { pmw3360_reg_write(PMW3360_CONFIG_1, cpi); }

/*
getCPI: get CPI level of the motion sensor.

# retrun
cpi: Count per Inch value
*/
uint8_t pmw3360_get_cpi(void) {
  uint8_t cpival = pmw3360_reg_read(PMW3360_CONFIG_1);

  return cpival;
}

struct pmw3360_burst_data pmw3360_read_burst(void) {
  uint8_t burst_buffer[12];

  gpio_clear(GPIOA, GPIO4);
  delay_us(100);

  delay_us(35);
  // Write any value to Motion_burst register
  spi_send(SPI1, PMW3360_MOTION_BURST | 0x80);
  spi_send(SPI1, 0x00);

  // Send Motion_burst address
  spi_send(SPI1, PMW3360_MOTION_BURST);

  // tSRAD_MOTBR
  delay_us(35);

  // clear the RXNE flag
  spi_read(SPI1);

  // Read the 12 bytes of burst data
  for (uint8_t i = 0; i < 12; i++) {
    spi_send(SPI1, 0x00);
    burst_buffer[i] = spi_read(SPI1);
  }

  delay_us(120);

  gpio_set(GPIOA, GPIO4);
  delay_us(100);

  bool motion = (burst_buffer[0] & 0x80) != 0;
  bool on_surface =
      (burst_buffer[0] & 0x08) == 0; // 0 if on surface / 1 if off surface

  uint8_t dxl = burst_buffer[2]; // dx LSB
  uint8_t dxh = burst_buffer[3]; // dx MSB
  uint8_t dyl = burst_buffer[4]; // dy LSB
  uint8_t dyh = burst_buffer[5]; // dy MSB
  uint8_t sl = burst_buffer[10]; // shutter LSB
  uint8_t sh = burst_buffer[11]; // shutter MSB

  // combine the register values
  uint16_t dx = dxh << 8 | dxl;
  uint16_t dy = dyh << 8 | dyl;
  uint16_t shutter = sh << 8 | sl;

  struct pmw3360_burst_data data = {
      .motion = motion,
      .on_surface = on_surface,
      .dx = dx,
      .dy = dy,
      .surface_quality = burst_buffer[6],
      .raw_data_sum = burst_buffer[7],
      .max_raw_data = burst_buffer[8],
      .min_raw_data = burst_buffer[9],
      .shutter = shutter,
  };

  return data;
}

/*
pmw3360_reg_read: write one byte value to the given reg_addr.
*/
uint8_t pmw3360_reg_read(uint8_t reg_addr) {

  gpio_clear(GPIOA, GPIO4);
  delay_us(100);

  // send adress of the register, with MSBit = 0 to indicate it's a read
  spi_send(SPI1, reg_addr & 0x7f);
  // tSRAD
  delay_us(100);
  // read data
  spi_xfer(SPI1, 0x00);
  uint8_t data = spi_read(SPI1);

  delay_us(100);
  gpio_set(GPIOA, GPIO4);
  //  tSRW/tSRR (=20us) minus tSCLK-NCS
  delay_us(19);

  return data;
}

/*
adns_write_reg: write one byte value to the given reg_addr
*/
void pmw3360_reg_write(uint8_t reg_addr, uint8_t data) {

  gpio_clear(GPIOA, GPIO4);
  delay_us(100);
  // send adress of the register, with MSBit = 1 to indicate it's a write
  spi_send(SPI1, reg_addr | 0x80);
  // sent data
  spi_send(SPI1, data);
  // tSCLK-NCS for write operation
  delay_us(35);
  delay_us(100);
  gpio_set(GPIOA, GPIO4);

  // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be
  // shortened, but is looks like a safe lower bound
  delay_us(100);
}

/*
adns_upload_firmware: load SROM content to the motion sensor
*/
void pmw3360_firmware_upload(void) {
  // Write 0 to Rest_En bit of Config2 register to disable Rest mode.
  pmw3360_reg_write(PMW3360_CONFIG_2, 0x00);

  // write 0x1d in SROM_enable reg for initializing
  pmw3360_reg_write(PMW3360_SROM_ENABLE, 0x1d);

  // wait for more than one frame period
  // assume that the frame rate is as low as 100fps... even if it
  // should never be that low
  delay_us(100);

  // write 0x18 to SROM_enable to start SROM download
  pmw3360_reg_write(PMW3360_SROM_ENABLE, 0x18);

  // write the SROM file (=firmware data)
  gpio_clear(GPIOA, GPIO4);
  spi_send(SPI1, PMW3360_SROM_LOAD_BURST | 0x80);
  delay_us(15);

  // send all bytes of the firmware
  uint8_t c;
  for (unsigned int i = 0; i < (sizeof(firmware_tracking) / sizeof(uint8_t)); i++) {
    c = firmware_tracking[i];
    spi_send(SPI1, c);
    delay_us(15);
    delay_us(15);
  }

  delay_us(100);
  gpio_set(GPIOA, GPIO4);
  delay_us(200);

  // Write 0x00 (rest disable) to Config2 register for wired mouse or 0x20 for
  // wireless mouse design.
  pmw3360_reg_write(PMW3360_CONFIG_2, 0x00);
}

/*
check_signature: check whether SROM is successfully loaded

return: true if the rom is loaded correctly.
*/
bool pmw3360_check_signature(void) {

  uint8_t pid = pmw3360_reg_read(PMW3360_PRODUCT_ID);
  uint8_t iv_pid = pmw3360_reg_read(PMW3360_INVERSE_PRODUCT_ID);
  uint8_t SROM_ver = pmw3360_reg_read(PMW3360_SROM_ID);

  // signature for SROM 0x04
  return (pid == 0x42 && iv_pid == 0xBD && SROM_ver == 0x04);
}

void pmw3360_self_test(void) {
  pmw3360_reg_write(PMW3360_SROM_ENABLE, 0x15);
  delay_us(60000);
  delay_us(60000);
  delay_us(60000);

  uint8_t u = pmw3360_reg_read(PMW3360_DATA_OUT_UPPER); // should be 0xBE
  uint8_t l = pmw3360_reg_read(PMW3360_DATA_OUT_LOWER); // should be 0xEF

  if (l + u == 0) {
    __asm__("nop");
  }
}
