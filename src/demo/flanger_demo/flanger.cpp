#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed             hw;
VariableSawOscillator varisaw;
Oscillator            lfo;
//Overdrive	      drive;
Flanger			flange;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    varisaw.SetFreq( hw.adc.GetFloat(0) * 300 );
    lfo.SetFreq( hw.adc.GetFloat(1) * 40 );
    flange.SetFeedback( hw.adc.GetFloat(2) );
    for(size_t i = 0; i < size; i++)
    {
        float sig = lfo.Process();
        varisaw.SetPW(sig);

        out[0][i] = out[1][i] = flange.Process( varisaw.Process() );
    }
}

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sample_rate = hw.AudioSampleRate();

    //set up ADC on pin D15 and D16 for our potentiometer
    const int adcChannels = 3;
    AdcChannelConfig adcConfig[adcChannels];
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    adcConfig[2].InitSingle(hw.GetPin(17));
    hw.adc.Init(adcConfig, adcChannels);

    varisaw.Init(sample_rate);
    varisaw.SetFreq(62.5f);
    varisaw.SetWaveshape(1.f);
    hw.adc.Start();

    lfo.Init(sample_rate);
    lfo.SetAmp(1.f);
    lfo.SetFreq(1.f);

    flange.Init(2E3);
//    flange.SetFeedback(0.5);
//    flange.SetLfoDepth(0.5);
//    flange.SetLfoFreq(10.0);

    hw.StartAudio(AudioCallback);
    while(1) {}
}
