#pragma once

//==============================================================================
/** Vertical fader with label for controlling a parameter.
    Uses FaderSlider internally for custom vertical fader appearance.
*/
class Fader : public ParamComponent,
              private juce::Timer
{
public:
    Fader (Parameter* parameter);

    void setDisplayName (const juce::String& name);

    FaderSlider& getSlider() { return fader; }
    Readout& getReadout() { return value; }

    void resized() override;
    void parentHierarchyChanged() override;

private:
    void mouseEnter (const juce::MouseEvent& e) override;
    void timerCallback() override;

    juce::Label name;
    Readout value;
    FaderSlider fader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fader)
};
