#pragma once

//==============================================================================
/** Slider + editable text for showing a param
*/
class Knob : public ParamComponent,
             private juce::Timer,
             private ModMatrix::Listener
{
public:
    Knob (Parameter* parameter, bool fromCentre = false);
    ~Knob() override;

    void setLiveValuesCallback (std::function<juce::Array<float> ()> cb)
    {
        liveValuesCallback = cb;
        modMatrixChanged();
    }

    PluginSlider& getSlider()   { return knob; }

private:
    void resized() override;
    void mouseEnter (const juce::MouseEvent& e) override;
    void timerCallback() override;
    void learnSourceChanged (ModSrcId) override;
    void modMatrixChanged() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

    void showModMenu();

    juce::Label name;
    Readout value;
    PluginSlider knob;
    bool learning = false;
    float modDepth = 0.0f;

    gin::CoalescedTimer modTimer;
    gin::CoalescedTimer shiftTimer;
    juce::Array<float> modValues;
    std::function<juce::Array<float> ()> liveValuesCallback;
    gin::ModulationDestinationButton modButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Knob)
};
