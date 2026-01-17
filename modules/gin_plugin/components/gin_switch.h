#pragma once

//==============================================================================
/** Control for toggling param with label
*/
class Switch : public ParamComponent
{
public:
    Switch (Parameter* parameter);

private:
    void resized() override;
    void parentHierarchyChanged() override
    {
        ParamComponent::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        name.setWantsKeyboardFocus (a);
        button.setWantsKeyboardFocus (a);
    }

    juce::Label name;
    PluginButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Switch)
};
