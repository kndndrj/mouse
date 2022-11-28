#ifndef DRIVER_SENSOR_PMW3360_H
#define DRIVER_SENSOR_PMW3360_H

#include <stdbool.h>
#include <stdint.h>

// Registers
#define PMW3360_PRODUCT_ID 0x00
#define PMW3360_REVISION_ID 0x01
#define PMW3360_MOTION 0x02
#define PMW3360_DELTA_X_L 0x03
#define PMW3360_DELTA_X_H 0x04
#define PMW3360_DELTA_Y_L 0x05
#define PMW3360_DELTA_Y_H 0x06
#define PMW3360_SQUAL 0x07
#define PMW3360_RAW_DATA_SUM 0x08
#define PMW3360_MAXIMUM_RAW_DATA 0x09
#define PMW3360_MINIMUM_RAW_DATA 0x0A
#define PMW3360_SHUTTER_LOWER 0x0B
#define PMW3360_SHUTTER_UPPER 0x0C
#define PMW3360_CONTROL 0x0D
#define PMW3360_CONFIG_1 0x0F
#define PMW3360_CONFIG_2 0x10
#define PMW3360_ANGLE_TUNE 0x11
#define PMW3360_FRAME_CAPTURE 0x12
#define PMW3360_SROM_ENABLE 0x13
#define PMW3360_RUN_DOWNSHIFT 0x14
#define PMW3360_REST_1_RATE_LOWER 0x15
#define PMW3360_REST_1_RATE_UPPER 0x16
#define PMW3360_REST_1_DOWNSHIFT 0x17
#define PMW3360_REST_2_RATE_LOWER 0x18
#define PMW3360_REST_2_RATE_UPPER 0x19
#define PMW3360_REST_2_DOWNSHIFT 0x1A
#define PMW3360_REST_3_RATE_LOWER 0x1B
#define PMW3360_REST_3_RATE_UPPER 0x1C
#define PMW3360_OBSERVATION 0x24
#define PMW3360_DATA_OUT_LOWER 0x25
#define PMW3360_DATA_OUT_UPPER 0x26
#define PMW3360_RAW_DATA_DUMP 0x29
#define PMW3360_SROM_ID 0x2A
#define PMW3360_MIN_SQ_RUN 0x2B
#define PMW3360_RAW_DATA_THRESHOLD 0x2C
#define PMW3360_CONFIG_5 0x2F
#define PMW3360_POWER_UP_RESET 0x3A
#define PMW3360_SHUTDOWN 0x3B
#define PMW3360_INVERSE_PRODUCT_ID 0x3F
#define PMW3360_LIFTCUTOFF_TUNE_3 0x41
#define PMW3360_ANGLE_SNAP 0x42
#define PMW3360_LIFTCUTOFF_TUNE_1 0x4A
#define PMW3360_MOTION_BURST 0x50
#define PMW3360_LIFTCUTOFF_TUNE_TIMEOUT 0x58
#define PMW3360_LIFTCUTOFF_TUNE_MIN_LENGTH 0x5A
#define PMW3360_SROM_LOAD_BURST 0x62
#define PMW3360_LIFT_CONFIG 0x63
#define PMW3360_RAW_DATA_BURST 0x64
#define PMW3360_LIFTCUTOFF_TUNE_2 0x65

typedef struct pmw3360_burst_data {
  bool motion;
  bool on_surface;
  uint16_t dx;
  uint16_t dy;
  uint8_t surface_quality;
  uint8_t raw_data_sum;
  uint8_t max_raw_data;
  uint8_t min_raw_data;
  uint16_t shutter;
} pmw3360_burst_data_t;

// function prototypes
bool PMW3360_init(uint8_t CPI);

void PMW3360_set_cpi(uint8_t cpi);

uint8_t PMW3360_get_cpi(void);

pmw3360_burst_data_t PMW3360_read_burst(void);

bool PMW3360_check_signature(void);

bool PMW3360_self_test(void);

#endif
