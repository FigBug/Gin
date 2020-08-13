/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/** Stereo delay with feedback and cross feedback
*/
class StereoDelay
{
public:
    StereoDelay (double maxDelay_ = 5.1)
        : maxDelay (maxDelay_)
    {
        setSampleRate (44100.0);
    }
    
    void process (juce::AudioSampleBuffer& buffer)
    {
        int numSamples = buffer.getNumSamples();
        
        ScratchBuffer scratchBuffer (buffer);

        auto outL = buffer.getWritePointer (0);
        auto outR = buffer.getWritePointer (1);
        
        auto inL = scratchBuffer.getWritePointer (0);
        auto inR = scratchBuffer.getWritePointer (1);

        for (int i = 0; i < numSamples; i++)
        {
            auto l = delayLine.readLinear (0, delay);
            auto r = delayLine.readLinear (1, delay);

            delayLine.write (0, inL[i] + (feedback * l) + (crossfeed * r));
            delayLine.write (1, inR[i] + (feedback * r) + (crossfeed * l));
            delayLine.writeFinished();

            outL[i] = l;
            outR[i] = r;
        }

        WetDryMix wetDry (mix);
        for (int i = 0; i < numSamples; i++)
        {
            outL[i] = (wetDry.wetGain * outL[i]) + (wetDry.dryGain * inL[i]);
            outR[i] = (wetDry.wetGain * outR[i]) + (wetDry.dryGain * inR[i]);
        }
    }

    void setSampleRate (double sr)
    {
        delayLine.setSize (2, maxDelay, sr);
    }

    void setParams (float delay_, float mix_, float feedback_, float crossfeed_)
    {
        mix   = mix_;
        delay = delay_;
        
        feedback  = std::min (0.99f, feedback_);
        crossfeed = std::min (0.99f, crossfeed_);
    }

    void reset()
    {
        delayLine.clear();
    }

private:
    DelayLine delayLine {2};

    double maxDelay = 5.0;
    float mix = 0, delay = 0, feedback = 0, crossfeed = 0;
};
