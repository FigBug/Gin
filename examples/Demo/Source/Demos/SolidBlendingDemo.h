/*
  ==============================================================================

    SolidBlendingDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct SolidBlendingDemo : public DemoComponent,
                           private juce::ComboBox::Listener,
                           private juce::Slider::Listener,
                           private juce::ChangeListener
{
    SolidBlendingDemo()
    {
        setName ("Solid Blending");

        img = juce::ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize);

        modeBox.addItemList (modeNames, 1);
        modeBox.setSelectedItemIndex (0);
        modeBox.addListener (this);
        addAndMakeVisible (modeBox);

        alphaSlider.setRange (0.0, 1.0);
        alphaSlider.setValue (1.0);
        alphaSlider.addListener (this);
        addAndMakeVisible (alphaSlider);

        selector.setCurrentColour (juce::Colour (0xffe3c916));
        selector.addChangeListener (this);
        addAndMakeVisible (selector);
    }

    void sliderValueChanged (juce::Slider*) override                  { repaint(); }
    void comboBoxChanged (juce::ComboBox*) override                   { repaint(); }
    void changeListenerCallback (juce::ChangeBroadcaster*) override   { repaint(); }

    void resized() override
    {
        auto rc = getLocalBounds();

        auto rcBottom = getLocalBounds().removeFromBottom (20);
        alphaSlider.setBounds (rcBottom.removeFromLeft (getWidth() / 3));

        selector.setBounds (rc.removeFromRight (rc.getWidth() / 3).removeFromTop (rc.getHeight() / 3));

        modeBox.setBounds (5, 5, 150, 20);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();

        auto copy = img.createCopy();

        gin::BlendMode blendMode = (gin::BlendMode) modeBox.getSelectedItemIndex();
        float alpha = float (alphaSlider.getValue());

        auto c = selector.getCurrentColour();
        c = c.withMultipliedAlpha (alpha);

        gin::applyBlend (copy, blendMode, c, &getDemoThreadPool());

        g.drawImage (copy, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image img;

    juce::StringArray modeNames =
    {
        "Normal", "Lighten", "Darken", "Multiply", "Average", "Add", "Subtract", "Difference", "Negation", "Screen", "Exclusion", "Overlay", "Soft Light", "Hard Light",
        "Color Dodge", "Color Burn", "Linear Dodge", "Linear Burn", "Linear Light", "Vivid Light", "Pin Light", "Hard Mix", "Reflect", "Glow", "Phoenix"
    };

    juce::ComboBox modeBox;
    juce::Slider alphaSlider;
    juce::ColourSelector selector;
};
