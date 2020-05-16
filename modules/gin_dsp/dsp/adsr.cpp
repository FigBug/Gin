/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#include <cmath>

void ADSR::setAttack (float seconds)
{
    attackDelta = 1.0f / float (seconds * sampleRate);
}

void ADSR::setDecay (float seconds)
{
    decayDelta = 1.0f / float (seconds * sampleRate);
}

void ADSR::setRelease (float seconds)
{
    releaseDelta = 1.0f / float (seconds * sampleRate);
}

void ADSR::setSustainLevel (float level)
{
    jassert (level >= 0.0f && level <= 1.0f);
    sustainLevel = level;
}

void ADSR::process (AudioSampleBuffer& buffer)
{
    process (buffer, 0, buffer.getNumSamples());
}

void ADSR::process (AudioSampleBuffer& buffer, int startSample, int numSamples)
{
    float* d = buffer.getWritePointer (0, startSample);
    for (int i = 0; i < numSamples; i++)
    {
        switch (state)
        {
            case idle:
                break;
            case attack:
                output += attackDelta;
                if (output >= 1.0)
                {
                    output = 1.0;
                    state = decay;
                }
                break;
            case decay:
                output -= decayDelta;
                if (output <= sustainLevel)
                {
                    output = sustainLevel;
                    state = sustain;
                }
                break;
            case sustain:
                break;
            case release:
                output -= releaseDelta;
                if (output <= 0.0)
                {
                    output = 0.0;
                    state = idle;
                }
                break;
        }
        *d++ = output;
    }
}

float ADSR::process()
{
    switch (state)
    {
        case idle:
            break;
        case attack:
            output += attackDelta;
            if (output >= 1.0)
            {
                output = 1.0;
                state = decay;
            }
            break;
        case decay:
            output -= decayDelta;
            if (output <= sustainLevel)
            {
                output = sustainLevel;
                state = sustain;
            }
            break;
        case sustain:
            break;
        case release:
            output -= releaseDelta;
            if (output <= 0.0)
            {
                output = 0.0;
                state = idle;
            }
            break;
    }
    return output;
}

float ADSR::process (int num)
{
    for (int i = num; --i >= 0;)
        process();
    return output;
}


void ADSR::reset()
{
    state = idle;
    output = 0.0f;
}
