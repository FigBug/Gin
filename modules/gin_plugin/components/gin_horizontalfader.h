#pragma once

//==============================================================================
/** Horizontal slider wth text for controlling a param
*/
class HorizontalFader : public ParamComponent
{
public:
    HorizontalFader (Parameter* parameter, bool fromCentre = false);

    void resized() override;
    void parentHierarchyChanged() override
    {
        ParamComponent::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        name.setWantsKeyboardFocus (a);
        value.setWantsKeyboardFocus (a);
        fader.setWantsKeyboardFocus (a);
    }

protected:
    juce::Label name;
    Readout value;
    PluginSlider fader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalFader)
};
