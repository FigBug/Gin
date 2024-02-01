#pragma once

/** How long is a note?
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
