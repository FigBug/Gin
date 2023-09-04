#pragma once

//==============================================================================
/** Slider + editable text for showing a param
*/
class Knob : public ParamComponent,
             public juce::DragAndDropTarget,
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
    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseEnter (const juce::MouseEvent& e) override;
    void timerCallback() override;
    void learnSourceChanged (ModSrcId) override;
    void modMatrixChanged() override;
    void parentHierarchyChanged() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

    bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) override;
    void itemDragEnter (const SourceDetails& dragSourceDetails) override;
    void itemDragExit (const SourceDetails& dragSourceDetails) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;

    void showModMenu();

    juce::Label name;
    Readout value;
    PluginSlider knob;
    bool learning = false;
    float modDepth = 0.0f;
    bool dragOver = false;

    gin::CoalescedTimer modTimer;
    gin::CoalescedTimer shiftTimer;
    juce::Array<float> modValues;
    std::function<juce::Array<float> ()> liveValuesCallback;
    gin::ModulationDepthSlider modDepthSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Knob)
};
