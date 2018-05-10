/*
  ==============================================================================

    AudioUtil.cpp
    Created: 24 Jan 2018 7:06:35pm
    Author:  Roland Rabien

  ==============================================================================
*/

#include "audioutil.h"

//==============================================================================
void applyGain (AudioSampleBuffer& buffer, LinearSmoothedValue<float>& gain)
{
    if (gain.isSmoothing())
    {
        if (float** w = buffer.getArrayOfWritePointers())
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                float g = gain.getNextValue();
                for (int c = 0; c < buffer.getNumChannels(); c++)
                    w[c][s] *= g;
            }
        }
        
    }
    else
    {
        buffer.applyGain (gain.getTargetValue());
    }
}

void applyGain (AudioSampleBuffer& buffer, int channel, LinearSmoothedValue<float>& gain)
{
    if (gain.isSmoothing())
    {
        if (float* w = buffer.getWritePointer (channel))
            for (int s = 0; s < buffer.getNumSamples(); s++)
                w[s] *= gain.getNextValue();
    }
    else
    {
        buffer.applyGain (channel, 0, buffer.getNumSamples(), gain.getTargetValue());
    }
}
