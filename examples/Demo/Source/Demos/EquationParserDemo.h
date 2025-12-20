/*
  ==============================================================================

    EquationParserDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct EquationParserDemo : public DemoComponent
{
    EquationParserDemo()
    {
        setName ("Equation Parser");

        parser.defineNameChars ("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.");

        equation.setText ("2 + 2", juce::dontSendNotification);
        equation.onReturnKey = [this]
        {
            parser.setEquation (equation.getText());
            auto res = parser.evaluate();

            if (parser.hasError())
                result.setText (parser.getError(), juce::dontSendNotification);
            else
                result.setText (juce::String (res), juce::dontSendNotification);
        };
        addAndMakeVisible (equation);

        result.setReadOnly (true);
        addAndMakeVisible (result);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        equation.setBounds (rc.removeFromTop (25));
        rc.removeFromTop (8);
        result.setBounds (rc.removeFromTop (25));
    }

    gin::EquationParser parser;

    juce::TextEditor equation;
    juce::TextEditor result;
};
