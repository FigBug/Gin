#pragma once

//==============================================================================
/** Button for toggling a parameter
*/
class PluginButton : public juce::TextButton,
                     private Parameter::Listener
{
public:
    PluginButton (Parameter* parameter_)
      : parameter (parameter_)
    {
        setButtonText (parameter->getUserValueText());
        setToggleState (parameter->getUserValue() > 0.0f, juce::dontSendNotification);

        parameter->addListener (this);
    }

    ~PluginButton() override
    {
        parameter->removeListener (this);
    }

    void parameterChanged (Parameter*) override
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

    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginButton)
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

};

//==============================================================================
/** A control for selecting param values from a dropbox
*/
class PluginComboBox : public juce::ComboBox,
                       private Parameter::Listener,
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

    void parameterChanged (Parameter*) override
    {
        setSelectedItemIndex (int (parameter->getUserValue() - parameter->getUserRangeStart()), juce::dontSendNotification);
    }

    void comboBoxChanged (ComboBox*) override
    {
        parameter->beginUserAction();
        parameter->setUserValueNotifingHost (float (getSelectedItemIndex() + parameter->getUserRangeStart()));
        parameter->endUserAction();
    }

    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginComboBox)
};

//==============================================================================
/** A slider for controlling a parameter
*/
class PluginSlider : public juce::Slider,
                     private juce::Slider::Listener,
                     private Parameter::Listener
{
public:
    PluginSlider (Parameter* parameter_, SliderStyle style, TextEntryBoxPosition textBoxPosition)
      : Slider (style, textBoxPosition), parameter (parameter_)
    {
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
        setValue (parameter->getUserValue());
    }

    void sliderDragStarted (Slider*) override
    {
        parameter->beginUserAction();
    }

    void sliderDragEnded (Slider*) override
    {
        parameter->endUserAction();
    }

    void parameterChanged (Parameter*) override
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

private:
    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSlider)
};

//==============================================================================
/** A control for displaying a parameter
*/
class Readout : public juce::Label,
                private Parameter::Listener,
                private SingleLineTextEditor::Listener
{
public:
    Readout (Parameter* parameter);
    ~Readout() override;

    bool isEditing()    { return editing; }

private:
    void parameterChanged (Parameter* source) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void paint (juce::Graphics& g) override;

    void sltextEditorReturnKeyPressed (SingleLineTextEditor&) override;
    void sltextEditorEscapeKeyPressed (SingleLineTextEditor&) override;
    void sltextEditorFocusLost (SingleLineTextEditor&) override;

    Parameter* parameter = nullptr;
    SingleLineTextEditor editor;

    bool editing = false;

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

private:
    void resized() override;

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

private:
    void resized() override;

    juce::Label name;
    PluginComboBox comboBox;

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
        g.setColour (juce::Colours::white);
        g.setFont (12);

        auto rc = getLocalBounds().reduced (2);
        int h = rc.getHeight();

        g.fillPath (voicePath, voicePath.getTransformToScaleToFit (rc.removeFromLeft (h).toFloat(), true));
        g.drawText (juce::String (voices), rc.removeFromLeft (int (h * 1.5)), juce::Justification::centred);

        g.fillPath (cpuPath, cpuPath.getTransformToScaleToFit (rc.removeFromLeft (h).toFloat(), true));
        g.drawText (juce::String (cpu), rc.removeFromLeft (int (h * 1.5)), juce::Justification::centred);
    }

    Synthesiser& synthesiser;
    int voices = 0, cpu = 0;

    juce::Path voicePath { parseSVGPath ("M12.41 148.02l232.94 105.67c6.8 3.09 14.49 3.09 21.29 0l232.94-105.67c16.55-7.51 16.55-32.52 0-40.03L266.65 2.31a25.607 25.607 0 0 0-21.29 0L12.41 107.98c-16.55 7.51-16.55 32.53 0 40.04zm487.18 88.28l-58.09-26.33-161.64 73.27c-7.56 3.43-15.59 5.17-23.86 5.17s-16.29-1.74-23.86-5.17L70.51 209.97l-58.1 26.33c-16.55 7.5-16.55 32.5 0 40l232.94 105.59c6.8 3.08 14.49 3.08 21.29 0L499.59 276.3c16.55-7.5 16.55-32.5 0-40zm0 127.8l-57.87-26.23-161.86 73.37c-7.56 3.43-15.59 5.17-23.86 5.17s-16.29-1.74-23.86-5.17L70.29 337.87 12.41 364.1c-16.55 7.5-16.55 32.5 0 40l232.94 105.59c6.8 3.08 14.49 3.08 21.29 0L499.59 404.1c16.55-7.5 16.55-32.5 0-40z") };
    juce::Path cpuPath { parseSVGPath ("M416 48v416c0 26.51-21.49 48-48 48H144c-26.51 0-48-21.49-48-48V48c0-26.51 21.49-48 48-48h224c26.51 0 48 21.49 48 48zm96 58v12a6 6 0 0 1-6 6h-18v6a6 6 0 0 1-6 6h-42V88h42a6 6 0 0 1 6 6v6h18a6 6 0 0 1 6 6zm0 96v12a6 6 0 0 1-6 6h-18v6a6 6 0 0 1-6 6h-42v-48h42a6 6 0 0 1 6 6v6h18a6 6 0 0 1 6 6zm0 96v12a6 6 0 0 1-6 6h-18v6a6 6 0 0 1-6 6h-42v-48h42a6 6 0 0 1 6 6v6h18a6 6 0 0 1 6 6zm0 96v12a6 6 0 0 1-6 6h-18v6a6 6 0 0 1-6 6h-42v-48h42a6 6 0 0 1 6 6v6h18a6 6 0 0 1 6 6zM30 376h42v48H30a6 6 0 0 1-6-6v-6H6a6 6 0 0 1-6-6v-12a6 6 0 0 1 6-6h18v-6a6 6 0 0 1 6-6zm0-96h42v48H30a6 6 0 0 1-6-6v-6H6a6 6 0 0 1-6-6v-12a6 6 0 0 1 6-6h18v-6a6 6 0 0 1 6-6zm0-96h42v48H30a6 6 0 0 1-6-6v-6H6a6 6 0 0 1-6-6v-12a6 6 0 0 1 6-6h18v-6a6 6 0 0 1 6-6zm0-96h42v48H30a6 6 0 0 1-6-6v-6H6a6 6 0 0 1-6-6v-12a6 6 0 0 1 6-6h18v-6a6 6 0 0 1 6-6z") };
};
