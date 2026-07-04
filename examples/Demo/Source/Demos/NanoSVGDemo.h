/*
  ==============================================================================

    NanoSVGDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct NanoSVGDemo : public DemoComponent,
                     private juce::ComboBox::Listener
{
    NanoSVGDemo()
    {
        setName ("NanoSVG");

        svgSelector.addItem ("Markup Grid", 1);
        svgSelector.addItem ("Basic Shapes", 2);
        svgSelector.addItem ("Gradients", 3);
        svgSelector.addItem ("Paths", 4);
        svgSelector.addItem ("Strokes", 5);
        svgSelector.addItem ("Opacity/Transforms", 6);

        svgSelector.setSelectedId (1, juce::dontSendNotification);
        svgSelector.addListener (this);
        addAndMakeVisible (svgSelector);

        loadSVG (1);
    }

    void loadSVG (int id)
    {
        switch (id)
        {
            case 1: svgText = juce::String (BinaryData::SVG_example_markup_grid_svg, BinaryData::SVG_example_markup_grid_svgSize); break;
            case 2: svgText = juce::String (BinaryData::svg_test_basic_svg, BinaryData::svg_test_basic_svgSize); break;
            case 3: svgText = juce::String (BinaryData::svg_test_gradients_svg, BinaryData::svg_test_gradients_svgSize); break;
            case 4: svgText = juce::String (BinaryData::svg_test_paths_svg, BinaryData::svg_test_paths_svgSize); break;
            case 5: svgText = juce::String (BinaryData::svg_test_strokes_svg, BinaryData::svg_test_strokes_svgSize); break;
            case 6: svgText = juce::String (BinaryData::svg_test_opacity_svg, BinaryData::svg_test_opacity_svgSize); break;
            default: return;
        }

        repaint();
    }

    void comboBoxChanged (juce::ComboBox*) override
    {
        loadSVG (svgSelector.getSelectedId());
    }

    void resized() override
    {
        svgSelector.setBounds (getLocalBounds().removeFromTop (25).reduced (5, 0));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::white);

        auto area = getLocalBounds().reduced (10);
        area.removeFromTop (30);

        auto colW = area.getWidth() / 3;
        auto col1 = area.removeFromLeft (colW).reduced (5);
        auto col2 = area.removeFromLeft (colW).reduced (5);
        auto col3 = area.reduced (5);

        auto renderH = col1.getHeight() - 40;

        // JUCE Drawable (reference)
        {
            auto t1 = juce::Time::getHighResolutionTicks();
            auto img = renderWithJUCEDrawable (col1.getWidth(), renderH);
            auto t2 = juce::Time::getHighResolutionTicks();
            auto ms = juce::Time::highResolutionTicksToSeconds (t2 - t1) * 1000.0;

            g.drawImageAt (img, col1.getX(), col1.getY());

            g.setColour (juce::Colours::grey);
            g.drawRect (col1.withHeight (renderH));
            g.setColour (juce::Colours::black);
            g.setFont (13.0f);
            g.drawText ("JUCE Drawable: " + juce::String (ms, 2) + " ms",
                         col1.removeFromBottom (40), juce::Justification::centredTop);
        }

        // gin NanoSVG engine
        {
            auto t1 = juce::Time::getHighResolutionTicks();
            auto img = gin::SVG::renderToImage (svgText, col2.getWidth(), renderH, gin::SVG::Engine::nanosvg);
            auto t2 = juce::Time::getHighResolutionTicks();
            auto ms = juce::Time::highResolutionTicksToSeconds (t2 - t1) * 1000.0;

            g.drawImageAt (img, col2.getX(), col2.getY());

            g.setColour (juce::Colours::grey);
            g.drawRect (col2.withHeight (renderH));
            g.setColour (juce::Colours::black);
            g.setFont (13.0f);
            g.drawText ("gin NanoSVG: " + juce::String (ms, 2) + " ms",
                         col2.removeFromBottom (40), juce::Justification::centredTop);
        }

        // gin JUCE engine
        {
            auto t1 = juce::Time::getHighResolutionTicks();
            auto img = gin::SVG::renderToImage (svgText, col3.getWidth(), renderH, gin::SVG::Engine::juce);
            auto t2 = juce::Time::getHighResolutionTicks();
            auto ms = juce::Time::highResolutionTicksToSeconds (t2 - t1) * 1000.0;

            g.drawImageAt (img, col3.getX(), col3.getY());

            g.setColour (juce::Colours::grey);
            g.drawRect (col3.withHeight (renderH));
            g.setColour (juce::Colours::black);
            g.setFont (13.0f);
            g.drawText ("gin JUCE: " + juce::String (ms, 2) + " ms",
                         col3.removeFromBottom (40), juce::Justification::centredTop);
        }
    }

    juce::Image renderWithJUCEDrawable (int w, int h)
    {
        juce::Image img (juce::Image::ARGB, w, h, true);

       #if JUCE_MAJOR_VERSION >= 9
        if (auto drawable = juce::Drawable::createFromSVGString (svgText))
       #else
        auto xml = juce::XmlDocument::parse (svgText);
        if (auto drawable = xml != nullptr ? juce::Drawable::createFromSVG (*xml) : nullptr)
       #endif
        {
            juce::Graphics g (img);
            drawable->drawWithin (g, juce::Rectangle<float> (float (w), float (h)),
                                 juce::RectanglePlacement::centred, 1.0f);
        }

        return img;
    }

    juce::String svgText;
    juce::ComboBox svgSelector;
};
