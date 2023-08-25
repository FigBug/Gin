
WavetableComponent::WavetableComponent()
{
}

WavetableComponent::~WavetableComponent()
{
}

//==============================================================================

void WavetableComponent::setParams (WTOscillator::Params params_)
{
    if (! almostEqual (params.pw, params_.pw))
    {
        params = params_;
        repaint();
    }
}

void WavetableComponent::setWavetables (juce::OwnedArray<BandLimitedLookupTable>* bllt_)
{
    bllt = bllt_;
    needsUpdate = true;
    repaint();
}

void WavetableComponent::paint (juce::Graphics& g)
{
    if (needsUpdate && bllt)
    {
        needsUpdate = false;

        auto numTables = std::min (32, bllt->size());
        for (auto i = 0; i < numTables; i++)
            paths.add (createWavetablePath (float (i) / numTables));
    }

    if (paths.size() > 0)
    {
        g.setColour (findColour (waveColourId, true));
        for (auto& p : paths)
            g.strokePath (p, juce::PathStrokeType (0.75f));
        
        g.setColour (findColour (activeWaveColourId, true));
        g.strokePath (createWavetablePath (params.pw), juce::PathStrokeType (0.75f));
    }
}

juce::Path WavetableComponent::createWavetablePath (float wtPos)
{
    constexpr auto samples = 256;

    juce::AudioSampleBuffer buf (2, samples);

    {
        WTOscillator osc;

        auto hz = 44100.0f / samples;
        auto note = getMidiNoteFromHertz (hz);
        auto p = params;

        p.pw = wtPos;

        osc.setSampleRate (44100.0);
        osc.setWavetable (*bllt);
        osc.noteOn (0.0f);
        osc.process (note, p, buf);
    }

    juce::Path p;

    auto w = float (getWidth());
    auto h = float (getHeight());

    auto data = buf.getReadPointer (0);

    auto xSpread = 0.5f;
    auto yScale = -(1.0f / 4.5f);
    auto dx = std::min (w, h);

    auto xSlope = (dx * 1.5f * (1.0f - xSpread)) / float (samples);
    auto ySlope = xSlope / 4.0f;

    auto xOffset = (dx * xSpread) * wtPos;
    auto yOffset = (dx * xSpread) - xOffset;

    xOffset += (w - dx * xSpread - samples * xSlope) / 2.0f;
    yOffset += (h - dx * xSpread - samples * ySlope) / 2.0f;

    p.startNewSubPath (xOffset, data[0] * yScale * dx + yOffset);

    for (auto s = 1; s < samples; s++)
    {
        p.lineTo (xOffset, data[s] * yScale * dx + yOffset);

        xOffset += xSlope;
        yOffset += ySlope;
    }

    return p;
}
//-------------------------------------------------------------------------------------------------


