/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void StereoOscillator::noteOn (float p)
{
    p >= 0 ? phaseL = p : juce::Random::getSystemRandom().nextFloat();
    phaseR = phaseL;
}

void StereoOscillator::process (float note, const Params& params, juce::AudioSampleBuffer& buffer)
{
    float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
    float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto s = bllt.process (params.wave, note, phaseL, params.pw);
        *l++ = s * params.leftGain;
        *r++ = s * params.rightGain;

        phaseL += delta;
        while (phaseL >= 1.0f)
            phaseL -= 1.0f;
    }
    phaseR = phaseL;
}

void StereoOscillator::process (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer)
{
    float freqL = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteL - 69.0) / 12.0)));
    float freqR = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteR - 69.0) / 12.0)));
    float deltaL = 1.0f / (float ((1.0f / freqL) * sampleRate));
    float deltaR = 1.0f / (float ((1.0f / freqR) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto sL = bllt.process (params.wave, noteL, phaseL, params.pw);
        auto sR = bllt.process (params.wave, noteR, phaseL, params.pw);
        *l++ = sL * params.leftGain;
        *r++ = sR * params.rightGain;

        phaseL += deltaL;
        phaseR += deltaR;
        while (phaseL >= 1.0f) phaseL -= 1.0f;
        while (phaseR >= 1.0f) phaseR -= 1.0f;
    }
}

void StereoOscillator::processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer)
{
    float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
    float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto s = bllt.process (params.wave, note, phaseL, params.pw);
        *l++ += s * params.leftGain;
        *r++ += s * params.rightGain;

        phaseL += delta;
        while (phaseL >= 1.0f)
            phaseL -= 1.0f;
    }
    phaseR = phaseL;
}

void StereoOscillator::processAdding (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer)
{
    float freqL = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteL - 69.0) / 12.0)));
    float freqR = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteR - 69.0) / 12.0)));
    float deltaL = 1.0f / (float ((1.0f / freqL) * sampleRate));
    float deltaR = 1.0f / (float ((1.0f / freqR) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto sL = bllt.process (params.wave, noteL, phaseL, params.pw);
        auto sR = bllt.process (params.wave, noteR, phaseR, params.pw);

        *l++ += sL * params.leftGain;
        *r++ += sR * params.rightGain;

        phaseL += deltaL;
        phaseR += deltaR;
        while (phaseL >= 1.0f) phaseL -= 1.0f;
        while (phaseR >= 1.0f) phaseR -= 1.0f;
    }
}
