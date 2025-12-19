/*
  ==============================================================================

    BlendingDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct BlendingDemo : public DemoComponent,
                      private juce::ComboBox::Listener,
                      private juce::Slider::Listener
{
    BlendingDemo()
    {
        setName ("Blending");

        imgA = juce::ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize);
        imgB = juce::ImageFileFormat::loadFrom (BinaryData::mountain_jpg, BinaryData::mountain_jpgSize);


        modeBox.addItemList (modeNames, 1);
        modeBox.setSelectedItemIndex (0);
        modeBox.addListener (this);
        addAndMakeVisible (modeBox);

        alphaSlider.setRange (0.0, 1.0);
        alphaSlider.setValue (1.0);
        alphaSlider.addListener (this);
        addAndMakeVisible (alphaSlider);
    }

    void sliderValueChanged (juce::Slider*) override { repaint(); }
    void comboBoxChanged (juce::ComboBox*) override  { repaint(); }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        alphaSlider.setBounds (rc.removeFromLeft (getWidth() / 3));

        modeBox.setBounds (5, 5, 150, 20);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();

        auto img = imgB.createCopy();

        gin::BlendMode blendMode = (gin::BlendMode) modeBox.getSelectedItemIndex();
        float alpha = float (alphaSlider.getValue());

        gin::applyBlend (img, imgA, blendMode, alpha, {}, &getDemoThreadPool());

        g.drawImage (img, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image imgA, imgB;

    juce::StringArray modeNames =
    {
        "Normal", "Lighten", "Darken", "Multiply", "Average", "Add", "Subtract", "Difference", "Negation", "Screen", "Exclusion", "Overlay", "Soft Light", "Hard Light",
        "Color Dodge", "Color Burn", "Linear Dodge", "Linear Burn", "Linear Light", "Vivid Light", "Pin Light", "Hard Mix", "Reflect", "Glow", "Phoenix"
    };


    juce::ComboBox modeBox;
    juce::Slider alphaSlider;
};
