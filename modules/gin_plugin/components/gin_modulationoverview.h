#pragma once

//==============================================================================
/** A button and text readout that shows the current modulation source
*/
class ModulationOverview : public juce::Component,
                           private IModMatrix::Listener,
                           private juce::Timer
{
public:
    ModulationOverview (IModMatrix& mm)
        : modMatrix (mm)
    {
        modMatrix.addListener (this);
        learnSourceChanged (modMatrix.getLearn ());

        addAndMakeVisible (button);

        name.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (name);
    }

    ~ModulationOverview() override
    {
        modMatrix.removeListener (this);
    }

private:
    void visibilityChanged() override
    {
        if (isVisible())
            startTimerHz (60);
        else
            stopTimer();

        timerCallback();
    }

    void timerCallback() override
    {
        auto phase = float (std::fmod (juce::Time::getMillisecondCounterHiRes() / 2000.0f, 1.0f));

        auto c1 = findColour (GinLookAndFeel::whiteColourId);
        auto c2 = findColour (GinLookAndFeel::accentColourId);

        if (phase < 0.5f)
            phase *= 2.0f;
        else
            phase = 1.0f - ((phase - 0.5f) * 2.0f);

        auto col = c1.overlaidWith (c2.withAlpha (phase));

        button.setColour (GinLookAndFeel::whiteColourId, col);
        name.setColour (juce::Label::textColourId, col);
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (e.mouseWasClicked())
            modMatrix.disableLearn();
    }

    void resized() override
    {
        auto rc = getLocalBounds();
        button.setBounds (rc.removeFromLeft (rc.getHeight()));
        rc.removeFromLeft (4);
        name.setBounds (rc);
    }

    void learnSourceChanged (ModSrcId src) override
    {
        setVisible (src.isValid());
        button.setSource (src, ! src.isValid() ? false : modMatrix.getModSrcPoly (src));

        if (src.isValid())
            name.setText (modMatrix.getModSrcName (src), juce::dontSendNotification);
        else
            name.setText ({}, juce::dontSendNotification);
    }

    IModMatrix& modMatrix;

    ModulationSourceButton button { modMatrix, {} };
    juce::Label name;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationOverview)
};
