#pragma once

//==============================================================================
/** Control for picking param value with label
*/
class Select : public ParamComponent
{
public:
    Select (Parameter* parameter);
    void setShowName (bool s)
    {
        showName = s;
        resized();
    }

protected:
    void resized() override;
    void parentHierarchyChanged() override
    {
        ParamComponent::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        name.setWantsKeyboardFocus (a);
        comboBox.setWantsKeyboardFocus (a);
    }

    juce::Label name;
    PluginComboBox comboBox;

    bool showName = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Select)
};
