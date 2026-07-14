#pragma once

//==============================================================================
/**
    MIDI Learn system for mapping MIDI CC messages to plugin parameters.

    MidiLearn provides a way to associate MIDI CC controllers (0-127) with
    plugin parameters. When a CC message is received, it automatically updates
    the corresponding parameter value and handles proper host notification
    with gesture begin/end tracking.

    Key Features:
    - Maps 128 MIDI CC controllers to parameters
    - Automatic host notification for parameter changes
    - Gesture timeout (333ms) for grouping rapid CC changes
    - Auto-detects relative (two's-complement) delta encoders during learn,
      applying increments instead of absolute values. Existing maps without the
      flag load as absolute.
    - State persistence via ValueTree

    Usage:
    @code
    MidiLearn midiLearn(processor);

    // Setup
    midiLearn.setSampleRate(44100.0);
    midiLearn.setMapping(1, modWheelParam);  // CC1 -> mod wheel param

    // In processBlock
    midiLearn.processBlock(midiBuffer, numSamples);

    // Save/load state
    midiLearn.saveState(state);
    midiLearn.loadState(state);
    @endcode

    @see Parameter, Processor
*/
class MidiLearn
{
public:
    MidiLearn (gin::Processor& p);
    ~MidiLearn();

    void setSampleRate (double sr)      { sampleRate = sr; }

    void setMapping (int ccNumber, gin::Parameter* param);
    void clearMapping (int ccNumber);
    void clearMapping (gin::Parameter* param);
    gin::Parameter* getMapping (int ccNumber) const;
    int getMappedCC (gin::Parameter* param) const;

    void startLearning (gin::Parameter* param);
    void cancelLearning();
    bool isLearning() const                         { return learnParameter != nullptr; }
    gin::Parameter* getLearningParameter() const    { return learnParameter; }

    void processBlock (juce::MidiBuffer& midi, int numSamples);

    void loadState (const juce::ValueTree& vt);
    void saveState (juce::ValueTree& vt);

    /** Returns true if the CC number is valid for MIDI learn.
        Excludes CC 0 (Bank Select MSB), CC 32 (Bank Select LSB),
        and CC 120-127 (Channel Mode Messages). */
    static bool isValidCC (int ccNumber);

private:
    struct Item
    {
        std::atomic<gin::Parameter*> parameter { nullptr };
        std::atomic<bool> relative { false }; // two's-complement delta encoder
        bool active = false;
        int countdown = 0;
    };

    gin::Processor& processor;
    std::array<Item, 128> items;
    std::array<int, 128> currentCCValues {};
    std::array<int, 128> learnStartCCValues {};
    std::atomic<gin::Parameter*> learnParameter = nullptr;
    double sampleRate = 44100.0;

    // Per-CC learn-time tracking, used to auto-detect relative (delta) encoders.
    std::array<int, 128>  learnLastVal {};
    std::array<int, 128>  learnRepeat {};
    std::array<bool, 128> learnSawLow {};
    std::array<bool, 128> learnSawHigh {};
    std::array<bool, 128> learnSawMid {};

    static constexpr float timeoutSeconds = 0.333f;
    static constexpr int learnThreshold = 5;

    // Normalized parameter change per relative-encoder tick (~128 ticks full range).
    static constexpr float relativeStep = 1.0f / 128.0f;

    void resetLearnTracking();
    void loadFromSettings();
    void saveToSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiLearn)
};
