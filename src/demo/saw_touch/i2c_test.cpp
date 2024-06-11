#include "daisy_seed.h"
#include "daisysp.h"
#include <src/per/i2c.h>
#include <cstdio>
#include "mpr121.h"

#define DEBUG_PRINT false
#define ENV_ATK_TIME 0.05F
#define ENV_DECAY_TIME 0.4F

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

static DaisySeed hw;

Oscillator c5, d5, e5, f5, g5, a5, b5, c6;
AdEnv c5_env, d5_env, e5_env, f5_env, g5_env, a5_env, b5_env, c6_env;

uint8_t touch_lo, touch_hi = 0;

float sig, vol;
float c5_out, d5_out, e5_out, f5_out, g5_out, a5_out, b5_out, c6_out;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    vol = hw.adc.GetFloat(0);
    //touch_lo
    if (touch_lo & 0x02)
        f5_env.Trigger();
    if (touch_lo & 0x04)
        e5_env.Trigger();
    if (touch_lo & 0x08)
        d5_env.Trigger();
    if (touch_lo & 0x10)
        c5_env.Trigger();
    if (touch_lo & 0x80)
        g5_env.Trigger();

    //touch_hi
    if (touch_hi & 0x01)
        a5_env.Trigger();
    if (touch_hi & 0x02)
        b5_env.Trigger();
    if (touch_hi & 0x04)
        c6_env.Trigger();

    //Prepare the audio block
    for(size_t i = 0; i < size; i += 2)
    {
        c5.SetAmp(c5_env.Process());
        c5_out = c5.Process();
        d5.SetAmp(d5_env.Process());
        d5_out = d5.Process();
        e5.SetAmp(e5_env.Process());
        e5_out = e5.Process();
        f5.SetAmp(f5_env.Process());
        f5_out = f5.Process();
        g5.SetAmp(g5_env.Process());
        g5_out = g5.Process();
        a5.SetAmp(a5_env.Process());
        a5_out = a5.Process();
        b5.SetAmp(b5_env.Process());
        b5_out = b5.Process();
        c6.SetAmp(c6_env.Process());
        c6_out = c6.Process();

        sig = c5_out + d5_out + e5_out + f5_out 
              + g5_out + a5_out + b5_out + c6_out;

        sig *= vol;
        //Set the left and right outputs to the mixed signals
        out[i]     = sig;
        out[i + 1] = sig;
    }
}

int main(void)
{
    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hw.Configure();
    hw.Init();
    hw.StartLog(DEBUG_PRINT);
    hw.PrintLine("Hello from Daisy Seed!");
    hw.SetAudioBlockSize(4);

    //set up ADC on pin D16
    const int adcChannels = 1;
    AdcChannelConfig adcConfig[adcChannels];
    adcConfig[0].InitSingle(hw.GetPin(16));
    hw.adc.Init(adcConfig, adcChannels);
    hw.adc.Start();

    float samplerate = hw.AudioSampleRate();
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

#if DEBUG_PRINT == true
    uint8_t mprdata = 0;
    System::Delay(64);

    mprdata = 0;
    i2c.ReadDataAtAddress(MPR121_ADDR, AUTOCFG_0, 1, &mprdata, 1, MAX_I2C_WAIT);
    hw.Print("Autoconfig 0 Reg: ");
    hw.Print("%#X\r\n", mprdata);

    System::Delay(64);

    mprdata = 0;
    i2c.ReadDataAtAddress(MPR121_ADDR, AUTOCFG_1, 1, &mprdata, 1, MAX_I2C_WAIT);
    hw.Print("Autoconfig 1 Reg: ");
    hw.Print("%#X\r\n", mprdata);

    System::Delay(64);

    mprdata = 0;
    i2c.ReadDataAtAddress(MPR121_ADDR, ELECTRODE_CFG, 1, &mprdata, 1, MAX_I2C_WAIT);
    hw.Print("Read from Electrode Enable Reg: ");
    hw.Print("%#X\r\n", mprdata);

#endif
    //initialize oscillators
    c5.Init(samplerate);
    c5.SetWaveform(Oscillator::WAVE_TRI);
    c5.SetAmp(1); 
    c5.SetFreq(523.25);
    
    d5.Init(samplerate);
    d5.SetWaveform(Oscillator::WAVE_TRI);
    d5.SetAmp(1); 
    d5.SetFreq(587.33);

    e5.Init(samplerate);
    e5.SetWaveform(Oscillator::WAVE_TRI);
    e5.SetAmp(1); 
    e5.SetFreq(659.26);

    f5.Init(samplerate);
    f5.SetWaveform(Oscillator::WAVE_TRI);
    f5.SetAmp(1); 
    f5.SetFreq(698.46);

    g5.Init(samplerate);
    g5.SetWaveform(Oscillator::WAVE_TRI);
    g5.SetAmp(1); 
    g5.SetFreq(783.99);

    a5.Init(samplerate);
    a5.SetWaveform(Oscillator::WAVE_TRI);
    a5.SetAmp(1); 
    a5.SetFreq(880.0);

    b5.Init(samplerate);
    b5.SetWaveform(Oscillator::WAVE_TRI);
    b5.SetAmp(1); 
    b5.SetFreq(987.77);

    c6.Init(samplerate);
    c6.SetWaveform(Oscillator::WAVE_TRI);
    c6.SetAmp(1); 
    c6.SetFreq(1046.5);

    //initialize envelopes
    c5_env.Init(samplerate);
    c5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    c5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    c5_env.SetMax(1);
    c5_env.SetMin(0);

    d5_env.Init(samplerate);
    d5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    d5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    d5_env.SetMax(1);
    d5_env.SetMin(0);

    e5_env.Init(samplerate);
    e5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    e5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    e5_env.SetMax(1);
    e5_env.SetMin(0);

    f5_env.Init(samplerate);
    f5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    f5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    f5_env.SetMax(1);
    f5_env.SetMin(0);

    g5_env.Init(samplerate);
    g5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    g5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    g5_env.SetMax(1);
    g5_env.SetMin(0);

    a5_env.Init(samplerate);
    a5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    a5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    a5_env.SetMax(1);
    a5_env.SetMin(0);

    b5_env.Init(samplerate);
    b5_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    b5_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    b5_env.SetMax(1);
    b5_env.SetMin(0);

    c6_env.Init(samplerate);
    c6_env.SetTime(ADENV_SEG_ATTACK, ENV_ATK_TIME);
    c6_env.SetTime(ADENV_SEG_DECAY, ENV_DECAY_TIME);
    c6_env.SetMax(1);
    c6_env.SetMin(0);

    //wait 2 seconds for MPR121 to get baseline values
    System::Delay(2000);
    //Start calling the callback function
    hw.StartAudio(AudioCallback);

    // Loop forever
    while(1)
    {
	hw.PrintLine("At top of While loop");
   	hw.SetLed(readTouch(&i2c) > 0);  
	//wait for touch
	touch_hi = touch_lo = 0;
	while(readTouch(&i2c) == 0)
	{
		System::Delay(64);
	}
	i2c.ReadDataAtAddress(MPR121_ADDR, TOUCH_STATUS_LO, 1, &touch_lo, 1, MAX_I2C_WAIT);
    	hw.PrintLine("Read from TOUCH_REG_LO : 0x%X", touch_lo);
	i2c.ReadDataAtAddress(MPR121_ADDR, TOUCH_STATUS_HI, 1, &touch_hi, 1, MAX_I2C_WAIT);
    	hw.PrintLine("Read from TOUCH_REG_HI: 0x%X", touch_hi);
	System::Delay(64);
    }
}
