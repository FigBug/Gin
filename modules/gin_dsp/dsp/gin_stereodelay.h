/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Stereo delay effect with feedback, cross-feedback, and wet/dry mix.

    StereoDelay provides a professional stereo delay effect with independent
    left/right delay lines, feedback control, and ping-pong capability via
    cross-feedback. Parameters are smoothed to avoid zipper noise.

    Key Features:
    - Independent stereo delay lines
    - Feedback control (repeats)
    - Cross-feedback (ping-pong delay effect)
    - Wet/dry mix control
    - Parameter smoothing for artifact-free automation
    - High-quality Lagrange interpolation

    Parameters:
    - delay: Delay time in seconds
    - mix: Wet/dry mix (0.0 = dry, 1.0 = wet)
    - feedback: Amount of delayed signal fed back (0.0 to 0.99)
    - crossfeed: Amount of cross-channel feedback for ping-pong (0.0 to 0.99)

    Usage:
    @code
    StereoDelay delay(5.0); // Max 5 seconds delay
    delay.setSampleRate(44100.0);
    delay.setParams(0.5f, 0.5f, 0.6f, 0.3f);
    // delay=0.5s, mix=50%, feedback=60%, crossfeed=30%

    delay.processSmoothed(audioBuffer); // Smooth parameter changes
    // or
    delay.process(audioBuffer); // No smoothing (faster)
    @endcode

    @see DelayLine
*/
class StereoDelay
{
public:
    StereoDelay (double maxDelay_ = 5.1)
        : maxDelay (maxDelay_)
    {
        setSampleRate (44100.0);
        mix.reset (44100.0, 0.2);
        delay.reset (44100.0, 0.2);
        feedback.reset (44100.0, 0.2);
        crossfeed.reset (44100.0, 0.2);
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
            auto l = delayLine.readLagrange (0, delay.getTargetValue());
            auto r = delayLine.readLagrange (1, delay.getTargetValue());

            delayLine.write (0, inL[i] + (feedback.getTargetValue() * l) + (crossfeed.getTargetValue() * r));
            delayLine.write (1, inR[i] + (feedback.getTargetValue() * r) + (crossfeed.getTargetValue() * l));
            delayLine.writeFinished();

            outL[i] = l;
            outR[i] = r;
        }

        WetDryMix wetDry (mix.getTargetValue());
        for (int i = 0; i < numSamples; i++)
        {
            outL[i] = (wetDry.wetGain * outL[i]) + (wetDry.dryGain * inL[i]);
            outR[i] = (wetDry.wetGain * outR[i]) + (wetDry.dryGain * inR[i]);
        }
    }

    void processSmoothed (juce::AudioSampleBuffer& buffer)
    {
        if (! isSmoothing())
            return process (buffer);

        int numSamples = buffer.getNumSamples();

        ScratchBuffer scratchBuffer (buffer);

        auto outL = buffer.getWritePointer (0);
        auto outR = buffer.getWritePointer (1);

        auto inL = scratchBuffer.getWritePointer (0);
        auto inR = scratchBuffer.getWritePointer (1);

        for (int i = 0; i < numSamples; i++)
        {
            auto delayVal = delay.getNextValue();

            auto l = delayLine.readLagrange (0, delayVal);
            auto r = delayLine.readLagrange (1, delayVal);

            auto feedbackVal  = feedback.getNextValue();
            auto crossfeedVal = crossfeed.getNextValue();

            delayLine.write (0, inL[i] + (feedbackVal * l) + (crossfeedVal * r));
            delayLine.write (1, inR[i] + (feedbackVal * r) + (crossfeedVal * l));
            delayLine.writeFinished();

            outL[i] = l;
            outR[i] = r;
        }

        for (int i = 0; i < numSamples; i++)
        {
            WetDryMix wetDry (mix.getNextValue());

            outL[i] = (wetDry.wetGain * outL[i]) + (wetDry.dryGain * inL[i]);
            outR[i] = (wetDry.wetGain * outR[i]) + (wetDry.dryGain * inR[i]);
        }
    }

    void setSampleRate (double sr)
    {
        delayLine.setSize (2, maxDelay, sr);
        mix.reset (sr, 0.2);
        delay.reset (sr, 0.2);
        feedback.reset (sr, 0.2);
        crossfeed.reset (sr, 0.2);
    }

    void setParams (float delay_, float mix_, float feedback_, float crossfeed_)
    {
        mix.setTargetValue (mix_);
        delay.setTargetValue (delay_);

        feedback.setTargetValue (std::min (0.99f, feedback_));
        crossfeed.setTargetValue (std::min (0.99f, crossfeed_));
    }

    void reset()
    {
        delayLine.clear();
        mix.setCurrentAndTargetValue (mix.getTargetValue());
        delay.setCurrentAndTargetValue (delay.getTargetValue());
        feedback.setCurrentAndTargetValue (feedback.getTargetValue());
        crossfeed.setCurrentAndTargetValue (crossfeed.getTargetValue());
    }

private:
    bool isSmoothing()
    {
        return mix.isSmoothing()        ||
               delay.isSmoothing()      ||
               feedback.isSmoothing()   ||
               crossfeed.isSmoothing();
    }

    DelayLine delayLine {2};

    double maxDelay = 5.0;
    juce::SmoothedValue<float> mix = 0, delay = 0, feedback = 0, crossfeed = 0;
};
