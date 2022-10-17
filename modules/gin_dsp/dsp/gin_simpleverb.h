/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/**  Simple Reverb

     Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn

     Permission to use, copy, modify, and/or distribute this software for any
     purpose with or without fee is hereby granted, provided that the above
     copyright notice and this permission notice appear in all copies.

     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
     WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
     MERCHANTABILITY, FITNESS AND NONINFRINGEMENT. IN NO EVENT SHALL THE
     AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
     DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
     PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
     ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
     THIS SOFTWARE.
*/

class SimpleVerb
{
public:
    SimpleVerb();

    void setSampleRate (float sr);
    void process (juce::AudioSampleBuffer& buffer);
    void setParameters (float roomIn, float dampIn, float preDelayIn, float lpFaderIn, float hpFaderIn, float wetIn, float dryIn);

private:
    void flushPreDelay();
    void flushBuffers();

    unsigned int preDelayPos, preDelayLength, preDelayMaxLength;

    std::vector<float> preDelay;
    float preDelayFader;

    unsigned int comb1Pos, comb1Length, comb1MaxLength;
    std::vector<float> comb1;
    unsigned int comb2Pos, comb2Length, comb2MaxLength;
    std::vector<float> comb2;
    unsigned int comb3Pos, comb3Length, comb3MaxLength;
    std::vector<float> comb3;
    unsigned int comb4Pos, comb4Length, comb4MaxLength;
    std::vector<float> comb4;
    unsigned int comb5Pos, comb5Length, comb5MaxLength;
    std::vector<float> comb5;
    unsigned int comb6Pos, comb6Length, comb6MaxLength;
    std::vector<float> comb6;
    unsigned int comb7Pos, comb7Length, comb7MaxLength;
    std::vector<float> comb7;
    unsigned int comb8Pos, comb8Length, comb8MaxLength;
    std::vector<float> comb8;
    unsigned int comb9Pos, comb9Length, comb9MaxLength;
    std::vector<float> comb9;
    unsigned int comb10Pos, comb10Length, comb10MaxLength;
    std::vector<float> comb10;
    unsigned int comb11Pos, comb11Length, comb11MaxLength;
    std::vector<float> comb11;
    unsigned int comb12Pos, comb12Length, comb12MaxLength;
    std::vector<float> comb12;

    unsigned int allpassL1Pos, allpassL1Length;
    std::vector<float> allpassL1;
    unsigned int allpassL2Pos, allpassL2Length;
    std::vector<float> allpassL2;
    unsigned int allpassL3Pos, allpassL3Length;
    std::vector<float> allpassL3;

    unsigned int allpassR1Pos, allpassR1Length;
    std::vector<float> allpassR1;
    unsigned int allpassR2Pos, allpassR2Length;
    std::vector<float> allpassR2;
    unsigned int allpassR3Pos, allpassR3Length;
    std::vector<float> allpassR3;

    float reverb, damp, dry, wet, left, right;
    float roomSize = -1.0f;
    float roomSizeFader = -1.0f;
    float dampFader = -1.0f;
    float dryFader = -1.0f;
    float wetFader = -1.0f;
    float sampleRate = 44100.0f;

    float freqLP, freqLPFader;
    float freqHP, freqHPFader;

    float a0LP, b1LP, tmp1LP, tmp2LP;
    float a0HP, b1HP, tmp1HP, tmp2HP;

    static constexpr float cDC_     = 1e-30f;

    static constexpr float $_C1_$   = 1.00f;
    static constexpr float $_C2_$   = 1.09f;
    static constexpr float $_C3_$   = 1.16f;
    static constexpr float $_C4_$   = 1.23f;
    static constexpr float $_C5_$   = 1.32f;
    static constexpr float $_C6_$   = 1.41f;
    static constexpr float $_C7_$   = 1.45f;
    static constexpr float $_C8_$   = 1.56f;
    static constexpr float $_C9_$   = 1.66f;
    static constexpr float $_C10_$  = 1.71f;
    static constexpr float $_C11_$  = 1.80f;
    static constexpr float $_C12_$  = 1.90f;

    static constexpr float $_AL1_$  = 1.0f;
    static constexpr float $_AL2_$  = 2.5f;
    static constexpr float $_AL3_$  = 5.0f;
    static constexpr float $_AR1_$  = 1.0f;
    static constexpr float $_AR2_$  = 2.5f;
    static constexpr float $_AR3_$  = 5.0f;

    static constexpr float $_SW_$   = 1.0f;

    static constexpr float $_AP1FBQ_$  = 0.6f;
    static constexpr float $_AP2FBQ_$  = 0.6f;
    static constexpr float $_AP3FBQ_$  = 0.6f;
};
