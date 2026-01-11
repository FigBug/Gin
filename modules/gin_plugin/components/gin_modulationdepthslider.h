#pragma once

//==============================================================================
/** A slider for adjusting modulation depth
*/
class ModulationDepthSlider : public juce::Slider
{
public:
    ModulationDepthSlider() : juce::Slider (RotaryHorizontalVerticalDrag, NoTextBox)
    {

    }

    ~ModulationDepthSlider() override
    {
    }

    juce::String getTextFromValue (double value) override
    {
        if (onTextFromValue)
            return onTextFromValue (value);

        return juce::Slider::getTextFromValue (value);
    }

    std::function<void ()> onClick;
    std::function<juce::String (double)> onTextFromValue;

private:
    void paint (juce::Graphics& g) override
    {
        auto c = juce::Colours::white.withAlpha (0.4f);
        g.setColour (c);

        auto rc = getLocalBounds().toFloat().reduced (1.5f);
        g.fillEllipse (rc);

        if (auto v = float (getValue()); v > 0.0f || v < 0.0f)
        {
            g.setColour (findColour (PluginLookAndFeel::accentColourId, true).withAlpha (0.9f));

            juce::Path p;
            p.addPieSegment (rc, 0.0f, juce::MathConstants<float>::pi * 2 * v, 0.0f);

            g.fillPath (p);
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (e.mouseWasClicked() && e.mods.isPopupMenu() && onClick)
            onClick ();
    }
};
