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

//==============================================================================
/** A control for displaying a parameter
*/
class Readout : public juce::Label,
                private Parameter::ParameterListener
{
public:
    Readout (Parameter* parameter);
    ~Readout() override;

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

//==============================================================================
/** Base class for a control showing a parameter with label
*/
class ParamComponent : public juce::Component,
                       public juce::SettableTooltipClient
{
public:
    using Ptr = ParamComponent*;

    ParamComponent (Parameter* parameter);

    juce::String getUid() { return parameter->getUid(); }

    void setTooltip (const juce::String& newTooltip) override
    {
        juce::SettableTooltipClient::setTooltip (newTooltip);

        for (auto c : getChildren())
            if (auto stc = dynamic_cast<juce::SettableTooltipClient*> (c))
                stc->setTooltip (newTooltip);
    }

protected:
    Parameter* parameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamComponent)
};

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

//==============================================================================
/** Control for synth usage
*/
class SynthesiserUsage : public juce::Component,
                         private juce::Timer
{
public:
    SynthesiserUsage (Synthesiser& s)
        : synthesiser (s)
    {
        startTimerHz (4);
        addAndMakeVisible (panic);
    }

    void timerCallback() override
    {
        auto v = synthesiser.getNumActiveVoices();
        auto c = synthesiser.getCpuUsage();

        if (v != voices || c != cpu)
        {
            voices = v;
            cpu    = c;

            repaint();
        }
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (findColour (PluginLookAndFeel::grey60ColourId, true));
        g.setFont (11);

        auto rc = getLocalBounds().reduced (2);
        int h = rc.getHeight();

        g.fillPath (voicePath, voicePath.getTransformToScaleToFit (rc.removeFromLeft (h).toFloat(), true));
        g.drawText (juce::String (voices), rc.removeFromLeft (int (h * 1.5)), juce::Justification::centred);

        g.fillPath (cpuPath, cpuPath.getTransformToScaleToFit (rc.removeFromLeft (h).toFloat(), true));
        g.drawText (juce::String (cpu) +"%", rc.removeFromLeft (int (h * 1.5)), juce::Justification::centred);
    }
    
    void resized() override
    {
        auto rc = getLocalBounds().reduced (2);
        int h = rc.getHeight();
        
        rc.removeFromLeft (int (h));
        rc.removeFromLeft (int (h * 1.5));
        rc.removeFromLeft (int (h));
        rc.removeFromLeft (int (h * 1.5));
        
        panic.setBounds (rc.removeFromLeft (int (h * 1.5)));
    }

    Synthesiser& synthesiser;
    int voices = 0, cpu = 0;

    juce::Path voicePath { parseSVGPath (gin::Assets::voice) };
    juce::Path cpuPath { parseSVGPath (gin::Assets::cpu) };
    
    SVGButton panic { "panic", gin::Assets::panic };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesiserUsage)
};
