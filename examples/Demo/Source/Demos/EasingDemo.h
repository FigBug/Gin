/*
  ==============================================================================

    EasingDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct EasingDemo : public DemoComponent
{
    EasingDemo()
    {
        setName ("Easing");
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        // Create a list of all easing functions with their names
        struct EasingFunction
        {
            juce::String name;
            std::function<float(float)> func;
        };

        std::vector<EasingFunction> easingFunctions = {
            {"Linear", [](float p) { return gin::easeLinear(p); }},
            {"QuadIn", [](float p) { return gin::easeQuadraticIn(p); }},
            {"QuadOut", [](float p) { return gin::easeQuadraticOut(p); }},
            {"QuadInOut", [](float p) { return gin::easeQuadraticInOut(p); }},
            {"CubicIn", [](float p) { return gin::easeCubicIn(p); }},
            {"CubicOut", [](float p) { return gin::easeCubicOut(p); }},
            {"CubicInOut", [](float p) { return gin::easeCubicInOut(p); }},
            {"QuartIn", [](float p) { return gin::easeQuarticIn(p); }},
            {"QuartOut", [](float p) { return gin::easeQuarticOut(p); }},
            {"QuartInOut", [](float p) { return gin::easeQuarticInOut(p); }},
            {"QuintIn", [](float p) { return gin::easeQuinticIn(p); }},
            {"QuintOut", [](float p) { return gin::easeQuinticOut(p); }},
            {"QuintInOut", [](float p) { return gin::easeQuinticInOut(p); }},
            {"SineIn", [](float p) { return gin::easeSineIn(p); }},
            {"SineOut", [](float p) { return gin::easeSineOut(p); }},
            {"SineInOut", [](float p) { return gin::easeSineInOut(p); }},
            {"CircIn", [](float p) { return gin::easeCircularIn(p); }},
            {"CircOut", [](float p) { return gin::easeCircularOut(p); }},
            {"CircInOut", [](float p) { return gin::easeCircularInOut(p); }},
            {"ExpoIn", [](float p) { return gin::easeExponentialIn(p); }},
            {"ExpoOut", [](float p) { return gin::easeExponentialOut(p); }},
            {"ExpoInOut", [](float p) { return gin::easeExponentialInOut(p); }},
            {"ElasticIn", [](float p) { return gin::easeElasticIn(p); }},
            {"ElasticOut", [](float p) { return gin::easeElasticOut(p); }},
            {"ElasticInOut", [](float p) { return gin::easeElasticInOut(p); }},
            {"BackIn", [](float p) { return gin::easeBackIn(p); }},
            {"BackOut", [](float p) { return gin::easeBackOut(p); }},
            {"BackInOut", [](float p) { return gin::easeBackInOut(p); }},
            {"BounceIn", [](float p) { return gin::easeBounceIn(p); }},
            {"BounceOut", [](float p) { return gin::easeBounceOut(p); }},
            {"BounceInOut", [](float p) { return gin::easeBounceInOut(p); }}
        };

        // Calculate grid layout
        const int cols = 6;
        const int rows = (int(easingFunctions.size()) + cols - 1) / cols;

        const int cellWidth = getWidth() / cols;
        const int cellHeight = getHeight() / rows;
        const int padding = 10;
        const int textHeight = 15;

        // Draw each easing function
        for (size_t i = 0; i < easingFunctions.size(); i++)
        {
            int col = int(i) % cols;
            int row = int(i) / cols;

            juce::Rectangle<int> cell (col * cellWidth, row * cellHeight, cellWidth, cellHeight);
            juce::Rectangle<int> graphArea = cell.reduced (padding).withTrimmedBottom (textHeight);

            // Draw a subtle box around each cell
            g.setColour (juce::Colours::grey.withAlpha (0.3f));
            g.drawRect (cell.reduced (2));

            // Draw the easing curve
            juce::Path path;
            const int steps = 100;

            // First, build the path in normalized coordinates
            for (int step = 0; step <= steps; step++)
            {
                float t = step / float(steps);
                float easedValue = easingFunctions[i].func (t);

                if (step == 0)
                    path.startNewSubPath (t, easedValue);
                else
                    path.lineTo (t, easedValue);
            }

            // Get the actual bounds of the path (handles overshoot)
            auto pathBounds = path.getBounds();

            // Create transform to fit path into graphArea
            // Flip Y axis since screen coordinates go top-down
            auto transform = juce::AffineTransform()
                .translated (-pathBounds.getX(), -pathBounds.getY())  // Move to origin
                .scaled (graphArea.getWidth() / pathBounds.getWidth(),
                        -graphArea.getHeight() / pathBounds.getHeight())  // Scale and flip Y
                .translated (graphArea.getX(), graphArea.getBottom());  // Move to position

            path.applyTransform (transform);

            g.setColour (juce::Colours::cyan);
            g.strokePath (path, juce::PathStrokeType (2.0f));

            // Draw reference axes
            g.setColour (juce::Colours::grey.withAlpha (0.5f));
            g.drawRect (graphArea);

            // Draw label
            g.setColour (juce::Colours::white);
            g.setFont (12.0f);
            juce::Rectangle<int> textArea = cell.removeFromBottom (textHeight);
            g.drawText (easingFunctions[i].name, textArea, juce::Justification::centred);
        }
    }
};
