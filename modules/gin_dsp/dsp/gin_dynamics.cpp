
//================================================================================
const float DIGITAL_TC = -2.0f; // log(1%)
const float ANALOG_TC = -0.43533393574791066201247090699309f; // (log(36.7%)

//================================================================================
void EnvelopeDetector::reset()
{
    envelope = 0.0;
    rmsState = 0.0f;
}

void EnvelopeDetector::setRMSWindow (float seconds)
{
    rmsWindow = seconds;

    if (seconds <= 0.0f)
    {
        // No window: the average is the sample, which is the old behaviour
        rmsA = 1.0f;
        rmsB = 0.0f;
    }
    else
    {
        rmsB = float (std::exp (-1.0 / (seconds * sampleRate)));
        rmsA = 1.0f - rmsB;
    }
}

void EnvelopeDetector::setParams (float attackS_, float holdS_, float releaseS_, bool analogTC_, Mode detect_, bool logDetector_)
{
    analogTC = analogTC_;
    mode = detect_;
    logDetector = logDetector_;

    setAttackTime (attackS_);
    setHoldTime (holdS_);
    setReleaseTime (releaseS_);
}

void EnvelopeDetector::setHoldTime (float holdS)
{
    holdTime = holdS;
}

void EnvelopeDetector::setAttackTime (float attackS)
{
    if (analogTC)
        attackTime = (float) std::exp (ANALOG_TC / (attackS * sampleRate));
    else
        attackTime = (float) std::exp (DIGITAL_TC / (attackS * sampleRate));
}

void EnvelopeDetector::setReleaseTime (float releaseS)
{
    if (analogTC)
        releaseTime = (float) std::exp (ANALOG_TC / (releaseS * sampleRate));
    else
        releaseTime = (float) std::exp (DIGITAL_TC / (releaseS * sampleRate));
}

float EnvelopeDetector::process (float input)
{
    switch (mode)
    {
        case peak:
            input = std::fabs (input);
            break;
        case ms:
            // Mean square: average the squares over the window
            rmsState = rmsA * (input * input) + rmsB * rmsState;
            input = rmsState;
            break;
        case rms:
            rmsState = rmsA * (input * input) + rmsB * rmsState;
            input = std::sqrt (rmsState);
            break;
    }

    if (input > envelope)
    {
        envelope = attackTime * (envelope - input) + input;
        holdRemaining = holdTime;
    }
    else if (holdTime > 0.0f && holdRemaining > 0.0f)
    {
        holdRemaining -= 1.0f / float (sampleRate);
    }
    else
    {
        envelope = releaseTime * (envelope - input) + input;
    }

    envelope = std::max (0.0f, envelope);

    if (logDetector)
    {
        if (envelope == 0.0f)
            return -100.0f;

        return juce::Decibels::gainToDecibels (envelope);
    }

    return envelope;
}

//================================================================================
void Dynamics::setSampleRate (double sampleRate_)
{
    sampleRate = sampleRate_;

    for (auto e : envelopes)
        e->setSampleRate (sampleRate);

    updateDetectorParams();
    updateLookahead();
    updateDetectorFilters (int (detectorFilters.size()));

    reset();
}

void Dynamics::setNumChannels (int ch)
{
    channels = ch;

    while (envelopes.size() < channels)
    {
        auto e = new EnvelopeDetector();
        e->setSampleRate (sampleRate);
        envelopes.add (e);
    }
    while (envelopes.size() > channels)
        envelopes.removeLast();

    updateDetectorParams();
    updateLookahead();
}

void Dynamics::setParams (float attackS, float holdS, float releaseS, float threshold_, float ratio_, float kneeWidth_)
{
    attackSeconds = attackS;
    holdSeconds = holdS;
    releaseSeconds = releaseS;

    updateDetectorParams();

    threshold = threshold_;
    ratio = ratio_;
    kneeWidth = kneeWidth_;
}

void Dynamics::setDetectorMode (EnvelopeDetector::Mode m)
{
    detectorMode = m;
    updateDetectorParams();
}

void Dynamics::setRMSWindow (float seconds)
{
    rmsWindow = seconds;
    updateDetectorParams();
}

