/*
  ==============================================================================

    WavetableDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct WavetableDemo : public DemoComponent
{
    WavetableDemo()
    {
        setName ("Wavetable");

        juce::MemoryBlock mb (BinaryData::Analog_PWM_Saw_01_wav, BinaryData::Analog_PWM_Saw_01_wavSize);
        juce::AudioSampleBuffer buffer;

        if (int size = gin::getWavetableSize (mb); size > 0)
        {
            auto is = new juce::MemoryInputStream (mb.getData(), mb.getSize(), false);
            auto reader = std::unique_ptr<juce::AudioFormatReader> (juce::WavAudioFormat().createReaderFor (is, true));

            if (reader)
            {
                buffer.setSize (1, int (reader->lengthInSamples));
                reader->read (&buffer, 0, int (reader->lengthInSamples), 0, true, false);

                gin::loadWavetables (bllt, reader->sampleRate, buffer, reader->sampleRate, size);

                osc = std::make_unique<gin::WTOscillator> ();
                osc->setWavetable (&bllt);
            }
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto note = gin::getMidiNoteFromHertz (44.1f);

        juce::AudioSampleBuffer buf (2, 1024);

        auto rc = getLocalBounds().withTrimmedLeft (bllt.size()).withTrimmedBottom (bllt.size());

        int step = 4;
        for (int i = bllt.size(); i >= 0; i -= step)
        {
            gin::WTOscillator::Params params;
            params.position = float (i) / bllt.size();

            buf.clear();

            osc->noteOn (0.0f);
            osc->process (note, params, buf);

            juce::Path p;
            auto d = buf.getReadPointer (0);
            for (auto x = 0; x < 1024; x++)
            {
                auto fx = x / 1024.0f * rc.getWidth() + rc.getX();
                auto fy = -d[x] * rc.getHeight() / 2.0f + rc.getCentreY();

                if (x == 0)
                    p.startNewSubPath (fx, fy);
                else
                    p.lineTo (fx, fy);
            }

            g.setColour (juce::Colours::yellow.withAlpha (0.3f));
            g.strokePath (p, juce::PathStrokeType (1.0f));

            rc.setX (rc.getX() - step);
            rc.setY (rc.getY() + step);
        }
    }

    gin::Wavetable bllt;
    std::unique_ptr<gin::WTOscillator> osc;
};
