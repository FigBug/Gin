/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/** Holds an audio sample
*/
class Sample
{
public:
    static std::unique_ptr<Sample> fromFile (const juce::File& f);

    int getLength()                         { return length;     }
    int getRootNote()                       { return rootNote;   }
    double getSampleRate()                  { return sampleRate; }
    juce::AudioSampleBuffer& getSamples()   { return data;       }

    void read (juce::AudioSampleBuffer& buffer, int samplePos, int bufferPos = 0, int length = -1);

    void readSamplesAt (int pos, float& lOut, float& rOut)
    {
        if (pos < outMarker + 1 - inMarker)
        {
            lOut = l[pos];
            rOut = r[pos];
        }
        else
        {
            lOut = 0;
            rOut = 0;
        }
    }

private:
    int length = 0, inMarker = 0, outMarker = 0, rootNote = 60;
    double sampleRate = 44100.0;
    const float* l = nullptr;
    const float* r = nullptr;

    juce::AudioSampleBuffer data;
};
