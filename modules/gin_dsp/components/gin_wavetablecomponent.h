#pragma once

/** Draws a wavetable
*/
class WavetableComponent : public juce::Component,
                           public juce::FileDragAndDropTarget
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
        phaseWaveColourId        = 0x3331f14,
    };

    void showPhase (float start, float len);
    void hidePhase();

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void setParams (WTOscillator::Params params);
    void setWavetables (juce::OwnedArray<BandLimitedLookupTable>*);

    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragEnter (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

    std::function<void (const juce::File&)> onFileDrop;

private:
    juce::Path createWavetablePath (float pos, float s = 0.0f, float e = 1.0f);

    juce::OwnedArray<BandLimitedLookupTable>* bllt = nullptr;
    WTOscillator::Params params;
    juce::Array<juce::Path> paths;
    bool needsUpdate = false;
    bool dragOver = false;

    float phaseStart = -1;
    float phaseLen = -1;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableComponent)
};
