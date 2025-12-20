/*
  ==============================================================================

    CatenaryDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct CatenaryDemo : public DemoComponent
{
    CatenaryDemo()
    {
        setName ("Catenary");
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
        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.size() == 2)
        {
            auto h = float (getHeight());
            auto p1 = points[0];
            auto p2 = points[1];

            if (p1.x > p2.x)
                std::swap (p1, p2);

            gin::Catenary catenary (p1.x, h - p1.y, p2.x, h - p2.y, juce::Line<float> (p1, p2).getLength() * 0.2f);

            juce::Path p;
            p.startNewSubPath (p1.x, p1.y);

            for (auto x = p1.x + 2.0f; x < p2.x - 2.0f; x += 2.0f)
                p.lineTo (x, h - catenary.calcY (x));
            p.lineTo (p2.x, p2.y);

            g.strokePath (p, juce::PathStrokeType (1.5f));
        }

        if (points.isEmpty())
            g.drawText ("Click twice to add a wire, click again to start a new wire", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<float>> points;
};
