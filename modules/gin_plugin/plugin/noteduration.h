#pragma once

class NoteDuration
{
public:
    NoteDuration (const char* name = "", float bars = 0.0f, float note = 0.0f);

    static const std::vector<NoteDuration>& getNoteDurations();
    
    const String& getName() const { return name; }
    float toSeconds (AudioPlayHead* playhead) const;
    float toSeconds (float bpm) const;

private:    
    String name;
    float bars = 0, note = 0;
};
