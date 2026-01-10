#pragma once

//==============================================================================
/** Button for toggling a parameter
*/
class PluginButton : public juce::TextButton,
                     private Parameter::ParameterListener
{
public:
    PluginButton (Parameter* parameter_)
      : parameter (parameter_)
    {
        setName (parameter->getShortName());
        setButtonText (parameter->getUserValueText());
        setToggleState (parameter->getUserValue() > 0.0f, juce::dontSendNotification);

        parameter->addListener (this);
    }

    ~PluginButton() override
    {
        parameter->removeListener (this);
    }

    void valueUpdated (Parameter*) override
    {
        setToggleState (parameter->getUserValue() > 0.0f, juce::dontSendNotification);
        setButtonText (parameter->getUserValueText());
        repaint ();
    }

    void clicked() override
    {
        parameter->beginUserAction();
        parameter->setUserValueNotifingHost (parameter->getUserValue() > 0.0f ? 0.0f : 1.0f);
        parameter->endUserAction();
        setButtonText (parameter->getUserValueText());
    }

    void parentHierarchyChanged() override
    {
        juce::TextButton::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        setWantsKeyboardFocus (a);
    }

    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginButton)
};

//==============================================================================
/** Button for toggling a parameter
*/
class SVGPluginButton : public SVGButton,
                        private Parameter::ParameterListener
{
public:
    SVGPluginButton (Parameter* parameter_, const juce::String& svg)
      : SVGButton (parameter_->getShortName(), svg), parameter (parameter_)
    {
        setButtonText (parameter->getUserValueText());
        setToggleState (parameter->getUserValue() > 0.0f, juce::dontSendNotification);

        parameter->addListener (this);
    }

    ~SVGPluginButton() override
    {
        parameter->removeListener (this);
    }

    void valueUpdated (Parameter*) override
    {
        setToggleState (parameter->getUserValue() > 0.0f, juce::dontSendNotification);
        setButtonText (parameter->getUserValueText());
        repaint ();
    }

    void clicked() override
    {
        parameter->beginUserAction();
        parameter->setUserValueNotifingHost (parameter->getUserValue() > 0.0f ? 0.0f : 1.0f);
        parameter->endUserAction();
        setButtonText (parameter->getUserValueText());
    }

    void parentHierarchyChanged() override
    {
        SVGButton::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        setWantsKeyboardFocus (a);
    }

    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVGPluginButton)
};

/** Button for toggling a parameter, drawn as a power button
*/
class PowerButton : public PluginButton
{
public:
    PowerButton (Parameter* p) : PluginButton (p) {}

private:
    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        auto c = findColour (GinLookAndFeel::whiteColourId).withAlpha (getToggleState() ? 0.9f : 0.4f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c);

        auto rc = getLocalBounds().toFloat();
        auto& p = getPowerPath();
        g.fillPath (p, p.getTransformToScaleToFit (rc, true));
    }

    const juce::Path& getPowerPath()
    {
        juce::SharedResourcePointer<Images> images;
        return images->powerPath;
    }

    void parentHierarchyChanged() override
    {
        PluginButton::parentHierarchyChanged();
        auto a = wantsAccessibleKeyboard (*this);
        setWantsKeyboardFocus (a);
    }
};
