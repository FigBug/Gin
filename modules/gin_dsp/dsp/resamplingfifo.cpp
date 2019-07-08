/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#include "audioutil.h"

ResamplingFifo::ResamplingFifo (int bs, int nc, int ms)
    : numChannels (nc), blockSize (bs),
    inputFifo (nc, ms), outputFifo (nc, ms)
{
    outBuffer.setSize (numChannels, blockSize);
    zeromem (coefficients, sizeof (coefficients));

    prepare();
}

void ResamplingFifo::setResamplingRatio (double inputRate, double outputRate)
{
    ratio = jmax (0.0, inputRate / outputRate);
}

void ResamplingFifo::prepare()
{
    auto scaledBlockSize = roundToInt (1024 * ratio);

    buffer.setSize (numChannels, scaledBlockSize + 32);

    filterStates.calloc (numChannels);
    srcBuffers.calloc (numChannels);
    destBuffers.calloc (numChannels);
    createLowPass (ratio);

    flushBuffers();
}

void ResamplingFifo::flushBuffers()
{
    buffer.clear();
    bufferPos = 0;
    sampsInBuffer = 0;
    subSampleOffset = 0.0;
    resetFilters();
}

void ResamplingFifo::pushAudioBuffer (const AudioSampleBuffer& src)
{
    inputFifo.write (src);

    const int sampsNeeded = roundToInt (blockSize * ratio) + 3;
    while (inputFifo.getNumReady() >= sampsNeeded)
        process();
}

void ResamplingFifo::popAudioBuffer (AudioSampleBuffer& dest)
{
    outputFifo.read (dest);
}

void ResamplingFifo::process()
{
    if (lastRatio != ratio)
    {
        createLowPass (ratio);
        lastRatio = ratio;
    }

    const int sampsNeeded = roundToInt (blockSize * ratio) + 3;

    int bufferSize = buffer.getNumSamples();

    if (bufferSize < sampsNeeded + 8)
    {
        bufferPos %= bufferSize;
        bufferSize = sampsNeeded + 32;
        buffer.setSize (buffer.getNumChannels(), bufferSize, true, true);
    }

    bufferPos %= bufferSize;

    int endOfBufferPos = bufferPos + sampsInBuffer;
    const int channelsToProcess = numChannels;

    while (sampsNeeded > sampsInBuffer)
    {
        endOfBufferPos %= bufferSize;

        int numToDo = jmin (sampsNeeded - sampsInBuffer, bufferSize - endOfBufferPos);

        inputFifo.read (buffer, endOfBufferPos, numToDo);

        if (ratio > 1.0001)
        {
            // for down-sampling, pre-apply the filter..
            for (int i = channelsToProcess; --i >= 0;)
                applyFilter (buffer.getWritePointer (i, endOfBufferPos), numToDo, filterStates[i]);
        }

        sampsInBuffer += numToDo;
        endOfBufferPos += numToDo;
    }

    for (int channel = 0; channel < channelsToProcess; ++channel)
    {
        destBuffers[channel] = outBuffer.getWritePointer (channel);
        srcBuffers[channel] = buffer.getReadPointer (channel);
    }

    int nextPos = (bufferPos + 1) % bufferSize;

    for (int m = blockSize; --m >= 0;)
    {
        jassert (sampsInBuffer > 0 && nextPos != endOfBufferPos);

        const float alpha = (float) subSampleOffset;

        for (int channel = 0; channel < channelsToProcess; ++channel)
            *destBuffers[channel]++ = srcBuffers[channel][bufferPos] + alpha * (srcBuffers[channel][nextPos] - srcBuffers[channel][bufferPos]);

        subSampleOffset += ratio;

        while (subSampleOffset >= 1.0)
        {
            if (++bufferPos >= bufferSize)
                bufferPos = 0;

            --sampsInBuffer;

            nextPos = (bufferPos + 1) % bufferSize;
            subSampleOffset -= 1.0;
        }
    }

    if (ratio < 0.9999)
    {
        // for up-sampling, apply the filter after transposing..
        for (int i = channelsToProcess; --i >= 0;)
            applyFilter (outBuffer.getWritePointer (i), blockSize, filterStates[i]);
    }
    else if (ratio <= 1.0001 && blockSize > 0)
    {
        // if the filter's not currently being applied, keep it stoked with the last couple of samples to avoid discontinuities
        for (int i = channelsToProcess; --i >= 0;)
        {
            const float* const endOfBuffer = outBuffer.getReadPointer (i, blockSize - 1);
            FilterState& fs = filterStates[i];

            fs.y2 = fs.x2 = *(endOfBuffer - 1);
            fs.y1 = fs.x1 = *endOfBuffer;
        }
    }

    jassert (sampsInBuffer >= 0);

    outputFifo.write (outBuffer);
}

void ResamplingFifo::createLowPass (const double frequencyRatio)
{
    const double proportionalRate = (frequencyRatio > 1.0) ? 0.5 / frequencyRatio : 0.5 * frequencyRatio;

    const double n = 1.0 / std::tan (MathConstants<double>::pi * jmax (0.001, proportionalRate));
    const double nSquared = n * n;
    const double c1 = 1.0 / (1.0 + MathConstants<double>::sqrt2 * n + nSquared);

    setFilterCoefficients (c1,
                           c1 * 2.0f,
                           c1,
                           1.0,
                           c1 * 2.0 * (1.0 - nSquared),
                           c1 * (1.0 - MathConstants<double>::sqrt2 * n + nSquared));
}

void ResamplingFifo::setFilterCoefficients (double c1, double c2, double c3, double c4, double c5, double c6)
{
    const double a = 1.0 / c4;

    c1 *= a;
    c2 *= a;
    c3 *= a;
    c5 *= a;
    c6 *= a;

    coefficients[0] = c1;
    coefficients[1] = c2;
    coefficients[2] = c3;
    coefficients[3] = c4;
    coefficients[4] = c5;
    coefficients[5] = c6;
}

void ResamplingFifo::resetFilters()
{
    if (filterStates != nullptr)
        filterStates.clear ((size_t) numChannels);
}

void ResamplingFifo::applyFilter (float* samples, int num, FilterState& fs)
{
    while (--num >= 0)
    {
        const double in = *samples;

        double out = coefficients[0] * in + coefficients[1] * fs.x1 + coefficients[2] * fs.x2 - coefficients[4] * fs.y1 - coefficients[5] * fs.y2;

#if JUCE_INTEL
        if (! (out < -1.0e-8 || out > 1.0e-8))
            out = 0;
#endif

        fs.x2 = fs.x1;
        fs.x1 = in;
        fs.y2 = fs.y1;
        fs.y1 = out;

        *samples++ = (float) out;
    }
}
