/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

void SampleOscillator::setSampleRateAndBlockSize (double sr, int bs)
{
    resampler.setSize (bs, 2, int (sr));
}

void SampleOscillator::setSample (Sample* s)
{
    sample = s;
    start = 0;
    end = s->getLength() - 1;
    loopStart = 0;
    loopEnd = end;
    looping = false;
}

void SampleOscillator::setRange (int s, int e)
{
    start = s;
    end = e;
}

void SampleOscillator::setLooping (bool l, int s, int e)
{
    looping = l;
    
    if (looping)
    {
        loopStart = s;
        loopEnd = e == -1 ? end : e;
    }
    else
    {
        loopStart = -1;
        loopEnd = -1;
    }
}

void SampleOscillator::setDirection (Direction d)
{
    direction = d;
}

void SampleOscillator::setCrossfadeDuration (double)
{
}

void SampleOscillator::noteOn()
{
    triggered = true;
    finished = false;
    looping = loopStart >= 0;
    
    if (direction == forwards || direction == pingpong)
    {
        pos = start;
        currentDirection = 1;
    }
    else
    {
        pos = end;
        currentDirection = -1;
    }
    
    resampler.reset();
}

void SampleOscillator::noteOff()
{
    triggered = false;
    looping = false;
}

bool SampleOscillator::read (double note, juce::AudioSampleBuffer& buffer, int bufStart, int bufLength)
{
    if (bufLength == -1)
        bufLength = buffer.getNumSamples() - bufStart;

    resampler.setRatio (float (sample->getRootNote() / note));
    
    while (resampler.samplesReady() < bufLength)
        fillFifo (bufLength - resampler.samplesReady());
    
    juce::AudioSampleBuffer slice (buffer.getArrayOfWritePointers(), 2, bufStart, bufLength);
    resampler.popAudioBuffer (slice);
    
    return false;
}

void SampleOscillator::fillFifo (int samplesNeeded)
{
    ScratchBuffer buffer (2, samplesNeeded);
    auto out = buffer.getArrayOfWritePointers();
    
    int readEnd = (looping && triggered) ? loopEnd : end;
    int readSrt = (looping && triggered) ? loopStart : start;
    
    int i = 0;
    for (; i < samplesNeeded; i++)
    {
        sample->readSamplesAt (pos, out[0][start + i], out[1][start + i]);
        
        pos += currentDirection;
        
        if (pos == readSrt || pos == readEnd)
        {
            if (! looping)
            {
                finished = true;
                break;
            }
            else if (direction == forwards)
            {
                pos = readSrt;
            }
            else if (direction == backwards)
            {
                pos = readEnd;
            }
            else if (direction == pingpong)
            {
                currentDirection = -currentDirection;
                pos = (pos == readSrt) ? readEnd : readSrt;
            }
        }
    }
    for (; i < samplesNeeded; i++)
    {
        out[0][i] = 0;
        out[1][i] = 0;
    }
    
}
