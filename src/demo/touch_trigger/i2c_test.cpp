#include "daisy_seed.h"
#include "daisysp.h"
#include <src/per/i2c.h>
#include <cstdio>
#include "mpr121.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

DaisySeed hw;

Oscillator osc;
WhiteNoise noise;

AdEnv kickVolEnv, kickPitchEnv, snareEnv;
uint8_t touch_lo, touch_hi = 0;


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    float osc_out, noise_out, snr_env_out, kck_env_out, sig;

    //If you press the kick button...
    if(touch_lo)
    {
        //Trigger both envelopes!
        kickVolEnv.Trigger();
        kickPitchEnv.Trigger();
    }

    //If press the snare button trigger its envelope
    if(touch_hi)
    {
        snareEnv.Trigger();
    }

    //Prepare the audio block
    for(size_t i = 0; i < size; i += 2)
    {
        //Get the next volume samples
        snr_env_out = snareEnv.Process();
        kck_env_out = kickVolEnv.Process();

        //Apply the pitch envelope to the kick
        osc.SetFreq(kickPitchEnv.Process());
        //Set the kick volume to the envelope's output
        osc.SetAmp(kck_env_out);
        //Process the next oscillator sample
        osc_out = osc.Process();

        //Get the next snare sample
        noise_out = noise.Process();
        //Set the sample to the correct volume
        noise_out *= snr_env_out;

        //Mix the two signals at half volume
        sig = .5 * noise_out + .5 * osc_out;

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
    hw.StartLog(false);
    hw.PrintLine("Hello from Daisy Seed!");
    hw.SetAudioBlockSize(4);
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

    uint8_t mprdata = 0;
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

    //Initialize oscillator for kickdrum
    osc.Init(samplerate);
    osc.SetWaveform(Oscillator::WAVE_TRI);
    osc.SetAmp(1);

    //Initialize noise
    noise.Init();

    //Initialize envelopes, this one's for the snare amplitude
    snareEnv.Init(samplerate);
    snareEnv.SetTime(ADENV_SEG_ATTACK, .01);
    snareEnv.SetTime(ADENV_SEG_DECAY, .2);
    snareEnv.SetMax(1);
    snareEnv.SetMin(0);

    //This envelope will control the kick oscillator's pitch
    //Note that this envelope is much faster than the volume
    kickPitchEnv.Init(samplerate);
    kickPitchEnv.SetTime(ADENV_SEG_ATTACK, .01);
    kickPitchEnv.SetTime(ADENV_SEG_DECAY, .05);
    kickPitchEnv.SetMax(400);
    kickPitchEnv.SetMin(50);

    //This one will control the kick's volume
    kickVolEnv.Init(samplerate);
    kickVolEnv.SetTime(ADENV_SEG_ATTACK, .01);
    kickVolEnv.SetTime(ADENV_SEG_DECAY, 1);
    kickVolEnv.SetMax(1);
    kickVolEnv.SetMin(0);

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
