/*
  ==============================================================================

    LagrangeDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct LagrangeDemo : public DemoComponent
{
    LagrangeDemo()
    {
        setName ("Lagrange");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
            points.clear();
        else if (points.size() == 0 || e.x > points.getLast().getX())
            points.add ({e.position.x, e.position.y});

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        juce::Array<gin::Point<double>> dpoints;
        for (auto p : points)
            dpoints.add ({ double (p.getX()), double (p.getY())});

        if (dpoints.size() >= 2)
        {
            g.setColour (juce::Colours::red);

            juce::Path p;

            p.startNewSubPath (points.getFirst().x, points.getFirst().y);
            for (float x = points.getFirst().getX(); x < points.getLast().getX(); x++)
            {
                float y = gin::Lagrange::interpolate (points, float (x));
                p.lineTo (x, y);
            }
            p.lineTo (points.getLast().x, points.getLast().y);

            g.strokePath (p, juce::PathStrokeType (2));
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click from left to right to add points. Double click to reset.", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<gin::Point<float>> points;
};
