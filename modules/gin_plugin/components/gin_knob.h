#pragma once

//==============================================================================
/** Slider + editable text for showing a param
*/
class Knob : public ParamComponent,
             private Timer,
             private ModMatrix::Listener
{
public:
    Knob (Parameter* parameter, bool fromCentre = false);
    ~Knob() override;

    void setLiveValuesCallback (std::function<Array<float> ()> cb)
    {
        liveValuesCallback = cb;
        modMatrixChanged();
    }

private:
    void resized() override;
    void mouseEnter (const MouseEvent& e) override;
    void timerCallback() override;
    void learnSourceChanged (ModSrcId) override;
    void modMatrixChanged() override;

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;

    void showModMenu();

    Label name;
    Readout value;
    PluginSlider knob;
    bool learning = false;
    float modDepth = 0.0f;

    gin::CoalescedTimer modTimer;
    gin::CoalescedTimer shiftTimer;
    Array<float> modValues;
    std::function<Array<float> ()> liveValuesCallback;
    gin::ModulationDestinationButton modButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Knob)
};
