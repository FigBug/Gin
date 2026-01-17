#pragma once

//==============================================================================
/** A control for displaying a parameter
*/
class Readout : public juce::Label,
                private Parameter::ParameterListener
{
public:
    Readout (Parameter* parameter);
    ~Readout() override;

    std::function<bool (const juce::String&)> applyValue = [] (const juce::String&) { return true; };

private:
    void valueUpdated (Parameter* source) override;
    void paint (juce::Graphics& g) override;
    void textWasEdited() override;

    void parentHierarchyChanged() override
    {
        juce::Label::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        setWantsKeyboardFocus (a);
    }

    juce::TextEditor* createEditorComponent() override;

    Parameter* parameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Readout)
};
