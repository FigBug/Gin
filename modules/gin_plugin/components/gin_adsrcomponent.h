#pragma once

//==============================================================================
/**
    Interactive visual editor for ADSR envelope parameters.

    ADSRComponent provides an intuitive graphical interface for editing ADSR
    (Attack, Decay, Sustain, Release) envelope parameters. Users can drag handles
    to adjust the envelope shape, with real-time visual feedback and optional
    phase indication showing the current envelope position.

    Key Features:
    - Visual envelope curve display
    - Draggable handles for Attack, Decay/Sustain, and Release
    - Real-time parameter updates while dragging
    - Optional phase indicators for showing active voices
    - Mouse cursor changes to indicate draggable areas
    - Bubble tooltips showing parameter values

    The component requires four parameters (attack, decay, sustain, release) and
    optionally accepts a callback to display current envelope phases for visual
    feedback during playback.

    Usage:
    @code
    ADSRComponent adsrEditor;
    adsrEditor.setParams(attackParam, decayParam, sustainParam, releaseParam);
    adsrEditor.setBounds(0, 0, 300, 150);

    // Optional: Show current envelope phases
    adsrEditor.phaseCallback = [this]() {
        std::vector<std::pair<int, float>> phases;
        for (auto* voice : activeVoices)
            phases.push_back(voice->getEnvelopePhase());
        return phases;
    };
    @endcode

    @see Parameter, MultiParamComponent, AnalogADSR
*/
class ADSRComponent : public MultiParamComponent
                    , private juce::Timer
{
public:
    ADSRComponent();
    ~ADSRComponent() override = default;
    
    void setParams (Parameter::Ptr attack, Parameter::Ptr decay,
                    Parameter::Ptr sustain, Parameter::Ptr release);

    std::function<std::vector<std::pair<int,float>>()> phaseCallback;

private:
    void timerCallback() override;
    void paint (juce::Graphics& g) override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    juce::MouseCursor getMouseCursor() override;

    enum class Handle
    {
        none,
        attack,
        decaySustain,
        release,
    };

    juce::String getBubbleText();

    Handle getHandleAt (juce::Point<int> pt);
    juce::Point<int> getHandlePos (Handle);
    juce::Rectangle<int> getHandleRect (Handle);
    juce::Rectangle<int> getArea();

    int paramToX (float t);
    float xToParam (float x);

    Handle handle = Handle::none;
    int dx = 0, dy = 0;
    Parameter::Ptr attack, decay, sustain, release;
    int handleSz = 8;

    std::vector<std::pair<int,float>> curPhases;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRComponent)
};
