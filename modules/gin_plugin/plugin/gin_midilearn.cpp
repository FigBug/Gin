//==============================================================================
MidiLearn::MidiLearn (gin::Processor& p)
    : processor (p)
{
    for (auto param : processor.getPluginParameters())
        param->setMidiLearn (this);

    loadFromSettings();
}

MidiLearn::~MidiLearn()
{
    for (auto param : processor.getPluginParameters())
        param->setMidiLearn (nullptr);
}

//==============================================================================
void MidiLearn::setMapping (int ccNumber, gin::Parameter* param)
{
    jassert (ccNumber >= 0 && ccNumber < 128);
    items[(size_t) ccNumber].parameter.store (param);
    items[(size_t) ccNumber].relative.store (false);
}

void MidiLearn::clearMapping (int ccNumber)
{
    jassert (ccNumber >= 0 && ccNumber < 128);
    items[(size_t) ccNumber].parameter.store (nullptr);
    items[(size_t) ccNumber].relative.store (false);
    saveToSettings();
}

void MidiLearn::clearMapping (gin::Parameter* param)
{
    for (auto& item : items)
    {
        if (item.parameter.load() == param)
        {
            item.parameter.store (nullptr);
            item.relative.store (false);
        }
    }

    saveToSettings();
}

gin::Parameter* MidiLearn::getMapping (int ccNumber) const
{
    jassert (ccNumber >= 0 && ccNumber < 128);
    return items[(size_t) ccNumber].parameter.load();
}

int MidiLearn::getMappedCC (gin::Parameter* param) const
{
    for (int i = 0; i < 128; i++)
        if (items[(size_t) i].parameter.load() == param)
            return i;

    return -1;
}

void MidiLearn::resetLearnTracking()
{
    learnLastVal.fill (-1);
    learnRepeat.fill (0);
    learnSawLow.fill (false);
    learnSawHigh.fill (false);
    learnSawMid.fill (false);
}

void MidiLearn::startLearning (gin::Parameter* param)
{
    learnStartCCValues = currentCCValues;
    resetLearnTracking();
    learnParameter = param;
}

void MidiLearn::cancelLearning()
{
    learnParameter = nullptr;
}

bool MidiLearn::isValidCC (int ccNumber)
{
    // Exclude Bank Select MSB/LSB and Channel Mode Messages (120-127)
    if (ccNumber == 0 || ccNumber == 32 || ccNumber >= 120)
        return false;

    return true;
}

void MidiLearn::processBlock (juce::MidiBuffer& midi, int numSamples)
{
    // Decrement all active countdowns
    for (auto& item : items)
    {
        if (item.active)
        {
            item.countdown -= numSamples;

            if (item.countdown <= 0)
            {
                item.active = false;
                item.countdown = 0;

                if (auto param = item.parameter.load())
                    param->endUserAction();
            }
        }
    }

    // Process incoming MIDI CC messages
    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();

        if (msg.isController())
        {
            const int ccNumber = msg.getControllerNumber();
            const int ccValue = msg.getControllerValue();

            // Track current CC values
            currentCCValues[(size_t) ccNumber] = ccValue;

            // Handle learn mode, auto-detecting absolute vs relative (delta) encoders
            if (auto learning = learnParameter.load(); learning != nullptr && isValidCC (ccNumber))
            {
                const auto cc = (size_t) ccNumber;

                // Classify each value: low/high wrap values look like relative +/- ticks,
                // mid-range values only ever come from an absolute control being swept.
                const bool low  = ccValue >= 1   && ccValue <= 8;
                const bool high = ccValue >= 119 && ccValue <= 127;
                const bool mid  = ccValue != 64 && ! low && ! high;

                if (mid)  learnSawMid[cc]  = true;
                if (low)  learnSawLow[cc]  = true;
                if (high) learnSawHigh[cc] = true;

                learnRepeat[cc] = (ccValue == learnLastVal[cc]) ? learnRepeat[cc] + 1 : 1;
                learnLastVal[cc] = ccValue;

                const int absDelta = std::abs (ccValue - learnStartCCValues[cc]);

                // Absolute: a mid-range value moved past the threshold.
                const bool absoluteHit = learnSawMid[cc] && absDelta >= learnThreshold;
                // Relative: only ever wrap values, either the same tick repeated (steady
                // one-way turn) or both directions seen (a back-and-forth wiggle).
                const bool relativeHit = ! learnSawMid[cc]
                                      && (learnRepeat[cc] >= 3 || (learnSawLow[cc] && learnSawHigh[cc]));

                if (absoluteHit || relativeHit)
                {
                    // Clear any existing mapping for this parameter (without saving yet)
                    for (auto& item : items)
                    {
                        if (item.parameter.load() == learning)
                        {
                            item.parameter.store (nullptr);
                            item.relative.store (false);
                        }
                    }

                    // Set the new mapping
                    items[cc].parameter.store (learning);
                    items[cc].relative.store (relativeHit);
                    learnParameter.store (nullptr);

                    saveToSettings();
                }
            }

            auto& item = items[(size_t) ccNumber];

            if (auto param = item.parameter.load())
            {
                if (item.relative.load())
                {
                    // Two's-complement delta: 1..63 = +, 65..127 = -, 64 = no change.
                    const int delta = ccValue < 64 ? ccValue : ccValue - 128;
                    if (delta != 0)
                    {
                        if (! item.active)
                        {
                            param->beginUserAction();
                            item.active = true;
                        }

                        const float nv = juce::jlimit (0.0f, 1.0f, param->getValue() + delta * relativeStep);
                        param->setValueNotifyingHost (nv);

                        item.countdown = static_cast<int> (timeoutSeconds * sampleRate);
                    }
                }
                else
                {
                    // Begin gesture if not already active
                    if (! item.active)
                    {
                        param->beginUserAction();
                        item.active = true;
                    }

                    // Set the parameter value
                    param->setValueNotifyingHost (ccValue / 127.0f);

                    // Reset countdown to timeout duration
                    item.countdown = static_cast<int> (timeoutSeconds * sampleRate);
                }
            }
        }
    }
}

