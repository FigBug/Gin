/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

struct ResamplingFifo::Impl
{
    SRC::SRC_STATE* state = nullptr;
};

ResamplingFifo::ResamplingFifo (int bs, int nc, int ms)
{
    impl = std::make_unique<Impl>();

    setSize (bs, nc, ms);
}

ResamplingFifo::~ResamplingFifo ()
{
    src_delete (impl->state);
}

void ResamplingFifo::setQuality (int q)
{
    quality = q;

    int error = 0;
    impl->state = SRC::src_new (getQuality (quality), numChannels, &error);
}

int ResamplingFifo::getQuality (int q)
{
    switch (q)
    {
        case 4: return SRC::SRC_SINC_BEST_QUALITY;
        case 3: return SRC::SRC_SINC_MEDIUM_QUALITY;
        case 2: return SRC::SRC_SINC_FASTEST;
        case 1: return SRC::SRC_ZERO_ORDER_HOLD;
        case 0: return SRC::SRC_LINEAR;
        default: return SRC::SRC_SINC_BEST_QUALITY;
    }
}

void ResamplingFifo::setSize (int bs, int nc, int ms)
{
    if (impl->state != nullptr)
        src_delete (impl->state);

    numChannels = nc;
    blockSize = bs;

    int error = 0;
    impl->state = SRC::src_new (getQuality (quality), numChannels, &error);

    outputFifo.setSize (nc, ms);

    ilInputBuffer.setSize (1, blockSize * numChannels);
    ilOutputBuffer.setSize (1, 4 * blockSize * numChannels);
    outputBuffer.setSize (numChannels, 4 * blockSize);
}

void ResamplingFifo::reset()
{
    src_reset (impl->state);
    src_set_ratio (impl->state, ratio);

    outputFifo.reset();
}

void ResamplingFifo::setResamplingRatio (double inputRate, double outputRate)
{
    ratio = float (std::max (0.0, outputRate / inputRate));
}

void ResamplingFifo::setRatio (float r)
{
    ratio = r;
}

float ResamplingFifo::getRatio()
{
    return ratio;
}

void ResamplingFifo::pushAudioBuffer (const juce::AudioSampleBuffer& src)
{
    if (src.getNumSamples() <= blockSize)
    {
        pushAudioBufferInt (src);
    }
    else
    {
        int todo = src.getNumSamples();
        int offset = 0;

        while (todo > 0)
        {
            int thisBlock = std::min (todo, blockSize);

            const juce::AudioSampleBuffer slice ((float**)src.getArrayOfReadPointers(), src.getNumChannels(), offset, thisBlock);

            pushAudioBufferInt (slice);
            todo -= thisBlock;
            offset += thisBlock;
        }
    }
}

void ResamplingFifo::pushAudioBufferInt (const juce::AudioSampleBuffer& src)
{
    jassert (src.getNumSamples() <= blockSize);

    int todo = src.getNumSamples();
    int done = 0;

    {
        using SourceFormat = juce::AudioData::Format<juce::AudioData::Float32, juce::AudioData::NativeEndian>;
        using DestFormat   = juce::AudioData::Format<juce::AudioData::Float32, juce::AudioData::NativeEndian>;

        juce::AudioData::interleaveSamples (juce::AudioData::NonInterleavedSource<SourceFormat> { src.getArrayOfReadPointers(),      numChannels },
                                            juce::AudioData::InterleavedDest<DestFormat>        { ilInputBuffer.getWritePointer (0), numChannels },
                                            src.getNumSamples());
    }


    SRC::SRC_DATA data;
    data.data_in = ilInputBuffer.getReadPointer (0);
    data.data_out = ilOutputBuffer.getWritePointer (0);
    data.output_frames = 4 * blockSize;
    data.src_ratio = ratio;
    data.end_of_input = 0;

    while (todo > 0)
    {
        data.input_frames = todo;
        data.input_frames_used = 0;
        data.output_frames_gen = 0;

        data.data_in = ilInputBuffer.getReadPointer (0) + done * numChannels;

        src_process (impl->state, &data);

        todo -= data.input_frames_used;
        done += data.input_frames_used;

        if (data.output_frames_gen > 0)
        {
            using SourceFormat = juce::AudioData::Format<juce::AudioData::Float32, juce::AudioData::NativeEndian>;
            using DestFormat   = juce::AudioData::Format<juce::AudioData::Float32, juce::AudioData::NativeEndian>;

            juce::AudioData::deinterleaveSamples (juce::AudioData::InterleavedSource<SourceFormat> { ilOutputBuffer.getReadPointer (0),      numChannels },
                                                  juce::AudioData::NonInterleavedDest<DestFormat>  { outputBuffer.getArrayOfWritePointers(), numChannels },
                                                  int (data.output_frames_gen));

            outputFifo.write (outputBuffer, int (data.output_frames_gen));
        }
    }
}

void ResamplingFifo::popAudioBuffer (juce::AudioSampleBuffer& dest)
{
    jassert (outputFifo.getNumReady() >= dest.getNumSamples());
    outputFifo.read (dest);
}

void ResamplingFifo::popAudioBufferAdding (juce::AudioSampleBuffer& dest)
{
    jassert (outputFifo.getNumReady() >= dest.getNumSamples());
    outputFifo.readAdding (dest);
}
