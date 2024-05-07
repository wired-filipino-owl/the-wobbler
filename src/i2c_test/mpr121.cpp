
#include "mpr121.h"

bool init_MPR121(I2CHandle* i2c, DaisySeed* hw, uint8_t addr, bool autoconfig)
{
  if (i2c == nullptr || hw == nullptr)
    return false;
  
  //reset the MPR121
  i2cdata = SOFT_RESET_MAGIC_NUMBER;
  i2c->WriteDataAtAddress(addr, SOFT_RESET, 1, &i2cdata, 1, MAX_I2C_WAIT);
  daisy::System::Delay(2); //wait for MPR121 to reset
  
  //make sure we're in stop mode, turn off all electrodes
  i2cdata = 0x00;
  i2c->WriteDataAtAddress(addr, ELECTRODE_CFG, 1, &i2cdata, 1, MAX_I2C_WAIT);
  
  //make sure that CDT Config is set to default
  i2c->ReadDataAtAddress(addr, FILTER_CDT_CFG, 1, &i2cdata, 1, MAX_I2C_WAIT);
  if(i2cdata != 0x24)
    return false;
  
  //set up touch and release thresholds
  i2cdata = TOUCH_THRESHOLD;
  for (int i = 0; i < 12; i++)
  {
    i2c->WriteDataAtAddress(addr, E0TTH + (i*2), 1, &i2cdata, 1, MAX_I2C_WAIT);
  }
  i2cdata = RELEASE_THRESHOLD;
  for (int i = 0; i < 12; i++)
  {
    i2c->WriteDataAtAddress(addr, E0RTH + (i*2), 1, &i2cdata, 1, MAX_I2C_WAIT);
  }
  
  //setup filtering baseline stuff, should probably read up on AN3889-3892 for why/how these numbers are the way they are
  //RISING
  i2cdata = 0x01;
  i2c->WriteDataAtAddress(addr, MHD_RISING, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2c->WriteDataAtAddress(addr, NHD_RISING_AMT, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2cdata = 0x0E;
  i2c->WriteDataAtAddress(addr, NCL_RISING, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2cdata = 0x00;
  i2c->WriteDataAtAddress(addr, FDL_RISING, 1, &i2cdata, 1, MAX_I2C_WAIT);
  //FALLING
  i2cdata = 0x01;
  i2c->WriteDataAtAddress(addr, MHD_FALLING, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2cdata = 0x05;
  i2c->WriteDataAtAddress(addr, NHD_FALLING_AMT, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2cdata = 0x01;
  i2c->WriteDataAtAddress(addr, NCL_FALLING, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2cdata = 0x00;
  i2c->WriteDataAtAddress(addr, FDL_FALLING, 1, &i2cdata, 1, MAX_I2C_WAIT);
  
  i2cdata = 0x00;
  i2c->WriteDataAtAddress(addr, NHD_AMT_TOUCHED, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2c->WriteDataAtAddress(addr, NCL_TOUCHED, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2c->WriteDataAtAddress(addr, FDL_TOUCHED, 1, &i2cdata, 1, MAX_I2C_WAIT);
  
  //debounce
  i2c->WriteDataAtAddress(addr, DEBOUNCE_T_R, 1, &i2cdata, 1, MAX_I2C_WAIT);
  
  //electrode charge/discharge timing and current
  i2cdata = (0b00 << FFI | 0b010000 << CDC); //16uA charge current
  i2c->WriteDataAtAddress(addr, FILTER_CDC_CFG, 1, &i2cdata, 1, MAX_I2C_WAIT);
  i2cdata = (0b001 << CDT | 0b00 << SFI | 0b000 << ESI); //0.5us encode, 1ms period
  i2c->WriteDataAtAddress(addr, FILTER_CDT_CFG, 1, &i2cdata, 1, MAX_I2C_WAIT);
  
  //setup autoconfig if it's enabled
  if (autoconfig)
  {
    i2cdata = (0b00 << AUTOCFG_FFI | 0b00 << AUTOCFG_RETRY | 0b10 << AUTOCFG_BVA 
               | 1 << AUTOCFG_ARE | 1 << AUTOCFG_ACE);
    i2c->WriteDataAtAddress(addr, AUTOCFG_0, 1, &i2cdata, 1, MAX_I2C_WAIT);
    
    i2cdata = 200; //((Vdd - 0.7)/Vdd) * 256
    i2c->WriteDataAtAddress(addr, UPSIDE_LIMIT, 1, &i2cdata, 1, MAX_I2C_WAIT);
    i2cdata = 130; //UPSIDE_LIMIT * 0.65
    i2c->WriteDataAtAddress(addr, LOWSIDE_LIMIT, 1, &i2cdata, 1, MAX_I2C_WAIT);
    i2cdata = 180; //(UPSIDE_LIMIT * 0.9
    i2c->WriteDataAtAddress(addr, TARGET_LEVEL, 1, &i2cdata, 1, MAX_I2C_WAIT);
  }
  
  i2cdata = (0b00 << ELECT_CL | 0b00 << ELECT_ELEPROX | 0b1111 << ELECT_ELE_EN); //enable all electrodes, default for CL and Proximity Detection
  i2c->WriteDataAtAddress(addr, ELECTRODE_CFG, 1, &i2cdata, 1, MAX_I2C_WAIT);
  
  return true;
}

uint16_t readTouch(I2CHandle* i2c, uint8_t addr)
{
  uint8_t reg_lo = 0;
  uint8_t reg_hi = 0;
  bool fail = 0;
  i2c->ReadDataAtAddress(addr, TOUCH_STATUS_LO, 1, &reg_lo, 1, MAX_I2C_WAIT);
  i2c->ReadDataAtAddress(addr, TOUCH_STATUS_HI, 1, &reg_hi, 1, MAX_I2C_WAIT);
  
  return ((reg_hi & 0x0F) << 8 | reg_lo);
}
