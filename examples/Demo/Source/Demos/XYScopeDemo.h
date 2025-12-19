/*
  ==============================================================================

    XYScopeDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct XYScopeDemo : public DemoComponent,
                     public juce::Timer
{
    enum class Pattern
    {
        circle,
        figure8,
        pretzel,
        star,
        spiral
    };

    XYScopeDemo()
    {
        setName ("XY Scope");

        addAndMakeVisible (xyScope);
        xyScope.setColour (gin::XYScope::backgroundColourId, juce::Colours::black);
        xyScope.setColour (gin::XYScope::lineColourId, juce::Colours::grey);
        xyScope.setColour (gin::XYScope::traceColourId, juce::Colours::limegreen);

        addAndMakeVisible (circleButton);
        addAndMakeVisible (figure8Button);
        addAndMakeVisible (pretzelButton);
        addAndMakeVisible (starButton);
        addAndMakeVisible (spiralButton);
        addAndMakeVisible (freezeButton);

        circleButton.setRadioGroupId (1);
        figure8Button.setRadioGroupId (1);
        pretzelButton.setRadioGroupId (1);
        starButton.setRadioGroupId (1);
        spiralButton.setRadioGroupId (1);

        circleButton.setToggleState (true, juce::dontSendNotification);
        freezeButton.setClickingTogglesState (true);

        circleButton.onClick = [this] { currentPattern = Pattern::circle; };
        figure8Button.onClick = [this] { currentPattern = Pattern::figure8; };
        pretzelButton.onClick = [this] { currentPattern = Pattern::pretzel; };
        starButton.onClick = [this] { currentPattern = Pattern::star; };
        spiralButton.onClick = [this] { currentPattern = Pattern::spiral; };

        xyFifo.setSize (2, 44100);

        startTimerHz (60);
    }

    ~XYScopeDemo() override
    {
        stopTimer();
    }

    void prepareToPlay (double sr, int samplesPerBlock) override
    {
        juce::ignoreUnused (samplesPerBlock);
        sampleRate = sr;
        xyFifo.setSize (2, int (sr));
    }

    void processBlock (juce::AudioBuffer<float>& buffer) override
    {
        const int numSamples = buffer.getNumSamples();
        const float baseFreq = 220.0f; // A3 - audible frequency

        // Amplitude modulation - pulse up and down
        const float ampModFreq = 2.0f; // 2 Hz amplitude modulation

        auto* leftChannel = buffer.getWritePointer (0);
        auto* rightChannel = buffer.getWritePointer (1);

        for (int i = 0; i < numSamples; ++i)
        {
            const float t = static_cast<float> (phase);
            const float ampMod = 0.5f + 0.5f * std::sin (static_cast<float> (ampPhase));

            float left = 0.0f;
            float right = 0.0f;

            switch (currentPattern)
            {
                case Pattern::circle:
                    // 90 degree phase difference creates a circle
                    left = std::sin (t) * ampMod;
                    right = std::cos (t) * ampMod;
                    break;

                case Pattern::figure8:
                    // Lissajous with 2:1 frequency ratio
                    left = std::sin (t) * ampMod;
                    right = std::sin (2.0f * t) * ampMod;
                    break;

                case Pattern::pretzel:
                    // Lissajous with 3:2 frequency ratio
                    left = std::sin (2.0f * t) * ampMod;
                    right = std::sin (3.0f * t) * ampMod;
                    break;

                case Pattern::star:
                    // 5-pointed star using 5:4 ratio with phase offset
                    left = std::sin (4.0f * t) * ampMod;
                    right = std::sin (5.0f * t + 0.5f) * ampMod;
                    break;

                case Pattern::spiral:
                    // Spiral effect using amplitude that varies with angle
                    {
                        const float spiralAmp = 0.3f + 0.7f * (0.5f + 0.5f * std::sin (t * 0.1f));
                        left = std::sin (t) * spiralAmp * ampMod;
                        right = std::cos (t) * spiralAmp * ampMod;
                    }
                    break;
            }

            leftChannel[i] = left;
            rightChannel[i] = right;

            // Advance phase (unless frozen)
            if (freezeButton.getToggleState())
                continue;
            phase += 2.0 * juce::MathConstants<double>::pi * baseFreq / sampleRate;
            if (phase > 2.0 * juce::MathConstants<double>::pi * 1000.0)
                phase -= 2.0 * juce::MathConstants<double>::pi * 1000.0;

            ampPhase += 2.0 * juce::MathConstants<double>::pi * ampModFreq / sampleRate;
            if (ampPhase > 2.0 * juce::MathConstants<double>::pi)
                ampPhase -= 2.0 * juce::MathConstants<double>::pi;
        }

        // Feed the XY scope (don't feed when frozen to keep the display static)
        if (! freezeButton.getToggleState() && xyFifo.getFreeSpace() >= numSamples)
            xyFifo.write (buffer);

        // Reduce output volume so it's not annoying
        buffer.applyGain (0.25f);
    }

    void timerCallback() override
    {
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black.brighter (0.1f));
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        auto buttonRow = rc.removeFromTop (28);
        const int buttonWidth = (buttonRow.getWidth() - 48) / 6;

        circleButton.setBounds (buttonRow.removeFromLeft (buttonWidth));
        buttonRow.removeFromLeft (8);
        figure8Button.setBounds (buttonRow.removeFromLeft (buttonWidth));
        buttonRow.removeFromLeft (8);
        pretzelButton.setBounds (buttonRow.removeFromLeft (buttonWidth));
        buttonRow.removeFromLeft (8);
        starButton.setBounds (buttonRow.removeFromLeft (buttonWidth));
        buttonRow.removeFromLeft (8);
        spiralButton.setBounds (buttonRow.removeFromLeft (buttonWidth));
        buttonRow.removeFromLeft (8);
        freezeButton.setBounds (buttonRow.removeFromLeft (buttonWidth));

        rc.removeFromTop (8);

        // Make the XY scope square and centered
        int scopeSize = std::min (rc.getWidth(), rc.getHeight());
        auto scopeBounds = rc.withSizeKeepingCentre (scopeSize, scopeSize);
        xyScope.setBounds (scopeBounds);
    }

    gin::AudioFifo xyFifo { 2, 44100 };
    gin::XYScope xyScope { xyFifo };

    juce::TextButton circleButton { "Circle" };
    juce::TextButton figure8Button { "Figure 8" };
    juce::TextButton pretzelButton { "Pretzel" };
    juce::TextButton starButton { "Star" };
    juce::TextButton spiralButton { "Spiral" };
    juce::TextButton freezeButton { "Freeze" };

    Pattern currentPattern = Pattern::circle;
    double sampleRate = 44100.0;
    double phase = 0.0;
    double ampPhase = 0.0;
};
