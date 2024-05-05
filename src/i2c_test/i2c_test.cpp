#include "daisy_seed.h"
#include "daisysp.h"
#include <src/per/i2c.h>
#include <cstdio>

using namespace daisy;
using namespace daisysp;

static DaisySeed             hw;
//VariableSawOscillator varisaw;
//Oscillator            lfo;

//MPR121 Register Addresses
const uint16_t touch_sensor_addr = 0x5A;
const uint16_t touch_reg_lo = 0x00;
const uint16_t touch_reg_hi = 0x01;
const uint16_t autocfg_reg_0 = 0x7B;
const uint16_t autocfg_reg_1 = 0x7C;
//set this whole register to 0 to disable GPIO function
//for inputs 4-11
const uint16_t gpio_ena_reg = 0x77;
//IMPORTANT - THIS REGISTER IS THE IMPORTANT ONE!!!
//Electrode Config Register
//Setting this register will determine if the MPR121 is in
// RUN or STOP modes, as well as enable/disable touch channel inputs.
const uint16_t electrode_cfg_reg = 0x5E;

//config register shiftmasks
//0x7B - Auto Config Control Register 0
//ACE: Auto Config Enable
//ARE: Auto RE-config Enable
//RETRY: Number of retries, 00 = 0, 01 = 2, 10 = 4, 11 = 8
//  data = (1 << auto_cfg_ace_bit | 1 << auto_cfg_are_bit |
//          0b11 << auto_cfg_retry_bit);
const uint8_t auto_cfg_ace_bit = 0x00;
const uint8_t auto_cfg_are_bit = 0x01;
const uint8_t auto_cfg_retry_bit = 0x04;
//0x7C - Auto Config Control Register 0
//ACFIE: Auto Config Fail Interrupt Enable
//ARFIE: Auto Reconfig Fail Interrupt Enable
//OORIE: Out-of-range Interrupt Enable
//  data = (0b00 << auto_cfg_oorie_bit |
//          0b00 << auto_cfg_arfie_bit |
//          0b00 << auto_cfg_acfie_bit |
//          0b00 << auto_cfg_scts_bit);
const uint8_t auto_cfg_acfie_bit = 0x00;
const uint8_t auto_cfg_arfie_bit = 0x01;
const uint8_t auto_cfg_oorie_bit = 0x02;
const uint8_t auto_cfg_scts_bit = 0x07;
//0x5E - Electrode Configuration Register
//CL: Calibration Lock - 00 is default
//ELEPROX_EN: Proximity detection - 00 is default (disabled)
//ELE_EN: Electrode enable
//0000 - Off, MPR121 is in STOP mode (low power state)
//0x1 - 0xB -- Enable inputs in order
//To enable all inputs, only the top two bits are needed, bottom
//two are don't cares/X
//  data = (0b00 << electrode_cl_bit |
//          0b00 << electrode_eleprox_en_bit |
//          0b1111 << electrode_ele_en_bit);
const uint8_t electrode_cl_bit = 0x06;
const uint8_t electrode_eleprox_en_bit = 0x04;
const uint8_t electrode_ele_en_bit = 0x00;

uint8_t i2cdata = 0x0;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
/*    varisaw.SetFreq(hw.adc.GetFloat(0) * 300);
    lfo.SetFreq(hw.adc.GetFloat(1) * 40);
    for(size_t i = 0; i < size; i++)
    {
        float sig = lfo.Process();
        varisaw.SetPW(sig);

        out[0][i] = out[1][i] = varisaw.Process();
    }
*/
}

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.StartLog(true);
    hw.PrintLine("Hello from Daisy Seed!");
    //hw.SetAudioBlockSize(4);
    //float sample_rate = hw.AudioSampleRate();
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

    //setup autocfg reg 0
    i2cdata = (1 << auto_cfg_ace_bit | 1 << auto_cfg_are_bit |
               0b11 << auto_cfg_retry_bit);
    i2c.WriteDataAtAddress(touch_sensor_addr, autocfg_reg_0, 1, &i2cdata, 1, -1);

    //setup autocfg reg 1
    i2cdata =  (0b00 << auto_cfg_oorie_bit |
                0b00 << auto_cfg_arfie_bit |
                0b00 << auto_cfg_acfie_bit |
                0b00 << auto_cfg_scts_bit);
    i2c.WriteDataAtAddress(touch_sensor_addr, autocfg_reg_1, 1, &i2cdata, 1, -1);

    //setup electrode register
    i2cdata =  (0b00 << electrode_cl_bit |
                0b00 << electrode_eleprox_en_bit |
                0b1111 << electrode_ele_en_bit);
    i2c.WriteDataAtAddress(touch_sensor_addr, electrode_cfg_reg, 1, &i2cdata, 1, -1);
/*
    //set up ADC on pin D15 and D16 for our potentiometer
    const int adcChannels = 2;
    AdcChannelConfig adcConfig[adcChannels];
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    hw.adc.Init(adcConfig, adcChannels);

    varisaw.Init(sample_rate);
    varisaw.SetFreq(62.5f);
    varisaw.SetWaveshape(1.f);
    hw.adc.Start();

    lfo.Init(sample_rate);
    lfo.SetAmp(1.f);
    lfo.SetFreq(1.f);

    hw.StartAudio(AudioCallback);
*/
    while(1) 
    {
        hw.PrintLine("At top of While loop");
        System::Delay(125);
        i2cdata = 0;
        i2c.ReceiveBlocking(touch_sensor_addr, &i2cdata, 1, -1); 
        hw.SetLed(i2cdata > 0);
        hw.PrintLine("I2C Data: %x", i2cdata);
    }
}