void Dynamics::setAnalogTC (bool analog)
{
    analogTC = analog;
    updateDetectorParams();
}

void Dynamics::updateDetectorParams()
{
    for (auto e : envelopes)
    {
        e->setRMSWindow (rmsWindow);
        e->setParams (attackSeconds, holdSeconds, releaseSeconds, analogTC, detectorMode, true);
    }
}

void Dynamics::setLookahead (float seconds)
{
    lookaheadSeconds = std::max (0.0f, seconds);
    updateLookahead();
}

void Dynamics::updateLookahead()
{
    auto wanted = int (lookaheadSeconds * sampleRate);

    if (wanted != lookaheadSamples || int (lookaheadBuffers.size()) != channels)
    {
        lookaheadSamples = wanted;
        lookaheadBuffers.assign (size_t (std::max (0, channels)),
                                 std::vector<float> (size_t (std::max (1, lookaheadSamples)), 0.0f));
        lookaheadPos = 0;
    }
}

void Dynamics::setSidechainFilter (float highpassHz, float lowpassHz)
{
    sidechainHighpass = highpassHz;
    sidechainLowpass = lowpassHz;
    updateDetectorFilters (int (detectorFilters.size()));
}

void Dynamics::setDetectorPeak (float freqHz, float q, float gainDb)
{
    detectorPeakFreq = freqHz;
    detectorPeakQ = q;
    detectorPeakGain = gainDb;
    updateDetectorFilters (int (detectorFilters.size()));
}

void Dynamics::updateDetectorFilters (int numChannels)
{
    auto nyquist = float (sampleRate / 2.1);

    auto hpOn   = sidechainHighpass > 0.0f && sidechainHighpass < nyquist;
    auto lpOn   = sidechainLowpass  > 0.0f && sidechainLowpass  < nyquist;
    auto peakOn = detectorPeakGain != 0.0f && detectorPeakFreq > 0.0f && detectorPeakFreq < nyquist;

    detectorFilterActive = hpOn || lpOn || peakOn;

    if (numChannels <= 0)
        return;

    if (int (detectorFilters.size()) != numChannels)
        detectorFilters.resize (size_t (numChannels));

    for (auto& f : detectorFilters)
    {
        if (hpOn)   f.highpass.setHighpass (sidechainHighpass, juce::MathConstants<double>::sqrt2 / 2.0, sampleRate);
        else        f.highpass.setBypass();

        if (lpOn)   f.lowpass.setLowpass (sidechainLowpass, juce::MathConstants<double>::sqrt2 / 2.0, sampleRate);
        else        f.lowpass.setBypass();

        if (peakOn) f.peak.setPeak (detectorPeakFreq, detectorPeakQ, detectorPeakGain, sampleRate);
        else        f.peak.setBypass();
    }
}

void Dynamics::reset()
{
    for (auto e : envelopes)
        e->reset();

    for (auto& b : lookaheadBuffers)
        std::fill (b.begin(), b.end(), 0.0f);

    lookaheadPos = 0;

    for (auto& f : detectorFilters)
    {
        f.highpass.reset();
        f.lowpass.reset();
        f.peak.reset();
    }
}

void Dynamics::process (juce::AudioSampleBuffer& buffer, juce::AudioSampleBuffer* envelopeOut)
{
    processInternal (buffer, nullptr, envelopeOut);
}

void Dynamics::process (juce::AudioSampleBuffer& buffer, const juce::AudioSampleBuffer& sidechain, juce::AudioSampleBuffer* envelopeOut)
{
    processInternal (buffer, &sidechain, envelopeOut);
}

