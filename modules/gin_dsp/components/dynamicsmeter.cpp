DynamicsMeter::DynamicsMeter (Dynamics& d) :
    dynamics (d)
{
    startTimerHz (60);
    
    setColour (lineColourId, Colours::white);
    setColour (meterColourId, Colours::white.withAlpha (0.3f));
}

DynamicsMeter::~DynamicsMeter()
{
    stopTimer();
}

//==============================================================================

void DynamicsMeter::paint (Graphics& g)
{
    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());
    
    auto in  = jlimit (range.start, range.end, dynamics.getInputTracker().getLevel());
    auto out = jlimit (range.start, range.end, dynamics.getOutputTracker().getLevel());
    
    g.setColour (findColour (meterColourId));
    
    auto rl = getLocalBounds().removeFromLeft (roundToInt (range.convertTo0to1 (in) * getWidth()));
    auto rb = getLocalBounds().removeFromBottom (roundToInt (range.convertTo0to1 (out) * getHeight()));
    
    g.fillRect (rl);
    g.fillRect (rb);
    
    g.setColour (findColour (lineColourId));
    
    Path p;
    for (int x = 0; x < getWidth(); x++)
    {
        auto dbIn  = range.convertFrom0to1 (float (x) / getWidth());
        auto dbOut = dynamics.calcCurve (dbIn);
        
        float y = getHeight() - range.convertTo0to1 (dbOut) * getHeight ();
        
        if (x == 0)
            p.startNewSubPath (x, y);
        else
            p.lineTo (x, y);
    }
    
    g.strokePath (p, PathStrokeType (1.5f));
}

void DynamicsMeter::timerCallback()
{
    repaint();
}
