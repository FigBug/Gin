#pragma once

//==============================================================================
/** A slider for controlling a parameter
*/
class PluginSlider : public juce::Slider,
                     private juce::Slider::Listener,
                     private Parameter::ParameterListener
{
public:
    PluginSlider (Parameter* parameter_, SliderStyle style, TextEntryBoxPosition textBoxPosition)
      : Slider (style, textBoxPosition), parameter (parameter_)
    {
        setName (parameter->getShortName());

        addListener (this);
        setRange (parameter->getUserRangeStart(), parameter->getUserRangeEnd());
        setValue (parameter->getUserValue(), juce::dontSendNotification);

        parameter->addListener (this);
    }

    ~PluginSlider() override
    {
        parameter->removeListener (this);
    }

    void sliderValueChanged (Slider*) override
    {
        parameter->setUserValueNotifingHost (float (getValue()));
        setValue (parameter->getUserValue(), juce::dontSendNotification);
    }

    void sliderDragStarted (Slider*) override
    {
        parameter->beginUserAction();
    }

    void sliderDragEnded (Slider*) override
    {
        parameter->endUserAction();
    }

    void valueUpdated (Parameter*) override
    {
        setValue (parameter->getUserValue(), juce::dontSendNotification);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.mods.isAltDown())
            parameter->setUserValueNotifingHost (parameter->getUserDefaultValue());
        else if (e.mods.isLeftButtonDown())
            Slider::mouseDown (e);
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (e.mods.isLeftButtonDown())
            Slider::mouseUp (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (e.mods.isLeftButtonDown())
            Slider::mouseDrag (e);
    }

    void parentHierarchyChanged() override
    {
        juce::Slider::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        setWantsKeyboardFocus (a);
    }

private:
    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSlider)
};
