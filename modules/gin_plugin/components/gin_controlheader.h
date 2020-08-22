#pragma once

/** Just a simple header with a title
*/
class ControlHeader : public juce::Component
{
public:
    ControlHeader (const juce::String& name_ = {})
        : name (name_)
    {
    }

private:
    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::white.withAlpha (0.2f));
        g.fillAll ();

        g.setColour (juce::Colours::white);
        g.drawText (name, getLocalBounds(), juce::Justification::centred);
    }

    juce::String name;
};
