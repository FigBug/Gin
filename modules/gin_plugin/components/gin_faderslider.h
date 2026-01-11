#pragma once

//==============================================================================
/** A vertical fader slider for controlling a parameter with custom drawing.
    Draws a stylized vertical fader with track, fill from center, and fader cap.
*/
class FaderSlider : public PluginSlider
{
public:
    FaderSlider (Parameter* parameter_)
      : PluginSlider (parameter_, juce::Slider::LinearVertical, juce::Slider::NoTextBox),
        parameter (parameter_)
    {
        setSliderStyle (juce::Slider::LinearVertical);
        setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        auto trackWidth = 6.0f;
        auto trackX = bounds.getCentreX() - trackWidth / 2.0f;
        auto trackY = bounds.getY() + 8.0f;
        auto trackHeight = bounds.getHeight() - 16.0f;

        // Draw track background
        g.setColour (findColour (PluginLookAndFeel::grey30ColourId));
        g.fillRoundedRectangle (trackX, trackY, trackWidth, trackHeight, 3.0f);

        // Draw center line
        auto centreY = bounds.getCentreY();
        g.setColour (findColour (PluginLookAndFeel::grey60ColourId));
        g.drawHorizontalLine ((int) centreY, trackX, trackX + trackWidth);

        // Calculate fader position
        auto range = parameter->getUserRange();
        auto normValue = (float) valueToProportionOfLength (getValue());
        auto centreNorm = range.convertTo0to1 (0.0f);

        auto faderHeight = 16.0f;
        auto faderWidth = 20.0f;
        auto faderTravel = trackHeight - faderHeight;

        auto faderY = trackY + (1.0f - normValue) * faderTravel;

        // Draw fill from center to current position
        g.setColour (findColour (PluginLookAndFeel::accentColourId).withAlpha (0.5f));
        auto centreYPos = trackY + (1.0f - centreNorm) * faderTravel + faderHeight / 2.0f;
        auto currentYPos = faderY + faderHeight / 2.0f;

        if (currentYPos < centreYPos)
            g.fillRoundedRectangle (trackX, currentYPos, trackWidth, centreYPos - currentYPos, 2.0f);
        else
            g.fillRoundedRectangle (trackX, centreYPos, trackWidth, currentYPos - centreYPos, 2.0f);

        // Draw fader cap
        auto faderX = bounds.getCentreX() - faderWidth / 2.0f;
        auto faderRect = juce::Rectangle<float> (faderX, faderY, faderWidth, faderHeight);

        g.setColour (findColour (PluginLookAndFeel::grey60ColourId));
        g.fillRoundedRectangle (faderRect, 3.0f);

        // Draw fader groove
        g.setColour (findColour (PluginLookAndFeel::grey45ColourId));
        g.drawHorizontalLine ((int) (faderY + faderHeight / 2.0f), faderX + 3, faderX + faderWidth - 3);
    }

private:
    Parameter* parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaderSlider)
};
