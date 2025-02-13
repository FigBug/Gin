LevelMeter::LevelMeter (const LevelTracker& l, juce::NormalisableRange<float> r) :
    tracker (l),
    range (r)
{
    startTimerHz (60);

    setColour ( lineColourId, juce::Colours::white);
    setColour ( meterColourId, juce::Colours::white.withAlpha ( 0.3f ) );
    setColour ( backgroundColourId, juce::Colours::white.withAlpha ( 0.1f ) );
    setColour ( clipColourId, juce::Colours::red );
}

LevelMeter::~LevelMeter()
{
    stopTimer();
}

//==============================================================================

void LevelMeter::paint (juce::Graphics& g)
{
    g.setColour ( findColour ( lineColourId ) );
    g.drawRect ( getLocalBounds () );

    auto level = juce::jlimit ( range.start, range.end, tracker.getLevel () );
    auto rc = getLocalBounds ().toFloat ();

    if ( topDown )
    {
        g.setColour ( findColour ( backgroundColourId ) );
        g.fillRect ( rc.removeFromBottom ( range.convertTo0to1 ( level ) * rc.getHeight () ) );

        g.setColour ( findColour ( meterColourId ) );
        g.fillRect ( rc );

        if ( tracker.getClip () )
        {
            g.setColour ( findColour ( clipColourId ) );
            g.fillRect ( getLocalBounds ().toFloat ().withTrimmedTop ( getHeight () * 0.95f ) );
        }
    }
    else
    {
        g.setColour ( findColour ( meterColourId ) );
        g.fillRect ( rc.removeFromBottom ( range.convertTo0to1 ( level ) * rc.getHeight () ) );

        g.setColour ( findColour ( backgroundColourId ) );
        g.fillRect ( rc );

        if ( tracker.getClip () )
        {
            g.setColour ( findColour ( clipColourId ) );
            g.fillRect ( getLocalBounds ().toFloat ().withTrimmedBottom ( getHeight () * 0.95f ) );
        }
    }
}

void LevelMeter::timerCallback()
{
    repaint();
}
