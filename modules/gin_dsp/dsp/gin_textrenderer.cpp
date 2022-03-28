/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2021 - Roland Rabien.

 ==============================================================================
 */


juce::String TextRenderer::renderBuffer (const juce::AudioSampleBuffer& buffer, int height, float numSamplesPerPixel)
{
    juce::String txt;
    txt.preallocateBytes (size_t (float ( buffer.getNumChannels() * buffer.getNumSamples() ) / numSamplesPerPixel + 1000));

    for (auto ch = 0; ch < buffer.getNumChannels(); ch++)
    {
        // Average data
        auto samples = buffer.getReadPointer (ch);

        int numSamples = buffer.getNumSamples();

        std::vector<float> values;

        float currentAve = 0;
        float numLeftToAverage = numSamplesPerPixel;
        int numAveraged = 0;

        while (--numSamples >= 0)
        {
            const float currentSample = *samples++;

            currentAve += currentSample;
            numAveraged++;

            if (--numLeftToAverage <= 0)
            {
                values.push_back (currentAve / float ( numAveraged ));

                currentAve = 0.0;

                numLeftToAverage += std::max (1.0f, numSamplesPerPixel);
                numAveraged = 0;
            }
        }

        // Draw channel
        for (int row = 0; row < height; row++)
        {
            for (auto v : values)
            {
                auto vi = int (std::round ((v + 1.0f) / 2.0f * (float ( height ) - 1)));

                if (vi == row)
                    txt += "*";
                else
                    txt += " ";
            }
            txt += "\n";
        }
        if (ch < buffer.getNumChannels() - 1)
            txt += "\n";
    }

    return txt;
}

#if JUCE_DEBUG
void TextRenderer::debugPrintBuffer (const juce::AudioSampleBuffer& buffer, int height, float samplesPerChar)
{
    DBG (renderBuffer (buffer, height, samplesPerChar));
}
#endif
