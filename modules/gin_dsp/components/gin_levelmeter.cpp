LevelMeter::LevelMeter (const LevelTracker& l, juce::NormalisableRange<float> r) :
    tracker (l),
    range (r)
{
    startTimerHz (60);

    setColour (lineColourId, juce::Colours::white);
    setColour (meterColourId, juce::Colours::white.withAlpha (0.3f));
}

LevelMeter::~LevelMeter()
{
    stopTimer();
}

//==============================================================================

void LevelMeter::paint (juce::Graphics& g)
{
    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());

    auto level = juce::jlimit (range.start, range.end, tracker.getLevel());

    auto rc = getLocalBounds();

    if (topDown)
    {
        rc.removeFromBottom (juce::roundToInt (range.convertTo0to1 (level) * float ( getHeight() )));
        g.setColour (findColour (meterColourId));
        g.fillRect (rc);
    }
    else
    {
        rc = rc.removeFromBottom (juce::roundToInt (range.convertTo0to1 (level) * float ( getHeight() )));
        g.setColour (findColour (meterColourId));
        g.fillRect (rc);
    }
}

void LevelMeter::timerCallback()
{
    repaint();
}
