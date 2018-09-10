#pragma once

#include "../plugin/parameter.h"
#include "singlelinetexteditor.h"

//==============================================================================
class PluginButton : public TextButton,
                     private Parameter::Listener
{
public:
    PluginButton (Parameter* parameter_)
      : parameter (parameter_)
    {
        setButtonText (parameter->getUserValueText());
        setToggleState (parameter->getUserValue() > 0.0f, dontSendNotification);
        
        parameter->addListener (this);
    }
    
    ~PluginButton()
    {
        parameter->removeListener (this);
    }
    
    void parameterChanged (Parameter*) override
    {
        setToggleState (parameter->getUserValue() > 0.0f, dontSendNotification);
        setButtonText (parameter->getUserValueText());
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
//==============================================================================
class PluginComboBox : public ComboBox,
                       private Parameter::Listener,
                       private ComboBox::Listener
{
public:
    PluginComboBox (Parameter* parameter_)
      : parameter (parameter_)
    {
        for (int i = 0; i <= parameter->getUserRangeEnd() - parameter->getUserRangeStart(); i++)
            addItem (parameter->userValueToText (float (i + parameter->getUserRangeStart())), i + 1);
        
        setSelectedItemIndex (int (parameter->getUserValue() - parameter->getUserRangeStart()), dontSendNotification);
        
        parameter->addListener (this);
        addListener (this);
    }
    
    ~PluginComboBox()
    {
        parameter->removeListener (this);
    }
    
    void parameterChanged (Parameter*) override
    {
        setSelectedItemIndex (int (parameter->getUserValue() - parameter->getUserRangeStart()), dontSendNotification);
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
class PluginSlider : public Slider,
                     private Slider::Listener,
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
    
    ~PluginSlider()
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
        setValue (parameter->getUserValue(), dontSendNotification);
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        if (e.mods.isAltDown())
            parameter->setUserValueNotifingHost (parameter->getUserDefaultValue());
        else if (e.mods.isLeftButtonDown())
            Slider::mouseDown (e);
    }
    
    void mouseUp (const MouseEvent& e) override
    {
        if (e.mods.isLeftButtonDown())
            Slider::mouseUp (e);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        if (e.mods.isLeftButtonDown())
            Slider::mouseDrag (e);
    }
    
private:
    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSlider)
};

//==============================================================================
class Readout : public Label,
                private Parameter::Listener,
                private SingleLineTextEditor::Listener
{
public:
    Readout (Parameter* parameter);
    ~Readout();
    
private:
    void parameterChanged (Parameter* source) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void paint (Graphics& g) override;
    
    void sltextEditorReturnKeyPressed (SingleLineTextEditor&) override;
    void sltextEditorEscapeKeyPressed (SingleLineTextEditor&) override;
    void sltextEditorFocusLost (SingleLineTextEditor&) override;
    
    Parameter* parameter = nullptr;
    SingleLineTextEditor editor;
    
    bool editing = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Readout)
};

//==============================================================================
class ParamComponent : public Component
{
public:
    ParamComponent (Parameter* parameter);
    
    String getUid() { return parameter->getUid(); }
    
private:
    Parameter* parameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamComponent)
};

//==============================================================================
class Knob : public ParamComponent
{
public:
    Knob (Parameter* parameter, bool fromCentre = false);
    
private:
    void resized() override;
    
    Label name;
    Readout value;
    PluginSlider knob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Knob)
};
//==============================================================================
class Switch : public ParamComponent
{
public:
    Switch (Parameter* parameter);
    
private:
    void resized() override;
    
    Label name;
    PluginButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Switch)
};
//==============================================================================
class Select : public ParamComponent
{
public:
    Select (Parameter* parameter);
    
private:
    void resized() override;
    
    Label name;
    PluginComboBox comboBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Select)
};
