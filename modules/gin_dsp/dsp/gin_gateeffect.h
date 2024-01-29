/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/** Gate effect
*/
class GateEffect
{
public:
    GateEffect ()
    {
        adsrL.setDecay (0.0f);
        adsrR.setDecay (0.0f);
        adsrL.setSustainLevel (1.0f);
        adsrR.setSustainLevel (1.0f);
    }

    void setSampleRate (double sr)
    {
        sampleRate = sr;
        adsrL.setSampleRate (sampleRate);
        adsrL.setSampleRate (sampleRate);
    }

    void reset()
    {
        phase = 0.0f;
        triggered = false;
        lastLOn = false;
        lastROn = false;

        adsrL.reset();
        adsrR.reset();
    }

    void process (juce::AudioSampleBuffer& buffer, int noteOnSample, int noteOffSample)
    {
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);

        int numSamples = buffer.getNumSamples();
        int steps = stepsL.size();

        float step = 0.0f;
        if (frequency > 0.0001f)
            step = float (frequency / sampleRate);

        for (int i = 0; i < numSamples; i++)
        {
            if (i == noteOnSample)
            {
                phase = 0.0;
                triggered = true;
            }
            if (i == noteOffSample) triggered = false;

            phase += step;
            while (phase >= float ( steps ))
                phase -= float ( steps );

            auto lOn = triggered && stepsL.getUnchecked (int (phase));
            auto rOn = triggered && stepsR.getUnchecked (int (phase));

            if (lOn != lastLOn)
            {
                lastLOn = lOn;
                if (lOn) adsrL.noteOn(); else adsrL.noteOff();
            }

            if (rOn != lastROn)
            {
                lastROn = rOn;
                if (rOn) adsrR.noteOn(); else adsrR.noteOff();
            }

            *l++ = *l * (1.0f - mix) + *l * adsrL.process() * mix;
            *r++ = *r * (1.0f - mix) + *r * adsrR.process() * mix;
        }
    }

    void setLength (int n)
    {
        stepsL.resize (n);
        stepsR.resize (n);
    }

    void setFrequency (float f)
    {
        frequency = f;
    }

    void setStep (int step, bool l, bool r)
    {
        stepsL.getReference (step) = l;
        stepsR.getReference (step) = r;
    }

    void setAttack (float seconds)
    {
        adsrL.setAttack (seconds);
        adsrR.setAttack (seconds);
    }

    void setRelease (float seconds)
    {
        adsrL.setRelease (seconds);
        adsrR.setRelease (seconds);
    }

    void setMix (float m)
    {
        mix = m;
    }

protected:
    double sampleRate = 44100.0;
    AnalogADSR adsrL, adsrR;
    juce::Array<bool> stepsL, stepsR;
    float frequency = 0.0f, phase = 0.0f, mix = 1.0f;
    bool triggered = false;
    bool lastLOn = false, lastROn = false;
};
