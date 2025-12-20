/*
  ==============================================================================

    LinearDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct LinearDemo : public DemoComponent
{
    LinearDemo()
    {
        setName ("Linear Regression");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
        {
            lr.clear();
            points.clear();
        }
        else
        {
            lr.addPoint ({double (e.x), double (e.y)});
            lr.calculate();
            points.add (e.getPosition());
        }
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        if (lr.haveData())
        {
            g.setColour (juce::Colours::red);

            juce::Path p;

            for (int x = 0; x < getWidth(); x++)
            {
                double y = lr.estimateY (x);
                if (x == 0)
                    p.startNewSubPath (float (x), float (y));
                else
                    p.lineTo (float (x), float (y));
            }
            g.strokePath (p, juce::PathStrokeType (2));
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click to add point. Double click to reset.", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<int>> points;
    gin::LinearRegression lr;
};
