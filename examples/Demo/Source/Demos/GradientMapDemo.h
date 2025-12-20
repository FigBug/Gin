/*
  ==============================================================================

    GradientMapDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct GradientMapDemo : public DemoComponent,
                         private juce::ChangeListener
{
    GradientMapDemo()
    {
        setName ("Gradient Map");

        source = juce::ImageFileFormat::loadFrom (BinaryData::Castle_jpg, BinaryData::Castle_jpgSize);

        selector1.setCurrentColour (juce::Colour (0xffe3c916));
        selector2.setCurrentColour (juce::Colour (0xff0c0d01));

        addAndMakeVisible (selector1);
        addAndMakeVisible (selector2);

        selector1.addChangeListener (this);
        selector2.addChangeListener (this);
    }

    void changeListenerCallback (juce::ChangeBroadcaster*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds();
        rc = rc.withTrimmedBottom (rc.getHeight() / 2);

        selector1.setBounds (rc.removeFromLeft (rc.getWidth() / 2));
        selector2.setBounds (rc.removeFromLeft (rc.getWidth()));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();
        rc = rc.withTrimmedTop (rc.getHeight() / 2);

        auto img = source.createCopy();
        gin::applyGradientMap (img, selector1.getCurrentColour(), selector2.getCurrentColour(), &getDemoThreadPool());

        g.drawImage (img, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source;
    juce::ColourSelector selector1, selector2;
};
