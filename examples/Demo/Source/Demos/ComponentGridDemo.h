/*
  ==============================================================================

    ComponentGridDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct ComponentGridDemo : public DemoComponent
{
    ComponentGridDemo()
        : horizontalGrid ("HorizontalGrid", gin::ComponentGrid::horizontal),
          verticalGrid ("VerticalGrid", gin::ComponentGrid::vertical)
    {
        setName ("Component Grid");

        // Setup horizontal grid
        horizontalGrid.setGap (8);
        addAndMakeVisible (horizontalGrid);

        // Add colorful buttons to horizontal grid
        for (int i = 0; i < 8; i++)
        {
            auto* btn = new juce::TextButton (juce::String (i + 1));
            btn->setSize (60, 40);
            btn->setColour (juce::TextButton::buttonColourId, juce::Colour::fromHSV (i / 8.0f, 0.7f, 0.8f, 1.0f));
            horizontalGrid.addAndMakeVisible (btn);
            horizontalButtons.add (btn);
        }

        // Setup vertical grid
        verticalGrid.setGap (8);
        addAndMakeVisible (verticalGrid);

        // Add colorful buttons to vertical grid
        for (int i = 0; i < 6; i++)
        {
            auto* btn = new juce::TextButton (juce::String::charToString ('A' + i));
            btn->setSize (80, 35);
            btn->setColour (juce::TextButton::buttonColourId, juce::Colour::fromHSV ((i + 4) / 10.0f, 0.6f, 0.9f, 1.0f));
            verticalGrid.addAndMakeVisible (btn);
            verticalButtons.add (btn);
        }

        // Add instruction label
        addAndMakeVisible (instructionLabel);
        instructionLabel.setText ("Drag and drop items to reorder them", juce::dontSendNotification);
        instructionLabel.setJustificationType (juce::Justification::centred);
        instructionLabel.setColour (juce::Label::textColourId, juce::Colours::white);

        // Add section labels
        addAndMakeVisible (horizontalLabel);
        horizontalLabel.setText ("Horizontal Grid:", juce::dontSendNotification);
        horizontalLabel.setColour (juce::Label::textColourId, juce::Colours::white);

        addAndMakeVisible (verticalLabel);
        verticalLabel.setText ("Vertical Grid:", juce::dontSendNotification);
        verticalLabel.setColour (juce::Label::textColourId, juce::Colours::white);

        // Setup callbacks
        horizontalGrid.onOrderChanged = [] (int oldIndex, int newIndex)
        {
            juce::ignoreUnused (oldIndex, newIndex);
            DBG ("Horizontal grid: moved item from " << oldIndex << " to " << newIndex);
        };

        verticalGrid.onOrderChanged = [] (int oldIndex, int newIndex)
        {
            juce::ignoreUnused (oldIndex, newIndex);
            DBG ("Vertical grid: moved item from " << oldIndex << " to " << newIndex);
        };
    }

    ~ComponentGridDemo() override
    {
        horizontalButtons.clear (true);
        verticalButtons.clear (true);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (20);

        // Instruction at top
        instructionLabel.setBounds (bounds.removeFromTop (25));
        bounds.removeFromTop (10);

        // Horizontal section
        horizontalLabel.setBounds (bounds.removeFromTop (20));
        bounds.removeFromTop (5);
        horizontalGrid.setBounds (bounds.removeFromTop (60));
        bounds.removeFromTop (30);

        // Vertical section
        verticalLabel.setBounds (bounds.removeFromTop (20));
        bounds.removeFromTop (5);
        verticalGrid.setBounds (bounds.removeFromLeft (100));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
    }

    gin::ComponentGrid horizontalGrid;
    gin::ComponentGrid verticalGrid;
    juce::OwnedArray<juce::TextButton> horizontalButtons;
    juce::OwnedArray<juce::TextButton> verticalButtons;
    juce::Label instructionLabel;
    juce::Label horizontalLabel;
    juce::Label verticalLabel;
};
