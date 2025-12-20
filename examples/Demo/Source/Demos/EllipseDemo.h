/*
  ==============================================================================

    EllipseDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct EllipseDemo : public DemoComponent
{
    EllipseDemo()
    {
        setName ("Ellipse");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (points.size() >= 2)
            points.clear();

        points.add ({e.position.x, e.position.y});

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bnds = getLocalBounds().toFloat();

        auto a = getWidth() / 2.0f;
        auto b = getHeight() / 2.0f;

        g.setColour (juce::Colours::green);
        g.drawEllipse (bnds.getCentreX() - a / 2, bnds.getCentreY() - b / 2, a, b, 2.0f);

        if (points.size() == 2)
        {
            g.drawLine (juce::Line<float> (points[0], points[1]), 2.0f);

            gin::Ellipse<float> ellipse (0, 0, a, b);
            auto hits = ellipse.findIntersections (points[0] - bnds.getCentre(), points[1] - bnds.getCentre());

            g.setColour (juce::Colours::red);
            for (auto h : hits)
                g.fillEllipse (h.x - 3.0f + bnds.getCentreX(), h.y - 3.0f + bnds.getCentreY(), 6.0f, 6.0f);
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click twice to add a line, click again to start a new line", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<float>> points;
};
