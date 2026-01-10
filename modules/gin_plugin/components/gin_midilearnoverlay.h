#pragma once

//==============================================================================
/** Overlay component that draws a pulsing ring around the parameter being learned.
    Add this as a child of your plugin editor and call setBounds to cover the whole editor.
*/
class MidiLearnOverlay : public juce::Component,
                         private juce::Timer
{
public:
    MidiLearnOverlay (MidiLearn& ml);
    ~MidiLearnOverlay() override;

    void paint (juce::Graphics& g) override;
    bool hitTest (int x, int y) override;

private:
    void timerCallback() override;
    juce::Component* findComponentForParameter (gin::Parameter* param);

    MidiLearn& midiLearn;
    juce::Component::SafePointer<juce::Component> targetComponent;
    float phase = 0.0f;
    float successPhase = 0.0f;
    bool wasLearning = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiLearnOverlay)
};
