/* Author: Rob Galeoto (bitrot_alpha)
 *
 * Filename: i2c_test.cpp
 * What is this: This demonstrates the usage and capabilities of
 * 		the MPR121 I2C library.
 */
#include "daisy_seed.h"
#include <src/per/i2c.h>
#include <cstdio>
#include "mpr121.h"

using namespace daisy;

static DaisySeed             hw;

uint8_t mprdata = 0x0;

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.StartLog(false);
    hw.PrintLine("Hello from Daisy Seed!");
    // setup the configuration
    I2CHandle::Config i2c_conf;
    i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
    i2c_conf.speed  = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c_conf.mode   = I2CHandle::Config::Mode::I2C_MASTER;
    i2c_conf.pin_config.scl  = {DSY_GPIOB, 8};
    i2c_conf.pin_config.sda  = {DSY_GPIOB, 9};
    // initialise the peripheral
    I2CHandle i2c;
    i2c.Init(i2c_conf);
    // now i2c points to the corresponding peripheral and can be used.

    hw.PrintLine("Attempting to setup MPR121 now.");
    bool result = init_MPR121(&i2c, MPR121_ADDR, false);
    hw.PrintLine("Result: %s", (result ? "success" : "failed!"));

    mprdata = 0;
    i2c.ReadDataAtAddress(MPR121_ADDR, AUTOCFG_0, 1, &mprdata, 1, MAX_I2C_WAIT);
    hw.PrintLine("Read from Autoconfig 0 Register: 0x%X", mprdata);

    mprdata = 0;
    i2c.ReadDataAtAddress(MPR121_ADDR, AUTOCFG_1, 1, &mprdata, 1, MAX_I2C_WAIT);
    hw.PrintLine("Read from Autoconfig 1 Register: 0x%X", mprdata);

    mprdata = 0;
    i2c.ReadDataAtAddress(MPR121_ADDR, ELECTRODE_CFG, 1, &mprdata, 1, MAX_I2C_WAIT);
    hw.PrintLine("Read from Electrode Enable Register: 0x%X", mprdata);

    //wait 2 seconds for MPR121 to get baseline values
    System::Delay(2000);
    while(1)
   {
        hw.PrintLine("At top of While loop");
   	hw.SetLed(readTouch(&i2c) > 0);  
	//wait for touch
	while(readTouch(&i2c) == 0)
	{
		System::Delay(64);
	}
        mprdata = 0;
	i2c.ReadDataAtAddress(MPR121_ADDR, TOUCH_STATUS_LO, 1, &mprdata, 1, MAX_I2C_WAIT);
    	hw.PrintLine("Read from TOUCH_REG_LO : 0x%X", mprdata);
        mprdata = 0;
	i2c.ReadDataAtAddress(MPR121_ADDR, TOUCH_STATUS_HI, 1, &mprdata, 1, MAX_I2C_WAIT);
    	hw.PrintLine("Read from TOUCH_REG_HI: 0x%X", mprdata);
	System::Delay(64);
   }
}

