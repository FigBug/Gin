/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void WTOscillator::noteOn (float p)
{
    p >= 0 ? phaseL = p : juce::Random::getSystemRandom().nextFloat();
    phaseR = phaseL;

    tableIndexL = -1;
    tableIndexR = -1;
}

void WTOscillator::process (float note, const Params& params, juce::AudioSampleBuffer& buffer)
{
    if (bllt == nullptr && bllt->size() == 0) return;

    if (tableIndexL == -1 || tableIndexL >= bllt->size())
        tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

    float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
    float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto s = bllt->getUnchecked (tableIndexL)->process (note, phaseDistortion (phaseL, params.bend, params.formant));

        *l++ = s * params.leftGain;
        *r++ = s * params.rightGain;

        phaseL += delta;
        while (phaseL >= 1.0f)
        {
            tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
            phaseL -= 1.0f;
        }
    }
    phaseR = phaseL;
}

void WTOscillator::process (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer)
{
    if (bllt == nullptr && bllt->size() == 0) return;

    if (tableIndexL == -1 || tableIndexL >= bllt->size() || tableIndexR >= bllt->size())
        tableIndexL = tableIndexR = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

    float freqL = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteL - 69.0) / 12.0)));
    float freqR = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteR - 69.0) / 12.0)));
    float deltaL = 1.0f / (float ((1.0f / freqL) * sampleRate));
    float deltaR = 1.0f / (float ((1.0f / freqR) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto sL = bllt->getUnchecked (tableIndexL)->process (noteL, phaseDistortion (phaseL, params.bend, params.formant));
        auto sR = bllt->getUnchecked (tableIndexR)->process (noteR, phaseDistortion (phaseR, params.bend, params.formant));
        *l++ = sL * params.leftGain;
        *r++ = sR * params.rightGain;

        phaseL += deltaL;
        phaseR += deltaR;
        while (phaseL >= 1.0f)
        {
            phaseL -= 1.0f;
            tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
        }
        while (phaseR >= 1.0f)
        {
            phaseR -= 1.0f;
            tableIndexR = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
        }
    }
}

void WTOscillator::processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer)
{
    if (bllt == nullptr && bllt->size() == 0) return;

    if (tableIndexL == -1 || tableIndexL >= bllt->size())
        tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

    float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
    float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto s = bllt->getUnchecked (tableIndexL)->process (note, phaseDistortion (phaseL, params.bend, params.formant));
        *l++ += s * params.leftGain;
        *r++ += s * params.rightGain;

        phaseL += delta;
        while (phaseL >= 1.0f)
        {
            phaseL -= 1.0f;
            tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
        }
    }
    phaseR = phaseL;
}

void WTOscillator::processAdding (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer)
{
    if (bllt == nullptr && bllt->size() == 0) return;

    if (tableIndexL == -1 || tableIndexL >= bllt->size() || tableIndexR >= bllt->size())
        tableIndexL = tableIndexR = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

    float freqL = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteL - 69.0) / 12.0)));
    float freqR = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteR - 69.0) / 12.0)));
    float deltaL = 1.0f / (float ((1.0f / freqL) * sampleRate));
    float deltaR = 1.0f / (float ((1.0f / freqR) * sampleRate));

    int samps = buffer.getNumSamples();
    auto l = buffer.getWritePointer (0);
    auto r = buffer.getWritePointer (1);

    for (int i = 0; i < samps; i++)
    {
        auto sL = bllt->getUnchecked (tableIndexL)->process (noteL, phaseDistortion (phaseL, params.bend, params.formant));
        auto sR = bllt->getUnchecked (tableIndexR)->process (noteR, phaseDistortion (phaseR, params.bend, params.formant));

        *l++ += sL * params.leftGain;
        *r++ += sR * params.rightGain;

        phaseL += deltaL;
        phaseR += deltaR;
        while (phaseL >= 1.0f)
        {
            phaseL -= 1.0f;
            tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
        }
        while (phaseR >= 1.0f)
        {
            phaseR -= 1.0f;
            tableIndexR = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
        }
    }
}

void WTOscillator::setWavetable (juce::OwnedArray<BandLimitedLookupTable>* table)
{
    bllt = table;
}

bool loadWavetables (juce::OwnedArray<BandLimitedLookupTable>& bllt, double playbackSampleRate, juce::AudioSampleBuffer& buffer, double fileSampleRate, int tableSize)
{
    bllt.clear();

    int numTables = buffer.getNumSamples() / tableSize;

    for (int i = 0; i < numTables; i++)
    {
        auto slice = sliceBuffer (buffer, i * tableSize, tableSize);

        auto table = new BandLimitedLookupTable();
        table->loadFromBuffer (float (playbackSampleRate), slice, float (fileSampleRate), 6);
        bllt.add (table);
    }
    return true;
}
