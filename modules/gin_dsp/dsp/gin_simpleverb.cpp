/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

SimpleVerb::SimpleVerb()
{
    roomSizeFader = 0.5;
    roomSize = 55;

    preDelayFader = 0;

    preDelayLength = 0;
    preDelayPos = 0;

    dampFader = 0.5;
    damp = 0.25;

    freqLPFader = 1;
    freqHPFader = 0;

    freqLP = 24000;
    freqHP = 0;

    b1LP = -std::exp (-2.0f * juce::MathConstants<float>::pi * freqLP / sampleRate); // 100Hz
    a0LP = 1.0f + b1LP;

    b1HP = -std::exp (-2.0f * juce::MathConstants<float>::pi * freqHP / sampleRate); // 100Hz
    a0HP = 1.0f + b1HP;

    dry = 1;
    wet = 0.5;
    
    setSampleRate (44100);
}

void SimpleVerb::setSampleRate (float sr)
{
    constexpr float roomMaxSize = 100.0f;

    sampleRate = sr;
    
    auto comb1MaxLength = static_cast<unsigned int>(C1 * roomMaxSize * sampleRate / 1000);
    comb1.resize (comb1MaxLength);
    auto comb2MaxLength = static_cast<unsigned int>(C2 * roomMaxSize * sampleRate / 1000);
    comb2.resize (comb2MaxLength);
    auto comb3MaxLength = static_cast<unsigned int>(C3 * roomMaxSize * sampleRate / 1000);
    comb3.resize (comb3MaxLength);
    auto comb4MaxLength = static_cast<unsigned int>(C4 * roomMaxSize * sampleRate / 1000);
    comb4.resize (comb4MaxLength);
    auto comb5MaxLength = static_cast<unsigned int>(C5 * roomMaxSize * sampleRate / 1000);
    comb5.resize (comb5MaxLength);
    auto comb6MaxLength = static_cast<unsigned int>(C6 * roomMaxSize * sampleRate / 1000);
    comb6.resize (comb6MaxLength);
    auto comb7MaxLength = static_cast<unsigned int>(C7 * roomMaxSize * sampleRate / 1000);
    comb7.resize (comb7MaxLength);
    auto comb8MaxLength = static_cast<unsigned int>(C8 * roomMaxSize * sampleRate / 1000);
    comb8.resize (comb8MaxLength);
    auto comb9MaxLength = static_cast<unsigned int>(C9 * roomMaxSize * sampleRate / 1000);
    comb9.resize (comb9MaxLength);
    auto comb10MaxLength = static_cast<unsigned int>(C10 * roomMaxSize * sampleRate / 1000);
    comb10.resize (comb10MaxLength);
    auto comb11MaxLength = static_cast<unsigned int>(C11 * roomMaxSize * sampleRate / 1000);
    comb11.resize (comb11MaxLength);
    auto comb12MaxLength = static_cast<unsigned int>(C12 * roomMaxSize * sampleRate / 1000);
    comb12.resize (comb12MaxLength);

    allpassL1Length = static_cast<unsigned int>(AL1 * sampleRate / 1000);
    allpassL1.resize (allpassL1Length);
    allpassL2Length = static_cast<unsigned int>((AL2 + SW) * sampleRate / 1000);
    allpassL2.resize (allpassL2Length);
    allpassL3Length = static_cast<unsigned int>(AL3 * sampleRate / 1000);
    allpassL3.resize (allpassL3Length);

    allpassR1Length = static_cast<unsigned int>((AR1 + SW) * sampleRate / 1000);
    allpassR1.resize (allpassR1Length);
    allpassR2Length = static_cast<unsigned int>(AR2 * sampleRate / 1000);
    allpassR2.resize (allpassR2Length);
    allpassR3Length = static_cast<unsigned int>((AR3 + SW) * sampleRate / 1000);
    allpassR3.resize (allpassR3Length);
    
    auto preDelayMaxLength = static_cast<unsigned int>(250 * sampleRate / 1000);
    preDelay.resize (preDelayMaxLength);

    flushPreDelay();
    flushBuffers();

    allpassL1Pos = allpassL2Pos = allpassL3Pos = allpassR1Pos = allpassR2Pos = allpassR3Pos = 0;
    tmp1LP = tmp2LP = tmp1HP = tmp2HP = 0;

    comb1Pos = comb2Pos = comb3Pos = comb4Pos = comb5Pos = comb6Pos = comb7Pos = comb8Pos = 0;
    comb9Pos = comb10Pos = comb11Pos = comb12Pos = 0;
    preDelayPos = 0;
}

