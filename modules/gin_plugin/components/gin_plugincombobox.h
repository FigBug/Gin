#pragma once

//==============================================================================
/** A control for selecting param values from a dropbox
*/
class PluginComboBox : public juce::ComboBox,
                       private Parameter::ParameterListener,
                       private juce::ComboBox::Listener
{
public:
    PluginComboBox (Parameter* parameter_)
      : parameter (parameter_)
    {
        for (int i = 0; i <= parameter->getUserRangeEnd() - parameter->getUserRangeStart(); i++)
            addItem (parameter->userValueToText (float (i + parameter->getUserRangeStart())), i + 1);

        setSelectedItemIndex (int (parameter->getUserValue() - parameter->getUserRangeStart()), juce::dontSendNotification);

        parameter->addListener (this);
        addListener (this);
    }

    ~PluginComboBox() override
    {
        parameter->removeListener (this);
    }

    void valueUpdated (Parameter*) override
    {
        setSelectedItemIndex (int (parameter->getUserValue() - parameter->getUserRangeStart()), juce::dontSendNotification);
    }

    void comboBoxChanged (ComboBox*) override
    {
        parameter->beginUserAction();
        parameter->setUserValueNotifingHost (float (getSelectedItemIndex() + parameter->getUserRangeStart()));
        parameter->endUserAction();
    }

    void parentHierarchyChanged() override
    {
        juce::ComboBox::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        setWantsKeyboardFocus (a);
    }

    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginComboBox)
};
