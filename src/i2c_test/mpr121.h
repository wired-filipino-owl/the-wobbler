// MPR121 library

#pragma once

// address is 7 bit, so need to shift left 1
#define MPR121_ADDR (0x5A << 1)
// register addresses
#define TOUCH_STATUS_LO 0x00
#define TOUCH_STATUS_HI 0x01
#define ELECTRODE_CFG 0x5E
#define AUTOCFG_0 0x7B
#define AUTOCFG_1 0x7C
#define GPIO_ENA 0x77

//config register bit offsets
//0x7B - Auto Config Control Register 0
#define AUTOCFG_ACE 0x00
#define AUTOCFG_ARE 0x01
#define AUTOCFG_BVA 0x02
#define AUTOCFG_RETRY 0x04

//0x7C - Auto Config Control Register 0
#define AUTOCFG_ACFIE 0x00
#define AUTOCFG_ARFIE 0x01
#define AUTOCFG_OORIE 0x02
#define AUTOCFG_SCTS 0x07

//0x5E - Electrode Configuration Register
#define ELECT_CL 0x06
#define ELECT_ELEPROX 0x04
#define ELECT_ELE_EN 0x00