void SimpleVerb::flushPreDelay()
{
    std::fill (preDelay.begin(), preDelay.end(), 0.0f);
}

void SimpleVerb::flushBuffers()
{
    std::fill (comb1.begin(), comb1.end(), 0.0f);
    std::fill (comb2.begin(), comb2.end(), 0.0f);
    std::fill (comb3.begin(), comb3.end(), 0.0f);
    std::fill (comb4.begin(), comb4.end(), 0.0f);
    std::fill (comb5.begin(), comb5.end(), 0.0f);
    std::fill (comb6.begin(), comb6.end(), 0.0f);
    std::fill (comb7.begin(), comb7.end(), 0.0f);
    std::fill (comb8.begin(), comb8.end(), 0.0f);
    std::fill (comb9.begin(), comb9.end(), 0.0f);
    std::fill (comb10.begin(), comb10.end(), 0.0f);
    std::fill (comb11.begin(), comb11.end(), 0.0f);
    std::fill (comb12.begin(), comb12.end(), 0.0f);
    std::fill (allpassL1.begin(), allpassL1.end(), 0.0f);
    std::fill (allpassL2.begin(), allpassL2.end(), 0.0f);
    std::fill (allpassL3.begin(), allpassL3.end(), 0.0f);
    std::fill (allpassR1.begin(), allpassR1.end(), 0.0f);
    std::fill (allpassR2.begin(), allpassR2.end(), 0.0f);
    std::fill (allpassR3.begin(), allpassR3.end(), 0.0f);
}

void SimpleVerb::setParameters (float roomIn, float dampIn, float preDelayIn, float lpFaderIn, float hpFaderIn, float wetIn, float dryIn)
{
    if (! juce::approximatelyEqual (roomIn, roomSizeFader))
    {
        roomSizeFader = roomIn;
        roomSize = 5 + roomSizeFader * roomSizeFader * 95;
        
        comb1Length = static_cast<unsigned int>(C1 * roomSize * sampleRate / 1000);
        comb1Pos = 0;

        comb2Length = static_cast<unsigned int>(C2 * roomSize * sampleRate / 1000);
        comb2Pos = 0;

        comb3Length = static_cast<unsigned int>(C3 * roomSize * sampleRate / 1000);
        comb3Pos = 0;

        comb4Length = static_cast<unsigned int>(C4 * roomSize * sampleRate / 1000);
        comb4Pos = 0;

        comb5Length = static_cast<unsigned int>(C5 * roomSize * sampleRate / 1000);
        comb5Pos = 0;

        comb6Length = static_cast<unsigned int>(C6 * roomSize * sampleRate / 1000);
        comb6Pos = 0;

        comb7Length = static_cast<unsigned int>(C7 * roomSize * sampleRate / 1000);
        comb7Pos = 0;

        comb8Length = static_cast<unsigned int>(C8 * roomSize * sampleRate / 1000);
        comb8Pos = 0;

        comb9Length = static_cast<unsigned int>(C9 * roomSize * sampleRate / 1000);
        comb9Pos = 0;

        comb10Length = static_cast<unsigned int>(C10 * roomSize * sampleRate / 1000);
        comb10Pos = 0;

        comb11Length = static_cast<unsigned int>(C11 * roomSize * sampleRate / 1000);
        comb11Pos = 0;

        comb12Length = static_cast<unsigned int>(C12 * roomSize * sampleRate / 1000);
        comb12Pos = 0;

        flushBuffers();
    }
    if (! juce::approximatelyEqual (dampIn, dampFader))
    {
        dampFader = dampIn;
        damp = std::min (1.0f - dampFader * dampFader, 0.95f);
    }
    if (! juce::approximatelyEqual (preDelayIn, preDelayFader))
    {
        preDelayFader = preDelayIn;
        preDelayLength = static_cast<unsigned int>(preDelayFader * preDelayFader * 250 * sampleRate / 1000);
        preDelayPos = 0;
        flushPreDelay();
    }
    if  (! juce::approximatelyEqual (lpFaderIn, freqLPFader))
    {
        freqLPFader = lpFaderIn;
        freqLP = freqLPFader * freqLPFader * freqLPFader * 24000;
        b1LP = -std::exp (-2.0f * juce::MathConstants<float>::pi * freqLP/ sampleRate); // 100Hz
        a0LP = 1.0f + b1LP;
    }
    if (! juce::approximatelyEqual (hpFaderIn, freqHPFader))
    {
        freqHPFader = hpFaderIn;
        freqHP = freqHPFader * freqHPFader * freqHPFader * 24000;
        b1HP = -std::exp (-2.0f * juce::MathConstants<float>::pi * freqHP / sampleRate); // 100Hz
        a0HP = 1.0f + b1HP;
    }
    if (! juce::approximatelyEqual (dryIn, dryFader))
    {
        dryFader = dryIn;
        dry = dryFader * 2;
    }
    if (! juce::approximatelyEqual (wetIn, wetFader))
    {
        wetFader = wetIn;
        wet = wetFader * 2;
    }
}

