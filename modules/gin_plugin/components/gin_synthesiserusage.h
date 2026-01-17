#pragma once

//==============================================================================
/** Control for synth usage
*/
class SynthesiserUsage : public juce::Component,
                         private juce::Timer
{
public:
    SynthesiserUsage (gin::Synthesiser& s)
        : synthesiser (s)
    {
        startTimerHz (4);
        addAndMakeVisible (panic);
    }

    void timerCallback() override
    {
        auto v = synthesiser.getNumActiveVoices();
        auto c = synthesiser.getCpuUsage();

        if (v != voices || c != cpu)
        {
            voices = v;
            cpu    = c;

            repaint();
        }
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (findColour (PluginLookAndFeel::grey60ColourId, true));
        g.setFont (11);

        auto rc = getLocalBounds().reduced (2);
        int h = rc.getHeight();

        g.fillPath (voicePath, voicePath.getTransformToScaleToFit (rc.removeFromLeft (h).toFloat(), true));
        g.drawText (juce::String (voices), rc.removeFromLeft (int (h * 1.5)), juce::Justification::centred);

        g.fillPath (cpuPath, cpuPath.getTransformToScaleToFit (rc.removeFromLeft (h).toFloat(), true));
        g.drawText (juce::String (cpu) +"%", rc.removeFromLeft (int (h * 1.5)), juce::Justification::centred);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (2);
        int h = rc.getHeight();

        rc.removeFromLeft (int (h));
        rc.removeFromLeft (int (h * 1.5));
        rc.removeFromLeft (int (h));
        rc.removeFromLeft (int (h * 1.5));

        panic.setBounds (rc.removeFromLeft (int (h * 1.5)));
    }

    gin::Synthesiser& synthesiser;
    int voices = 0, cpu = 0;

    juce::Path voicePath { parseSVGPath (gin::Assets::voice) };
    juce::Path cpuPath { parseSVGPath (gin::Assets::cpu) };

    SVGButton panic { "panic", gin::Assets::panic };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesiserUsage)
};
