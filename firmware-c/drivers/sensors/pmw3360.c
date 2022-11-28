#include "pmw3360.h"

#include "hal/delay.h"
#include "hal/gpio.h"
#include "hal/spi.h"
#include "pmw3360_fw_tracking.h"

static uint8_t pmw3360_register_read(uint8_t address) {
  SPI_cs_clear();
  DELAY_us(100);

  // send adress of the register, with MSBit = 0 to indicate it's a read
  SPI_transfer(address & 0x7f);
  // tSRAD
  DELAY_us(160);

  // read data
  uint8_t data = SPI_transfer(0x00);

  DELAY_us(100);
  SPI_cs_set();

  //  tSRW/tSRR (=20us) minus tSCLK-NCS
  DELAY_us(19);

  return data;
}

static void pmw3360_register_write(uint8_t address, uint8_t data) {
  SPI_cs_clear();
  DELAY_us(100);
  // send adress of the register, with MSBit = 1 to indicate it's a write
  SPI_transfer(address | 0x80);
  // sent data
  SPI_transfer(data);
  // tSCLK-NCS for write operation
  DELAY_us(35);
  DELAY_us(100);

  SPI_cs_set();

  // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be
  // shortened, but is looks like a safe lower bound
  DELAY_us(100);
}

/*
adns_upload_firmware: load SROM content to the motion sensor
*/
static void pmw3360_firmware_upload(void) {
  // Write 0 to Rest_En bit of Config2 register to disable Rest mode.
  pmw3360_register_write(PMW3360_CONFIG_2, 0x00);

  // write 0x1d in SROM_enable reg for initializing
  pmw3360_register_write(PMW3360_SROM_ENABLE, 0x1d);

  // wait for 10 ms
  DELAY_us(10000);

  // write 0x18 to SROM_enable to start SROM download
  pmw3360_register_write(PMW3360_SROM_ENABLE, 0x18);

  // lower NCS
  SPI_cs_clear();

  // first byte is address
  SPI_transfer(PMW3360_SROM_LOAD_BURST | 0x80);
  DELAY_us(15);

  // send the rest of the firmware
  uint8_t c;
  for (unsigned int i = 0; i < (sizeof(pmw3360_fw_tracking) / sizeof(uint8_t));
       i++) {
    c = pmw3360_fw_tracking[i];
    SPI_transfer(c);
    DELAY_us(15);
  }

  DELAY_us(2);
  SPI_cs_set();
  DELAY_us(200);
}

bool PMW3360_init(uint8_t cpi) {
  static bool initialised = false;
  if (initialised) {
    return false;
  }

  // set up needed hal drivers
  SPI_init();
  DELAY_init();

  // sensor reset not active
  GPIO_init(IO_A10, GPIO_MODE_OUTPUT_PUSHPULL);
  GPIO_set(IO_A10);

  // reset the spi bus on the sensor
  SPI_cs_set();
  DELAY_us(50);
  SPI_cs_clear();
  DELAY_us(50);

  // Write to reset register
  pmw3360_register_write(PMW3360_POWER_UP_RESET, 0x5A);
  // 50 ms delay
  DELAY_us(100000);

  // read registers 0x02 to 0x06 (and discard the data)
  pmw3360_register_read(PMW3360_MOTION);
  pmw3360_register_read(PMW3360_DELTA_X_L);
  pmw3360_register_read(PMW3360_DELTA_X_H);
  pmw3360_register_read(PMW3360_DELTA_Y_L);
  pmw3360_register_read(PMW3360_DELTA_Y_H);

  // upload the firmware
  pmw3360_firmware_upload();

  bool is_valid_signature = PMW3360_check_signature();

  // Write 0x00 (rest disable) to Config2 register for wired mouse or 0x20 for
  // wireless mouse design.
  pmw3360_register_write(PMW3360_CONFIG_2, 0x00);

  DELAY_us(100);

  PMW3360_set_cpi(cpi);

  if (is_valid_signature) {
    initialised = true;
  }

  return is_valid_signature;
}

/*
setCPI: set CPI level of the motion sensor.

# parameter
cpi: Count per Inch value
*/
void PMW3360_set_cpi(uint8_t cpi) {
  pmw3360_register_write(PMW3360_CONFIG_1, cpi);
}

/*
getCPI: get CPI level of the motion sensor.

# retrun
cpi: Count per Inch value
*/
uint8_t PMW3360_get_cpi(void) {
  return pmw3360_register_read(PMW3360_CONFIG_1);
}

pmw3360_burst_data_t PMW3360_read_burst(void) {
  // Write any value to Motion_burst register
  pmw3360_register_write(PMW3360_MOTION_BURST, 0x00);

  // Lower NCS
  SPI_cs_clear();
  // Send Motion_burst address
  SPI_transfer(PMW3360_MOTION_BURST);

  // tSRAD_MOTBR
  DELAY_us(35);

  uint8_t buf[12];

  // Read the 12 bytes of burst data
  for (uint8_t i = 0; i < 12; i++) {
    buf[i] = SPI_transfer(0x00);
  }

  // Raise NCS
  SPI_cs_set();
  // tBEXIT
  DELAY_us(1);

  bool motion = (buf[0] & 0x80) != 0;
  bool on_surface = (buf[0] & 0x08) == 0;  // 0 if on surface / 1 if off surface

  uint8_t dxl = buf[2];  // dx LSB
  uint8_t dxh = buf[3];  // dx MSB
  uint8_t dyl = buf[4];  // dy LSB
  uint8_t dyh = buf[5];  // dy MSB
  uint8_t sl = buf[10];  // shutter LSB
  uint8_t sh = buf[11];  // shutter MSB

  // combine the register values
  uint16_t dx = dxh << 8 | dxl;
  uint16_t dy = dyh << 8 | dyl;
  uint16_t shutter = sh << 8 | sl;

  pmw3360_burst_data_t data = {
      .motion = motion,
      .on_surface = on_surface,
      .dx = dx,
      .dy = dy,
      .surface_quality = buf[6],
      .raw_data_sum = buf[7],
      .max_raw_data = buf[8],
      .min_raw_data = buf[9],
      .shutter = shutter,
  };

  return data;
}

/*
check_signature: check whether SROM is successfully loaded

return: true if the rom is loaded correctly.
*/
bool PMW3360_check_signature(void) {
  uint8_t srom = pmw3360_register_read(PMW3360_SROM_ID);
  uint8_t pid = pmw3360_register_read(PMW3360_PRODUCT_ID);
  uint8_t ipid = pmw3360_register_read(PMW3360_INVERSE_PRODUCT_ID);

  // signature for SROM 0x04
  return (srom == 0x04 && pid == 0x42 && ipid == 0xBD);
}

bool PMW3360_self_test(void) {
  pmw3360_register_write(PMW3360_SROM_ENABLE, 0x15);
  DELAY_us(10000);

  uint8_t u = pmw3360_register_read(PMW3360_DATA_OUT_UPPER);  // should be 0xBE
  uint8_t l = pmw3360_register_read(PMW3360_DATA_OUT_LOWER);  // should be 0xEF

  return (u == 0xBE && l == 0xEF);
}
