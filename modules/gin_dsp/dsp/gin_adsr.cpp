/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#include <cmath>

void ADSR::setAttack (float seconds)
{
    if (seconds > 0.0f)
        attackDelta = 1.0f / float (seconds * sampleRate);
    else
        attackDelta = 0.0f;
}

void ADSR::setDecay (float seconds)
{
    if (seconds > 0.0f)
        decayDelta = 1.0f / float (seconds * sampleRate);
    else
        decayDelta = 0.0f;
}

void ADSR::setRelease (float seconds)
{
    if (seconds > 0.0f)
        releaseDelta = 1.0f / float (seconds * sampleRate);
    else
        releaseDelta = 0.0f;
}

void ADSR::setSustainLevel (float level)
{
    jassert (level >= 0.0f && level <= 1.0f);
    sustainLevel = level;
}

void ADSR::process (juce::AudioSampleBuffer& buffer)
{
    process (buffer, 0, buffer.getNumSamples());
}

void ADSR::process (juce::AudioSampleBuffer& buffer, int startSample, int numSamples)
{
    auto d = buffer.getWritePointer (0, startSample);
    for (int i = 0; i < numSamples; i++)
    {
        switch (state)
        {
            case idle:
                break;
            case attack:
                if (attackDelta > 0.0f)
                    output += attackDelta;
                else
                    output = 1.0f;

                if (output >= 1.0f)
                {
                    output = 1.0f;
                    state = decay;
                }
                break;
            case decay:
                if (decayDelta > 0.0f)
                    output -= decayDelta;
                else
                    output = sustainLevel;

                if (output <= sustainLevel)
                {
                    output = sustainLevel;
                    state = sustain;
                }
                break;
            case sustain:
                break;
            case release:
                if (releaseDelta > 0)
                    output -= releaseDelta;
                else
                    output = 0.0f;

                if (output <= 0.0f)
                {
                    output = 0.0f;
                    state = finished;
                }
                break;
            case finished:
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
              if (attackDelta > 0.0f)
                  output += attackDelta;
              else
                  output = 1.0f;

              if (output >= 1.0f)
              {
                  output = 1.0f;
                  state = decay;
              }
              break;
          case decay:
              if (decayDelta > 0.0f)
                  output -= decayDelta;
              else
                  output = sustainLevel;

              if (output <= sustainLevel)
              {
                  output = sustainLevel;
                  state = sustain;
              }
              break;
          case sustain:
              break;
          case release:
              if (releaseDelta > 0)
                  output -= releaseDelta;
              else
                  output = 0.0f;

              if (output <= 0.0f)
              {
                  output = 0.0f;
                  state = finished;
              }
              break;
        case finished:
            break;
    }
    return output;
}

float ADSR::process (int num)
{
    if (state == idle)
        return (attackDelta == 0.0f) ? 1.0f : 0.0f;

    if (state == finished)
        return 0.0f;

    for (int i = num; --i >= 0;)
        process();

    return output;
}

void ADSR::reset()
{
    state = idle;
    output = 0.0f;
}
