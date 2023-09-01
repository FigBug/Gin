/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

class WhiteNoise
{
public:
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


/*
 * A very simple and very fast implementation of the Voss-McCartney pink-noise approximation algorithm
 * using a middle square weyl PRNG and a look-up table for octave-sequencing
 *
 * LICENSE:
 *
 * Copyright 2020 Thomas Merchant
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
 *
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
