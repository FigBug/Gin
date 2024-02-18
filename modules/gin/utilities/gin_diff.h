#pragma once

namespace Diff
{
    std::vector<uint8_t> bsDiff (const juce::String& s1, const juce::String& s2);
    juce::String bsApplyPatch (const juce::String& s, const std::vector<uint8_t>& patch);
}