void Dynamics::processInternal (juce::AudioSampleBuffer& buffer, const juce::AudioSampleBuffer* sidechain, juce::AudioSampleBuffer* envelopeOut)
{
    inputTracker.trackBuffer (buffer);

    int numSamples = buffer.getNumSamples();

    auto input  = buffer.getArrayOfReadPointers();
    auto output = buffer.getArrayOfWritePointers();
    auto env    = envelopeOut != nullptr ? envelopeOut->getArrayOfWritePointers() : nullptr;

    jassert (sidechain == nullptr || sidechain->getNumChannels() > 0);
    jassert (sidechain == nullptr || sidechain->getNumSamples() >= numSamples);

    // A side chain that cannot cover the buffer is dropped rather than read
    // off the end of: keying off the signal is always a defensible fallback,
    // and the asserts above say so in a debug build.
    if (sidechain != nullptr && (sidechain->getNumChannels() <= 0 || sidechain->getNumSamples() < numSamples))
        sidechain = nullptr;

    // What the detector follows: the side chain if there is one, otherwise
    // the signal itself. The gain still lands on the signal either way.
    auto detect         = sidechain != nullptr ? sidechain->getArrayOfReadPointers() : input;
    auto detectChannels = sidechain != nullptr ? sidechain->getNumChannels() : channels;

    // Side chain and detector filters shape what the detector hears, so they
    // have to run on a copy - the whole point is that none of it is audible.
    // One pass here rather than inside the sample loop, because a key channel
    // can be read by several output channels and its filter state must only
    // advance once per sample.
    ScratchBuffer filtered (detectorFilterActive ? detectChannels : 0,
                            detectorFilterActive ? numSamples : 0);

    if (detectorFilterActive && detectChannels > 0)
    {
        if (int (detectorFilters.size()) != detectChannels)
            updateDetectorFilters (detectChannels);

        for (int c = 0; c < detectChannels; c++)
        {
            auto& f = detectorFilters[size_t (c)];
            auto* d = filtered.getWritePointer (c);

            juce::FloatVectorOperations::copy (d, detect[c], numSamples);

            for (int i = 0; i < numSamples; i++)
                d[i] = f.peak.process (f.lowpass.process (f.highpass.process (d[i])));
        }

        detect = filtered.getArrayOfReadPointers();
    }

    // Fewer key channels than the processor has: the last one drives the
    // rest, so a mono side chain is a valid thing to be handed.
    auto keyOf = [&] (int c, int i) { return detect[std::min (c, detectChannels - 1)][i]; };

    // Calculate auto makeup gain for compressor/limiter modes
    // Formula: makeup (dB) = -threshold * (1 - 1/ratio)
    // For limiter (infinite ratio): makeup = -threshold
    float autoMakeup = 1.0f;
    if (autoMakeupGain && (type == compressor || type == limiter))
    {
        float makeupDb = 0.0f;
        if (type == limiter)
            makeupDb = -threshold;
        else if (ratio > 1.0f)
            makeupDb = -threshold * (1.0f - 1.0f / ratio);

        autoMakeup = juce::Decibels::decibelsToGain (makeupDb);
    }

    float peakReduction = 1.0f;

    const bool lookingAhead = lookaheadSamples > 0 && int (lookaheadBuffers.size()) >= channels;

    // Reads the sample the gain should land on: with lookahead that is one
    // delay line behind what the detector just measured, which is what lets
    // the gain already be moving when the transient arrives.
    auto delayed = [&] (int c, float in) -> float
    {
        if (! lookingAhead)
            return in;

        auto& ring = lookaheadBuffers[size_t (c)];
        auto out = ring[size_t (lookaheadPos)];
        ring[size_t (lookaheadPos)] = in;
        return out;
    };

    for (int i = 0; i < numSamples; i++)
    {
        if (channelsLinked)
        {
            float linked = 0.0f;
            for (int c = 0; c < channels; c++)
            {
                float in = inputGain * keyOf (c, i);

                in = envelopes[c]->process (in);

                linked += juce::Decibels::decibelsToGain (in);
            }

            linked /= float ( channels );

            if (env != nullptr)
                env[0][i] = linked;

            linked = juce::Decibels::gainToDecibels (linked);

            auto gain = juce::Decibels::decibelsToGain (calcCurve (linked) - linked);
            peakReduction = std::min (peakReduction, gain);

            for (int c = 0; c < channels; c++)
                output[c][i] = inputGain * gain * delayed (c, input[c][i]) * outputGain * autoMakeup;
        }
        else
        {
            for (int c = 0; c < channels; c++)
            {
                float in = inputGain * keyOf (c, i);

                in = envelopes[c]->process (in);

                if (env != nullptr)
                    env[c][i] = juce::Decibels::decibelsToGain (in);

                auto gain = juce::Decibels::decibelsToGain (calcCurve (in) - in);
                peakReduction = std::min (peakReduction, gain);

                output[c][i] = inputGain * gain * delayed (c, input[c][i]) * outputGain * autoMakeup;
            }
        }

        if (lookingAhead && ++lookaheadPos >= lookaheadSamples)
            lookaheadPos = 0;
    }

    reductionTracker.trackSample (peakReduction);
    outputTracker.trackBuffer (buffer);
}

