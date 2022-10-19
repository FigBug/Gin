/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

StereoEnhancer::StereoEnhancer()
{
}

void StereoEnhancer::setParameters (float widthLP_, float freqHPFader_, float widthHP_)
{
    widthLP = widthLP_;
    widthCoeffLP = std::max (widthLP, 1.0f);

    freqHP = freqHPFader_ * freqHPFader_ * freqHPFader_ * 24000;
    xHP = std::exp (-2.0f * juce::MathConstants<float>::pi * freqHP / sampleRate);
    a0HP = 1.0f - xHP;
    b1HP = -xHP;

    widthHP = widthHP_;
    widthCoeffHP = std::max (widthHP, 1.0f);
}

void StereoEnhancer::process (juce::AudioSampleBuffer& buffer)
{
    constexpr auto cDcAdd = 1e-30f;

    auto in1  = buffer.getReadPointer (0);
    auto in2  = buffer.getReadPointer (1);
    auto out1 = buffer.getWritePointer (0);
    auto out2 = buffer.getWritePointer (1);

    int sampleFrames = buffer.getNumSamples();

    while (--sampleFrames >= 0)
    {
        auto out1HP = *in1;
        auto out2HP = *in2;

        *out1 = (tmp1HP = a0HP * (*in1) - b1HP * tmp1HP + cDcAdd);
        *out2 = (tmp2HP = a0HP * (*in2) - b1HP * tmp2HP + cDcAdd);

        out1HP -= (*out1);
        out2HP -= (*out2);

        auto monoLP   = ((*out1) + (*out2)) / 2.0f;
        auto stereoLP =  (*out1) - (*out2);

        *out1 = (monoLP + stereoLP * widthLP) / widthCoeffLP;
        *out2 = (monoLP - stereoLP * widthLP) / widthCoeffLP;

        auto monoHP   = (out1HP + out2HP) / 2.0f;
        auto stereoHP =  out1HP - out2HP;

        out1HP = (monoHP + stereoHP * widthHP) / widthCoeffHP;
        out2HP = (monoHP - stereoHP * widthHP) / widthCoeffHP;

        *out1 += out1HP;
        *out2 += out2HP;

        out1++;
        out2++;
        in1++;
        in2++;
    }
}
