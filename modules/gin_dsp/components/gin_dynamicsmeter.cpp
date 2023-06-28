DynamicsMeter::DynamicsMeter (Dynamics& d) :
    dynamics (d)
{
    startTimerHz (60);

    setColour (lineColourId, juce::Colours::white);
    setColour (meterColourId, juce::Colours::white.withAlpha (0.3f));
}

DynamicsMeter::~DynamicsMeter()
{
    stopTimer();
}

//==============================================================================

void DynamicsMeter::paint (juce::Graphics& g)
{
    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());

    auto in  = juce::jlimit (range.start, range.end, dynamics.getInputTracker().getLevel());
    auto out = juce::jlimit (range.start, range.end, dynamics.getOutputTracker().getLevel());

    g.setColour (findColour (meterColourId));

    auto rl = getLocalBounds().removeFromLeft (juce::roundToInt (range.convertTo0to1 (in) * float ( getWidth() )));
    auto rb = getLocalBounds().removeFromBottom (juce::roundToInt (range.convertTo0to1 (out) * float ( getHeight() )));

    g.fillRect (rl);
    g.fillRect (rb);

    g.setColour (findColour (lineColourId));

    juce::Path p;
    for (float x = 0; x < float ( getWidth() ) + 1; x++)
    {
        auto dbIn  = range.convertFrom0to1 (float (x) / float ( std::max (1, getWidth() )));
        auto dbOut = dynamics.calcCurve (dbIn);

        dbOut = dynamics.calcCurve (dbIn);

        float y = float (getHeight()) - range.convertTo0to1 (juce::jlimit (range.start, range.end, dbOut)) * float (getHeight ());

        if (juce::exactlyEqual (x, 0.0f))
            p.startNewSubPath (x, y);
        else
            p.lineTo (x, y);
    }

    g.strokePath (p, juce::PathStrokeType (1.5f));
}

void DynamicsMeter::timerCallback()
{
    repaint();
}
