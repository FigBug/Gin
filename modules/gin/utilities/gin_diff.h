#pragma once

class Diff
{
public:
    Diff();
    ~Diff();
    
    juce::String diff (const juce::String& s1, const juce::String& s2);
    juce::String applyPatch (const juce::String& s, const juce::String& patch);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};
