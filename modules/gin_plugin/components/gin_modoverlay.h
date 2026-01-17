#pragma once

//==============================================================================
/** Overlay component that draws a catenary cable from the active modulation source
    to the mouse pointer during modulation learn mode.
    Add this as a child of your plugin editor and call setBounds to cover the whole editor.
*/
class ModOverlay : public juce::Component,
                   private juce::Timer
{
public:
    ModOverlay();
    ~ModOverlay() override;

    void paint (juce::Graphics& g) override;
    bool hitTest (int x, int y) override;

private:
    void timerCallback() override;

    juce::Component* findActiveSourceButton();
    bool isOverValidDestination();

    juce::Point<float> sourcePoint;
    juce::Point<float> mousePoint;
    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModOverlay)
};
