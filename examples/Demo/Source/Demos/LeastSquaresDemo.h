/*
  ==============================================================================

    LeastSquaresDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct LeastSquaresDemo : public DemoComponent
{
    LeastSquaresDemo()
    {
        setName ("Least Squares");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
        {
            lsr.clear();
            points.clear();
        }
        else
        {
            lsr.addPoint (e.x, e.y);
            points.add (e.getPosition());
        }
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        if (lsr.enoughPoints())
        {
            g.setColour (juce::Colours::red);

            juce::Path p;

            double a = lsr.aTerm();
            double b = lsr.bTerm();
            double c = lsr.cTerm();

            for (int x = 0; x < getWidth(); x++)
            {
                double y = a * x * x + b * x + c;
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
    gin::LeastSquaresRegression lsr;
};
