/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Audio sample container for loading and playing back sample data.

    Sample holds audio data loaded from files and provides methods for reading
    the sample data. It's designed to work with SampleOscillator for pitch-shifted
    playback and supports storing sample metadata like root note and sample rate.

    Key Features:
    - Load samples from audio files
    - Store multi-channel audio data
    - Sample rate and root note metadata
    - In/out marker support
    - Efficient sample reading

    The root note is used by SampleOscillator to determine pitch shift amounts
    when playing the sample at different MIDI notes.

    Usage:
    @code
    auto sample = Sample::fromFile(File("/path/to/sample.wav"));

    if (sample)
    {
        DBG("Length: " + String(sample->getLength()));
        DBG("Root note: " + String(sample->getRootNote()));
        DBG("Sample rate: " + String(sample->getSampleRate()));

        // Read portion of sample
        AudioBuffer<float> buffer(2, 512);
        sample->read(buffer, 0); // Read from sample position 0

        // Or use with SampleOscillator
        SampleOscillator osc;
        osc.setSample(sample.get());
        osc.noteOn();
    }
    @endcode

    @see SampleOscillator
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