void SimpleVerb::process (juce::AudioSampleBuffer& buffer)
{
    const float* in1  = buffer.getReadPointer (0);
    const float* in2  = buffer.getReadPointer (1);
    float* out1 = buffer.getWritePointer (0);
    float* out2 = buffer.getWritePointer (1);

    int sampleFrames = buffer.getNumSamples();

    while (--sampleFrames >= 0)
    {
        if (preDelayLength <= 1)
        {
            reverb = ((*in1) + (*in2)) / (1 + damp) + cDC_;
        }
        else
        {
            preDelay[preDelayPos] = ((*in1) + (*in2)) / (1 + damp) + cDC_;
            if (++preDelayPos >= preDelayLength)
                preDelayPos = 0;

            reverb = preDelay[preDelayPos];
        }

        comb1[comb1Pos]   = reverb * 0.49f +  comb1[comb1Pos]  * damp;
        comb2[comb2Pos]   = reverb * 0.76f +  comb2[comb2Pos]  * damp;
        comb3[comb3Pos]   = reverb * 1.00f +  comb3[comb3Pos]  * damp;
        comb4[comb4Pos]   = reverb * 0.91f +  comb4[comb4Pos]  * damp;
        comb5[comb5Pos]   = reverb * 0.79f +  comb5[comb5Pos]  * damp;
        comb6[comb6Pos]   = reverb * 0.71f +  comb6[comb6Pos]  * damp;
        comb7[comb7Pos]   = reverb * 0.59f +  comb7[comb7Pos]  * damp;
        comb8[comb8Pos]   = reverb * 0.51f +  comb8[comb8Pos]  * damp;
        comb9[comb9Pos]   = reverb * 0.42f +  comb9[comb9Pos]  * damp;
        comb10[comb10Pos] = reverb * 0.38f + comb10[comb10Pos] * damp;
        comb11[comb11Pos] = reverb * 0.35f + comb11[comb11Pos] * damp;
        comb12[comb12Pos] = reverb * 0.30f + comb12[comb12Pos] * damp;

        if (++comb1Pos  >= comb1Length)  comb1Pos = 0;
        if (++comb2Pos  >= comb2Length)  comb2Pos = 0;
        if (++comb3Pos  >= comb3Length)  comb3Pos = 0;
        if (++comb4Pos  >= comb4Length)  comb4Pos = 0;
        if (++comb5Pos  >= comb5Length)  comb5Pos = 0;
        if (++comb6Pos  >= comb6Length)  comb6Pos = 0;
        if (++comb7Pos  >= comb7Length)  comb7Pos = 0;
        if (++comb8Pos  >= comb8Length)  comb8Pos = 0;
        if (++comb9Pos  >= comb9Length)  comb9Pos = 0;
        if (++comb10Pos >= comb10Length) comb10Pos = 0;
        if (++comb11Pos >= comb11Length) comb11Pos = 0;
        if (++comb12Pos >= comb12Length) comb12Pos = 0;

        reverb = (comb1[comb1Pos]
                  + comb2[comb2Pos]
                  + comb3[comb3Pos]
                  + comb4[comb4Pos]
                  + comb5[comb5Pos]
                  + comb6[comb6Pos]
                  + comb7[comb7Pos]
                  + comb8[comb8Pos]
                  + comb9[comb9Pos]
                  + comb10[comb10Pos]
                  + comb11[comb11Pos]
                  + comb12[comb12Pos]);
        
        jassert (! std::isnan (reverb) && ! std::isinf (reverb));

        allpassL1[allpassL1Pos] = reverb + allpassL1[allpassL1Pos] * AP1FBQ;
        left = (reverb - allpassL1[allpassL1Pos] * AP1FBQ);
        jassert (! std::isnan (left) && ! std::isinf (left));
        if (++allpassL1Pos >= allpassL1Length)
            allpassL1Pos = 0;
        
        allpassL2[allpassL2Pos] = left + allpassL2[allpassL2Pos] * AP2FBQ;
        left = (left - allpassL2[allpassL2Pos] * AP2FBQ);
        jassert (! std::isnan (left) && ! std::isinf (left));
        if (++allpassL2Pos >= allpassL2Length)
            allpassL2Pos = 0;
        
        allpassL3[allpassL3Pos] = left + allpassL3[allpassL3Pos] * AP3FBQ;
        left = (left - allpassL3[allpassL3Pos] * AP3FBQ);
        jassert (! std::isnan (left) && ! std::isinf (left));
        if (++allpassL3Pos >= allpassL3Length)
            allpassL3Pos = 0;

        allpassR1[allpassR1Pos] = reverb + allpassR1[allpassR1Pos] * AP1FBQ;
        right = (reverb - allpassR1[allpassR1Pos] * AP1FBQ);
        jassert (! std::isnan (right) && ! std::isinf (right));
        if (++allpassR1Pos >= allpassR1Length)
            allpassR1Pos = 0;
        
        allpassR2[allpassR2Pos] = right + allpassR2[allpassR2Pos] * AP2FBQ;
        right = (right - allpassR2[allpassR2Pos] * AP2FBQ);
        jassert (! std::isnan (right) && ! std::isinf (right));
        if (++allpassR2Pos >= allpassR2Length)
            allpassR2Pos = 0;
        
        allpassR3[allpassR3Pos] = right + allpassR3[allpassR3Pos] * AP3FBQ;
        right = (right - allpassR3[allpassR3Pos] * AP3FBQ);
        jassert (! std::isnan (right) && ! std::isinf (right));
        if (++allpassR3Pos >= allpassR3Length)
            allpassR3Pos = 0;

        if (! juce::approximatelyEqual (freqHPFader, 0.0f))
        {
            left -= (tmp1HP = a0HP * left - b1HP * tmp1HP + cDC_) - cDC_;
            right -= (tmp2HP = a0HP * right - b1HP * tmp2HP + cDC_) - cDC_;
            
            jassert (! std::isnan (left) && ! std::isinf (left));
            jassert (! std::isnan (right) && ! std::isinf (right));
        }
        if (! juce::approximatelyEqual (freqLPFader, 1.0f))
        {
            left = (tmp1LP = a0LP * left - b1LP * tmp1LP + cDC_) - cDC_;
            right = (tmp2LP = a0LP * right - b1LP * tmp2LP + cDC_) - cDC_;
            
            jassert (! std::isnan (left) && ! std::isinf (left));
            jassert (! std::isnan (right) && ! std::isinf (right));
        }

        (*out1++) = (*in1++) * dry + left * wet;
        (*out2++) = (*in2++) * dry + right * wet;
    }
}
