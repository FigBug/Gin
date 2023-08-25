#pragma once

/** Draws a wavetable
*/
class WavetableComponent : public juce::Component
{
public:
    WavetableComponent();
    ~WavetableComponent() override;
    
    enum ColourIds
    {
        lineColourId             = 0x3331e10,
        backgroundColourId       = 0x3331e11,
        waveColourId             = 0x3331e12,
        activeWaveColourId       = 0x3331f13,
    };
    
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void setParams (WTOscillator::Params params);
    void setWavetables (juce::OwnedArray<BandLimitedLookupTable>*);

private:
    juce::Path createWavetablePath (float pos);

    juce::OwnedArray<BandLimitedLookupTable>* bllt = nullptr;
    WTOscillator::Params params;
    juce::Array<juce::Path> paths;
    bool needsUpdate = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableComponent)
};
