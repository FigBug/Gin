/*
  ==============================================================================

    BLLTDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct BLLTDemo : public DemoComponent
{
    BLLTDemo()
    {
        setName ("BLLT");

        juce::AudioSampleBuffer buf (1, 2048);

        auto w = buf.getWritePointer (0);
        for (auto i = 0; i < 2048; i++)
            w[i] = tables.processSquare (0.0f, i / 2048.0f);

        std::unique_ptr<juce::dsp::FFT> fft;
        blltFFT.loadFromBuffer (fft, 44100, buf, 44100, 12);

        blltLowpass.loadFromBuffer (44100, buf, 44100, 44100.0f / 2048.0f, 12);
    }

    void paint (juce::Graphics& g) override
    {

        //
        // From formula
        //
        {
            auto area = getLocalBounds();
            auto note = 0.5f;
            for (int i = 0; i < int (blltFFT.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / blltFFT.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = tables.processSquare (note, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }

                g.setColour (juce::Colours::green);
                g.strokePath (p, juce::PathStrokeType (1.0f));

                note += 12.0f;
            }
        }

        //
        // From wavefile FFT
        //
        {
            auto area = getLocalBounds();
            for (int i = 0; i < int (blltFFT.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / blltFFT.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = blltFFT.get (i, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }
                g.setColour (juce::Colours::yellow);
                g.strokePath (p, juce::PathStrokeType (1.0f));
            }
        }

        //
        // From wavefile Lowpass
        //
        {
            auto area = getLocalBounds();
            for (int i = 0; i < int (blltLowpass.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / blltLowpass.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = blltLowpass.get (i, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }
                g.setColour (juce::Colours::orange);
                g.strokePath (p, juce::PathStrokeType (1.0f));
            }
        }
    }

    gin::BandLimitedLookupTables tables {44100, 12, 2048};
    gin::BandLimitedLookupTable blltFFT;
    gin::BandLimitedLookupTable blltLowpass;
};
