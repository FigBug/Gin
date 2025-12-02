#pragma once

/**
    Musical note duration for tempo-synchronized timing.

    NoteDuration represents musical time durations (whole notes, quarter notes, etc.)
    and provides conversion to seconds based on tempo from AudioPlayHead. This is
    useful for tempo-synced delays, LFOs, arpeggiators, and other time-based effects
    that need to follow the host DAW's tempo.

    Key Features:
    - Standard musical note durations (whole, half, quarter, etc.)
    - Dotted and triplet variations
    - Tempo-synchronized conversion to seconds
    - Pre-defined list of common durations
    - Support for bars and beats

    The class stores duration as bars and beats (quarter notes), then converts
    to seconds using the current tempo from the host's transport.

    Usage:
    @code
    // Get standard note durations
    auto durations = NoteDuration::getNoteDurations();
    // Contains: 1/64, 1/32, 1/16, 1/8, 1/4, 1/2, 1 bar, etc.

    // Use with tempo sync
    auto quarterNote = NoteDuration("1/4", 0.0f, 1.0f);
    float seconds = quarterNote.toSeconds(audioPlayhead);
    // At 120 BPM: returns 0.5 seconds

    // For delay time
    auto delayTime = NoteDuration::toSeconds(audioPlayhead, paramValue);
    @endcode

    @see Parameter, StepLFO
*/
class NoteDuration
{
public:
    NoteDuration (const char* name = "", float bars = 0.0f, float note = 0.0f);

    static const std::vector<NoteDuration>& getNoteDurations();
    static float toSeconds (juce::AudioPlayHead* playhead, float value);

    const juce::String& getName() const { return name; }
    float toSeconds (juce::AudioPlayHead* playhead) const;
    float toSeconds (float bpm) const;

    float getBars()     { return bars; }
    float getBeats()    { return note; }

private:    
    juce::String name;
    float bars = 0, note = 0;
};
