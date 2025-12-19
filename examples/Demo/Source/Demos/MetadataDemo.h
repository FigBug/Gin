/*
  ==============================================================================

    MetadataDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct MetadataDemo : public DemoComponent
{
    MetadataDemo()
    {
        setName ("Metadata");

        juce::MemoryBlock mb (BinaryData::IMG_1883_JPG, BinaryData::IMG_1883_JPGSize);
        juce::MemoryInputStream is (mb, true);

        addAndMakeVisible (panel);

        juce::OwnedArray<gin::ImageMetadata> metadata;
        if (gin::ImageMetadata::getFromImage (is, metadata))
        {
            for (auto* m : metadata)
            {
                juce::Array<juce::PropertyComponent*> comps;
                juce::StringPairArray values = m->getAllMetadata();

                for (auto key : values.getAllKeys())
                {
                    auto v = juce::Value (values [key]);
                    auto tpc = new juce::TextPropertyComponent (v, key, 1000, false, false);
                    comps.add (tpc);
                }

                panel.addSection (m->getType(), comps);
            }
        }
    }

    void resized()
    {
        panel.setBounds (getLocalBounds());
    }

    juce::PropertyPanel panel;
};
