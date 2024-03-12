#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed             hw;
VariableSawOscillator varisaw;
Oscillator            lfo;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    varisaw.SetFreq(hw.adc.GetFloat(0) * 300);
    lfo.SetFreq(hw.adc.GetFloat(1) * 40);
    for(size_t i = 0; i < size; i++)
    {
        float sig = lfo.Process();
        varisaw.SetPW(sig);

        out[0][i] = out[1][i] = varisaw.Process();
    }
}

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sample_rate = hw.AudioSampleRate();

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
    while(1) {}
}
