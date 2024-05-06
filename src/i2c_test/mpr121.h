// MPR121 library
#pragma once

#include "daisy_seed.h"
#include <src/per/i2c.h>

//maximum wait before returning from I2C transaction
#ifndef MAX_I2C_WAIT
#define MAX_I2C_WAIT 500
#endif

// address is 7 bit, so need to shift left 1
#define MPR121_ADDR (0x5A << 1)
// register addresses
#define TOUCH_STATUS_LO 0x00
#define TOUCH_STATUS_HI 0x01
#define OOR_STATUS_LO 0x02
#define OOR_STATUS_HI 0x03
#define ELE0_BASELINE 0x1E
#define ELE1_BASELINE 0x1F
#define ELE2_BASELINE 0x20
#define ELE3_BASELINE 0x21
#define ELE4_BASELINE 0x22
#define ELE5_BASELINE 0x23
#define ELE6_BASELINE 0x24
#define ELE7_BASELINE 0x25
#define ELE8_BASELINE 0x26
#define ELE9_BASELINE 0x27
#define ELE10_BASELINE 0x28
#define ELE11_BASELINE 0x29
#define MHD_RISING 0x2B
#define NHD_RISING_AMT 0x2C
#define NCL_RISING 0x2D
#define FDL_RISING 0x2E
#define MHD_FALLING 0x2F
#define NHD_FALLING_AMT 0x30
#define NCL_FALLING 0x31
#define FDL_FALLING 0x32
#define NHD_AMT_TOUCHED 0x33
#define NCL_TOUCHED 0x34
#define FDL_TOUCHED 0x35
#define E0TTH 0x41
#define E0RTH 0x42
#define E1TTH 0x43
#define E1RTH 0x44
#define E2TTH 0x45
#define E2RTH 0x46
#define E3TTH 0x47
#define E3RTH 0x48
#define E4TTH 0x49
#define E4RTH 0x4A
#define E5TTH 0x4B
#define E5RTH 0x4C
#define E6TTH 0x4D
#define E6RTH 0x4E
#define E7TTH 0x4F
#define E7RTH 0x50
#define E8TTH 0x51
#define E8RTH 0x52
#define E9TTH 0x53
#define E9RTH 0x54
#define E10TTH 0x55
#define E10RTH 0x56
#define E11TTH 0x57
#define E11RTH 0x58
#define DEBOUNCE_T_R 0x5B
#define FILTER_CDC_CFG 0x5C
#define FILTER_CDT_CFG 0x5D
#define ELECTRODE_CFG 0x5E
#define GPIO_ENA 0x77
#define AUTOCFG_0 0x7B
#define AUTOCFG_1 0x7C
#define UPSIDE_LIMIT 0x7D
#define LOWSIDE_LIMIT 0x7E
#define TARGET_LEVEL 0x7F
#define SOFT_RESET 0x80

//config register bit offsets
//0x5B - Debounce Register
#define DR 0x04
#define DT 0x00
//0x5C - Filter/Global CDC Configuration Register
#define FFI 0x06
#define CDC 0x00
//0x5D - Filter/Global CDT Configuration Register
#define CDT 0x05
#define SFI 0x03
#define ESI 0x0
//0x5E - Electrode Configuration Register
#define ELECT_CL 0x06
#define ELECT_ELEPROX 0x04
#define ELECT_ELE_EN 0x00
//0x7B - Auto Config Control Register 0
#define AUTOCFG_ACE 0x00
#define AUTOCFG_ARE 0x01
#define AUTOCFG_BVA 0x02
#define AUTOCFG_RETRY 0x04
#define AUTOCFG_FFI 0x06
//0x7C - Auto Config Control Register 0
#define AUTOCFG_ACFIE 0x00
#define AUTOCFG_ARFIE 0x01
#define AUTOCFG_OORIE 0x02
#define AUTOCFG_SCTS 0x07

// Known good register settings taken from Adafruit MPR121 library
// check out https://github.com/adafruit/Adafruit_MPR121/
#define TOUCH_THRESHOLD 12
#define RELEASE_THRESHOLD 6
#define SOFT_RESET_MAGIC_NUMBER 0x63

static uint8_t i2cdata = 0;

bool init_MPR121(I2CHandle* i2c, DaisySeed* hw, uint8_t addr = MPR121_ADDR, bool autoconfig = true);
uint16_t readTouch(I2CHandle* i2c, uint8_t addr = MPR121_ADDR);

