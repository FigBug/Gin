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

    unsigned int preDelayPos, preDelayLength;

    std::vector<float> preDelay;
    float preDelayFader;

    unsigned int comb1Pos, comb1Length;
    std::vector<float> comb1;
    unsigned int comb2Pos, comb2Length;
    std::vector<float> comb2;
    unsigned int comb3Pos, comb3Length;
    std::vector<float> comb3;
    unsigned int comb4Pos, comb4Length;
    std::vector<float> comb4;
    unsigned int comb5Pos, comb5Length;
    std::vector<float> comb5;
    unsigned int comb6Pos, comb6Length;
    std::vector<float> comb6;
    unsigned int comb7Pos, comb7Length;
    std::vector<float> comb7;
    unsigned int comb8Pos, comb8Length;
    std::vector<float> comb8;
    unsigned int comb9Pos, comb9Length;
    std::vector<float> comb9;
    unsigned int comb10Pos, comb10Length;
    std::vector<float> comb10;
    unsigned int comb11Pos, comb11Length;
    std::vector<float> comb11;
    unsigned int comb12Pos, comb12Length;
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

    static constexpr float C1   = 1.00f;
    static constexpr float C2   = 1.09f;
    static constexpr float C3   = 1.16f;
    static constexpr float C4   = 1.23f;
    static constexpr float C5   = 1.32f;
    static constexpr float C6   = 1.41f;
    static constexpr float C7   = 1.45f;
    static constexpr float C8   = 1.56f;
    static constexpr float C9   = 1.66f;
    static constexpr float C10  = 1.71f;
    static constexpr float C11  = 1.80f;
    static constexpr float C12  = 1.90f;

    static constexpr float AL1  = 1.0f;
    static constexpr float AL2  = 2.5f;
    static constexpr float AL3  = 5.0f;
    static constexpr float AR1  = 1.0f;
    static constexpr float AR2  = 2.5f;
    static constexpr float AR3  = 5.0f;

    static constexpr float SW   = 1.0f;

    static constexpr float AP1FBQ  = 0.6f;
    static constexpr float AP2FBQ  = 0.6f;
    static constexpr float AP3FBQ  = 0.6f;
};
