/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

struct ResamplingFifo::Impl
{
    SRC_STATE* state = nullptr;
};

ResamplingFifo::ResamplingFifo (int bs, int nc, int ms)
    : numChannels (nc), blockSize (bs), outputFifo (nc, ms)
{
    impl = std::make_unique<Impl>();

    int error = 0;
    impl->state = src_new (SRC_SINC_BEST_QUALITY, numChannels, &error);

    ilInputBuffer.setSize (1, blockSize * numChannels);
    ilOutputBuffer.setSize (1, 4 * blockSize * numChannels);
    outputBuffer.setSize (numChannels, 4 * blockSize);
}

ResamplingFifo::~ResamplingFifo ()
{
    src_delete (impl->state);
}

void ResamplingFifo::setSize (int bs, int nc, int ms)
{
    numChannels = nc;
    blockSize = bs;
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

    juce::AudioDataConverters::interleaveSamples (src.getArrayOfReadPointers(), ilInputBuffer.getWritePointer (0), src.getNumSamples(), numChannels);

    SRC_DATA data;
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
            juce::AudioDataConverters::deinterleaveSamples (ilOutputBuffer.getReadPointer (0), outputBuffer.getArrayOfWritePointers(), int (data.output_frames_gen), numChannels);

            outputFifo.write (outputBuffer, int (data.output_frames_gen));
        }
    }
}

void ResamplingFifo::popAudioBuffer (juce::AudioSampleBuffer& dest)
{
    outputFifo.read (dest);
}