void MidiLearn::loadState (const juce::ValueTree& vt)
{
    // Clear existing mappings
    for (auto& item : items)
    {
        item.parameter.store (nullptr);
        item.relative.store (false);
    }

    auto ml = vt.getChildWithName ("MIDILEARN");
    if (ml.isValid())
    {
        for (auto c : ml)
        {
            if (! c.hasType ("MAPPING")) continue;

            const int ccNumber = c.getProperty ("cc", -1);
            const juce::String paramUid = c.getProperty ("param").toString();
            const bool relative = (int) c.getProperty ("relative", 0) != 0; // absent = absolute

            if (ccNumber >= 0 && ccNumber < 128 && isValidCC (ccNumber) && paramUid.isNotEmpty())
            {
                if (auto param = processor.getParameter (paramUid))
                {
                    items[(size_t) ccNumber].parameter.store (param);
                    items[(size_t) ccNumber].relative.store (relative);
                }
            }
        }
    }

    saveToSettings();
}

void MidiLearn::saveState (juce::ValueTree& vt)
{
    auto ml = vt.getOrCreateChildWithName ("MIDILEARN", nullptr);
    ml.removeAllChildren (nullptr);

    for (int i = 0; i < 128; i++)
    {
        if (auto param = items[(size_t) i].parameter.load())
        {
            auto c = juce::ValueTree ("MAPPING");
            c.setProperty ("cc", i, nullptr);
            c.setProperty ("param", param->getUid(), nullptr);
            if (items[(size_t) i].relative.load())
                c.setProperty ("relative", 1, nullptr);
            ml.addChild (c, -1, nullptr);
        }
    }
}

void MidiLearn::loadFromSettings()
{
    if (auto props = processor.getSettings())
    {
        auto xmlString = props->getValue ("midiMap");
        if (xmlString.isNotEmpty())
        {
            if (auto xml = juce::XmlDocument::parse (xmlString))
            {
                auto vt = juce::ValueTree::fromXml (*xml);
                if (vt.isValid())
                {
                    // Load without triggering another save
                    for (auto& item : items)
                    {
                        item.parameter.store (nullptr);
                        item.relative.store (false);
                    }

                    auto ml = vt.getChildWithName ("MIDILEARN");
                    if (ml.isValid())
                    {
                        for (auto c : ml)
                        {
                            if (! c.hasType ("MAPPING")) continue;

                            const int ccNumber = c.getProperty ("cc", -1);
                            const juce::String paramUid = c.getProperty ("param").toString();
                            const bool relative = (int) c.getProperty ("relative", 0) != 0; // absent = absolute

                            if (ccNumber >= 0 && ccNumber < 128 && isValidCC (ccNumber) && paramUid.isNotEmpty())
                            {
                                if (auto param = processor.getParameter (paramUid))
                                {
                                    items[(size_t) ccNumber].parameter.store (param);
                                    items[(size_t) ccNumber].relative.store (relative);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void MidiLearn::saveToSettings()
{
    if (auto props = processor.getSettings())
    {
        juce::ValueTree vt ("MIDIMAP");
        saveState (vt);

        if (auto xml = vt.createXml())
            props->setValue ("midiMap", xml->toString());
    }
}