float Dynamics::calcCurve (float dbIn)
{
    if (type == compressor)
    {
        float dbOut = dbIn;

        if (kneeWidth > 0 && dbIn >= (threshold - kneeWidth / 2.0f) && dbIn <= threshold + kneeWidth / 2.0f)
            dbOut = dbIn + ((1.0f / ratio - 1.0f) * std::pow (dbIn - threshold + kneeWidth / 2.0f, 2.0f) / (2.0f * kneeWidth));
        else if (dbIn > threshold + kneeWidth / 2.0)
            dbOut = threshold + (dbIn - threshold) / ratio;

        return dbOut;
    }
    else if (type == limiter)
    {
        float dbOut = dbIn;

        if (kneeWidth > 0 && dbIn >= (threshold - kneeWidth / 2.0f) && dbIn <= threshold + kneeWidth / 2.0f)
            dbOut = dbIn + (1.0f * std::pow (dbIn - threshold + kneeWidth / 2.0f, 2.0f) / (2.0f * kneeWidth));
        else if (dbIn > threshold + kneeWidth / 2.0f)
            dbOut = threshold;

        return dbOut;
    }
    else if (type == expander)
    {
        float dbOut = dbIn;

        // soft-knee with detection value in range?
        if (kneeWidth > 0 && dbIn >= (threshold - kneeWidth / 2.0f) && dbIn <= threshold + kneeWidth / 2.0f)
            dbOut = dbIn - ((ratio - 1.0f) * std::pow ((dbIn - threshold - (kneeWidth / 2.0f)), 2.0f)) / (2.0f * kneeWidth);
        else if (dbIn < threshold + kneeWidth / 2.0)
            dbOut = threshold + (dbIn - threshold) * ratio;

        return dbOut;
    }
    else if (type == gate)
    {
        float dbOut = dbIn;

        if (kneeWidth > 0 && dbIn >= (threshold - kneeWidth / 2.0f) && dbIn <= threshold + kneeWidth / 2.0f)
            dbOut = dbIn - ((100.0f - 1.0f) * std::pow ((dbIn - threshold - (kneeWidth / 2.0f)), 2.0f)) / (2.0f * kneeWidth);
        else if (dbIn < threshold - kneeWidth / 2.0f)
            dbOut = -1000.0f;

        return dbOut;
    }
    else if (type == compexp)
    {
        float dbOut = dbIn;
        float expThreshold = -100.0f - threshold;

        // Compressor: above threshold
        if (kneeWidth > 0 && dbIn >= (threshold - kneeWidth / 2.0f) && dbIn <= threshold + kneeWidth / 2.0f)
            dbOut = dbIn + ((1.0f / ratio - 1.0f) * std::pow (dbIn - threshold + kneeWidth / 2.0f, 2.0f) / (2.0f * kneeWidth));
        else if (dbIn > threshold + kneeWidth / 2.0f)
            dbOut = threshold + (dbIn - threshold) / ratio;
        // Expander: below inverse threshold
        else if (kneeWidth > 0 && dbIn >= (expThreshold - kneeWidth / 2.0f) && dbIn <= expThreshold + kneeWidth / 2.0f)
            dbOut = dbIn - ((ratio - 1.0f) * std::pow ((dbIn - expThreshold - (kneeWidth / 2.0f)), 2.0f)) / (2.0f * kneeWidth);
        else if (dbIn < expThreshold - kneeWidth / 2.0f)
            dbOut = expThreshold + (dbIn - expThreshold) * ratio;

        return dbOut;
    }

    jassertfalse;
    return dbIn;
}
