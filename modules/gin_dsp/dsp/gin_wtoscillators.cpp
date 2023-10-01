/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void WTOscillator::noteOn (float p)
{
    p >= 0 ? phase = p : juce::Random::getSystemRandom().nextFloat();

    tableIndex = -1;
}

void WTOscillator::setWavetable (juce::OwnedArray<BandLimitedLookupTable>* table)
{
    bllt = table;
}

bool loadWavetables (juce::OwnedArray<BandLimitedLookupTable>& bllt, double playbackSampleRate, juce::AudioSampleBuffer& buffer, double fileSampleRate, int tableSize)
{
    std::unique_ptr<juce::dsp::FFT> fft;

    bllt.clear();

    int numTables = buffer.getNumSamples() / tableSize;

    for (int i = 0; i < numTables; i++)
    {
        auto slice = sliceBuffer (buffer, i * tableSize, tableSize);

        auto table = new BandLimitedLookupTable();
        table->loadFromBuffer (fft, float (playbackSampleRate), slice, float (fileSampleRate), 6);
        bllt.add (table);
    }
    return true;
}
