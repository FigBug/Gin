LevelMeter::LevelMeter (const LevelTracker& l, NormalisableRange<float> r) :
    tracker (l),
    range (r)
{
    startTimerHz (60);
    
    setColour (lineColourId, Colours::white);
    setColour (meterColourId, Colours::white.withAlpha (0.3f));
}

LevelMeter::~LevelMeter()
{
    stopTimer();
}

//==============================================================================

void LevelMeter::paint (Graphics& g)
{
    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());
    
    auto level = jlimit (range.start, range.end, tracker.getLevel());

    auto rc = getLocalBounds();

    if (topDown)
    {
        rc.removeFromBottom (roundToInt (range.convertTo0to1 (level) * getHeight()));
        g.setColour (findColour (meterColourId));
        g.fillRect (rc);
    }
    else
    {
        rc = rc.removeFromBottom (roundToInt (range.convertTo0to1 (level) * getHeight()));
        g.setColour (findColour (meterColourId));
        g.fillRect (rc);
    }
}

void LevelMeter::timerCallback()
{
    repaint();
}
