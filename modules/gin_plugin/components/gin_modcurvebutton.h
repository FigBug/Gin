#pragma once

//==============================================================================
/** A button that displays mod curve
*/
class ModCurveButton : public juce::Button
{
public:
    ModCurveButton() : juce::Button ("")
    {
    }

    void setCurve (ModMatrix::Function c)
    {
        curve = c;
        repaint();
    }

    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        auto c = findColour (getToggleState() ? GinLookAndFeel::whiteColourId : GinLookAndFeel::accentColourId).withAlpha (0.9f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c.withMultipliedAlpha (isEnabled() ? 1.0f : 0.5f));

        auto rc = getLocalBounds().toFloat().reduced (1.0f);

        juce::Path p;
        for (auto x = 0.0f; x <= rc.getWidth(); x += 0.5f)
        {
            auto y = (1.0f - ModMatrix::shape (x / rc.getWidth(), curve, false, false)) * rc.getHeight();

            if (x == 0.0f)
                p.startNewSubPath (rc.getX() + x, rc.getY() + y);
            else
                p.lineTo (rc.getX() + x, rc.getY() + y);
        }

        g.strokePath (p, juce::PathStrokeType (1.0f));
    }

private:
    ModMatrix::Function curve;
};
