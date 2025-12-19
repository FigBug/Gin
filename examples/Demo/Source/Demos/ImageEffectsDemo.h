/*
  ==============================================================================

    ImageEffectsDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct ImageEffectsDemo : public DemoComponent,
                          private juce::Slider::Listener
{
    ImageEffectsDemo()
    {
        setName ("Image Effects");
        addAndMakeVisible (effects);
        effects.addItemList ({"None", "Vignette", "Sepia", "Greyscale", "Soften",
                              "Sharpen", "Gamma", "Invert", "Contrast",
                              "Brightness/Contrast", "Hue/Sat/Light", "Stack Blur"}, 1);
        effects.setSelectedItemIndex (0);
        effects.onChange = [this]
        {
            repaint();
            updateVisibility();
        };

        source = juce::ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        source = source.convertedToFormat (juce::Image::ARGB);

        addAndMakeVisible (vignetteAmount);
        addAndMakeVisible (vignetteRadius);
        addAndMakeVisible (vignetteFalloff);
        addAndMakeVisible (gamma);
        addAndMakeVisible (contrast);
        addAndMakeVisible (brightness);
        addAndMakeVisible (hue);
        addAndMakeVisible (saturation);
        addAndMakeVisible (lightness);
        addAndMakeVisible (radius);

        for (int i = 0; i < getNumChildComponents(); i++)
        {
            if (auto* s = dynamic_cast<juce::Slider*> (getChildComponent (i)))
            {
                s->addListener (this);
                s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            }
        }

        vignetteAmount.setRange (0.01, 0.99);
        vignetteRadius.setRange (0.01, 0.99);
        vignetteFalloff.setRange (0.01, 0.99);
        gamma.setRange (0, 4);
        contrast.setRange (-100, 100);
        brightness.setRange (-100, 100);
        hue.setRange (-180, 180);
        saturation.setRange (0, 200);
        lightness.setRange (-100, 100);
        radius.setRange (2, 254);

        vignetteAmount.setValue (0.5);
        vignetteRadius.setValue (0.5);
        vignetteFalloff.setValue (0.5);
        gamma.setValue (1);
        saturation.setValue (100);

        updateVisibility();
    }

    void updateVisibility()
    {
        int idx = effects.getSelectedItemIndex();
        vignetteAmount.setVisible (idx == 1);
        vignetteRadius.setVisible (idx == 1);
        vignetteFalloff.setVisible (idx == 1);
        gamma.setVisible (idx == 6);
        contrast.setVisible (idx == 8 || idx == 9);
        brightness.setVisible (idx == 9);
        hue.setVisible (idx == 10);
        saturation.setVisible (idx == 10);
        lightness.setVisible (idx == 10);
        radius.setVisible (idx == 11);

        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        for (int i = 0; i < getNumChildComponents(); i++)
            if (auto* s = dynamic_cast<juce::Slider*> (getChildComponent (i)))
                if (s->isVisible())
                    s->setBounds (rc.removeFromLeft (w));
    }

    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        effects.setBounds (5, 5, 150, 20);
    }

    void paint (juce::Graphics& g) override
    {
        auto img = source.createCopy();

        switch (effects.getSelectedItemIndex())
        {
            case 1: gin::applyVignette (img, (float) vignetteAmount.getValue(), (float) vignetteRadius.getValue(), (float) vignetteFalloff.getValue(), &getDemoThreadPool()); break;
            case 2: gin::applySepia (img, &getDemoThreadPool()); break;
            case 3: gin::applyGreyScale (img, &getDemoThreadPool()); break;
            case 4: gin::applySoften (img, &getDemoThreadPool()); break;
            case 5: gin::applySharpen (img, &getDemoThreadPool()); break;
            case 6: gin::applyGamma (img, (float) gamma.getValue(), &getDemoThreadPool()); break;
            case 7: gin::applyInvert (img, &getDemoThreadPool()); break;
            case 8: gin::applyContrast (img, (float) contrast.getValue(), &getDemoThreadPool()); break;
            case 9: gin::applyBrightnessContrast (img, (float) brightness.getValue(), (float) contrast.getValue(), &getDemoThreadPool()); break;
            case 10: gin::applyHueSaturationLightness (img, (float) hue.getValue(), (float) saturation.getValue(), (float) lightness.getValue(), &getDemoThreadPool()); break;
            case 11: gin::applyStackBlur (img, (int) radius.getValue()); break;
        }

        g.fillAll (juce::Colours::black);
        g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source;
    juce::ComboBox effects;

    juce::Slider vignetteAmount, vignetteRadius, vignetteFalloff, gamma, contrast, brightness, hue, saturation, lightness, radius;
};
