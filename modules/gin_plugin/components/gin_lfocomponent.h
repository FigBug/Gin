#pragma once

//==============================================================================
/**
    Visual display of LFO waveform with animated phase indicators.

    LFOComponent provides a real-time visualization of an LFO's output waveform
    with animated dots showing the current phase position. It displays the
    waveform shape based on all LFO parameters and updates in real-time.

    Key Features:
    - Real-time waveform display for all LFO wave shapes
    - Animated phase indicators (dots) showing current position
    - Supports multiple simultaneous phase indicators (for voices)
    - Depth and offset visualization
    - Optional unclamped mode for values outside -1 to +1
    - Automatic waveform regeneration on parameter changes

    The component works with LFO parameters including wave shape, sync mode,
    rate, beat division, depth, offset, phase, and enable state.

    Usage:
    @code
    LFOComponent lfoViz;
    lfoViz.setParams(waveParam, syncParam, rateParam, beatParam,
                     depthParam, offsetParam, phaseParam, enableParam);
    lfoViz.setBounds(0, 0, 200, 100);

    // Optional: Show current LFO phases for multiple voices
    lfoViz.phaseCallback = [this]() {
        std::vector<float> phases;
        for (auto* voice : activeVoices)
            phases.push_back(voice->getLFOPhase());
        return phases;
    };
    @endcode

    @see Parameter, MultiParamComponent, LFO, StepLFO
*/
class LFOComponent : public MultiParamComponent,
                     private juce::Timer
{
public:
    LFOComponent();
    ~LFOComponent() override = default;
    
    void setUnclamped (bool b) { unclamped = b; }

    void setParams (Parameter::Ptr wave, Parameter::Ptr sync, Parameter::Ptr rate,
                    Parameter::Ptr beat, Parameter::Ptr depth, Parameter::Ptr offset,
                    Parameter::Ptr phase, Parameter::Ptr enable);
    
    std::function<std::vector<float>()> phaseCallback;

private:
    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void paramChanged () override;

    void createPath (juce::Rectangle<int> area);
    float getSample (float phase);
    int getNumSteps();

    Parameter::Ptr wave, sync, rate, beat, depth, offset, phase, enable;

    LFO lfo;
    juce::Path path;
    bool dirty = true;
    std::vector<float> curPhases;
    std::map<int,float> curve;

private:
    bool unclamped = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFOComponent)
};
