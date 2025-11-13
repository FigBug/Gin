/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/**
    White noise generator using a normal distribution.

    WhiteNoise produces random samples with a flat frequency spectrum (equal power
    across all frequencies). Uses C++ standard library's normal distribution with
    mean 0.0 and standard deviation 0.1.

    White noise characteristics:
    - Flat frequency spectrum (equal energy at all frequencies)
    - Gaussian amplitude distribution
    - Mean of 0.0, standard deviation of 0.1
    - No correlation between samples

    Usage:
    @code
    WhiteNoise noise;

    // In audio processing loop
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = noise.nextSample();
        audioBuffer[i] = sample;
    }
    @endcode

    Note: Each instance is seeded with a random device, ensuring different
    sequences across instances.

    @see PinkNoise
*/
class WhiteNoise
{
public:
    WhiteNoise()
    {
        std::random_device rd;
        generator.seed (rd());
    }

    inline float nextSample()
    {
        return dist (generator);
    }
    
private:
    const float mean = 0.0f;
    const float stddev = 0.1f;

    std::default_random_engine generator;
    std::normal_distribution<float> dist {mean, stddev};
};


/**
    Pink noise generator using the Voss-McCartney algorithm.

    PinkNoise produces random samples with a 1/f frequency spectrum (power decreases
    3dB per octave). This implementation uses the Voss-McCartney approximation with
    a middle square Weyl PRNG and octave-sequencing lookup table for efficiency.

    Pink noise characteristics:
    - 1/f power spectrum (3dB/octave roll-off)
    - Equal energy per octave
    - More natural-sounding than white noise
    - Efficient Voss-McCartney approximation

    Implementation details:
    - Uses 9 octave bands for frequency spectrum shaping
    - Middle square Weyl PRNG for random number generation
    - Lookup table for determining which octave to update
    - Very fast execution suitable for real-time audio

    Usage:
    @code
    PinkNoise noise;

    // In audio processing loop
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = noise.nextSample();
        audioBuffer[i] = sample;
    }
    @endcode

    LICENSE:
    Copyright 2020 Thomas Merchant

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE

    @see WhiteNoise
*/
class PinkNoise
{
public:
    PinkNoise()
    {
        rand.s  = 0x2252e7cd98846fdd;
        rand.x  = 0;
        rand.w  = 0;
        counter = 0;
        
        memset (octave_hold_vals, 0, 9 * 4);
        out = 0;
    }
    
    float nextSample()
    {
        uint8_t octave = trailing_zeros[counter];
        
        out -= octave_hold_vals[octave];
        octave_hold_vals[octave] = (float)next_int() / (float)INT32_MAX;
        octave_hold_vals[octave] /= (float)(10 - octave);
        out += octave_hold_vals[octave];
        
        counter++;
        
        return out;
    }
    
private:
    struct middle_square_weyl
    {
        uint64_t s;
        uint64_t x;
        uint64_t w;
    };
    
    int32_t next_int()
    {
        rand.x *= rand.x;
        rand.x += (rand.w += rand.s);
        rand.x = (rand.x >> 32u) | (rand.x << 32u);
        return int32_t (rand.x);
    }
    
    uint8_t trailing_zeros[256] = {8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,};
    
    middle_square_weyl rand;
    float out;
    float octave_hold_vals[9];
    uint8_t counter;
};
