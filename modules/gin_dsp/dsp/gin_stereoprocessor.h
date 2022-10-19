/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/**  Stereo Processor

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

class StereoProcessor
{
public:
    StereoProcessor();
    void process (juce::AudioSampleBuffer& buffer);

    //
    // width1        [ 0 - 1 ]
    // centerFader1  [ 0 - 1 ]
    // pan1          [-1 - 1 ]
    // rotationFader [ 0 - 1 ]
    // pan2          [-1 - 1 ]
    // centerFader2  [ 0 - 1 ]
    // width2        [ 0 - 1 ]
    // outVolFader   [ 0 - 1 ]
    //
    void setParameters (float width1, float centerFader1, float pan1, float rotationFader, float pan2, float centerFader2, float width2, float outVolFader);

private:
    float rotation = 0.0f;

    float width1 = 0.0f, widthCoeff1 = 0.0f;
    float center1 = 0.0f, side1 = 0.0f;
    float width2 = 0.0f, widthCoeff2 = 0.0f;
    float center2 = 0.0f, side2 = 0.0f;

    float pan1Left = 0.0f, pan1Right = 0.0f;
    float pan2Left = 0.0f, pan2Right = 0.0f;

    float outVol = 0.0f;
};
