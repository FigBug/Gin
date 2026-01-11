#pragma once

//==============================================================================
/**
    Complete parameter control combining rotary slider, value readout, and modulation.

    Knob provides a full-featured parameter UI component that combines a rotary slider
    (PluginSlider), a text value display (Readout), and visual modulation feedback.
    It supports drag-and-drop modulation assignment, modulation depth adjustment, and
    real-time display of modulation values.

    Key Features:
    - Rotary slider control with mouse interaction
    - Editable text value readout
    - Visual modulation depth indicator
    - Drag-and-drop modulation source assignment
    - Live modulation value display
    - Modulation learning mode
    - Optional bipolar (from-centre) display
    - Customizable display name
    - Integration with ModMatrix

    Interaction:
    - Drag slider to adjust parameter value
    - Click readout to type exact value
    - Drag-and-drop mod source to assign modulation
    - Right-click for modulation menu
    - Shift-drag to adjust modulation depth

    The knob automatically connects to the ModMatrix to display active modulations
    and allows users to assign, adjust, and remove modulation sources visually.

    Usage:
    @code
    auto* cutoffParam = processor.getParameter("cutoff");
    auto* knob = new Knob(cutoffParam);
    knob->setDisplayName("Cutoff");

    // Enable live modulation display
    knob->setLiveValuesCallback([this]() {
        return getCurrentModulationValues();
    });

    addAndMakeVisible(knob);
    @endcode

    @see Parameter, ModMatrix, PluginSlider, Readout
*/
class Knob : public ParamComponent,
             public juce::DragAndDropTarget,
             private juce::Timer,
             private ModMatrix::Listener
{
public:
    Knob (Parameter* parameter, bool fromCentre = false);
    ~Knob() override;

    void setDisplayName (const juce::String&);

    void setLiveValuesCallback (std::function<juce::Array<float> ()> cb)
    {
        liveValuesCallback = cb;
        modMatrixChanged();
    }

    PluginSlider& getSlider()   { return knob; }
    Readout& getReadout()       { return value; }

    void paint (juce::Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;

    void mouseEnter (const juce::MouseEvent& e) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

    bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) override;
    void itemDragEnter (const SourceDetails& dragSourceDetails) override;
    void itemDragExit (const SourceDetails& dragSourceDetails) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;

    bool allowClicksOn (const juce::Component& c) override { return &c == &modDepthSlider; }

protected:
    void timerCallback() override;

    void learnSourceChanged (ModSrcId) override;
    void modMatrixChanged() override;

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
