#pragma once

class StandaloneFilterWindow;

//==============================================================================
class SideBarComponent : public juce::Component
{
public:
    SideBarComponent (StandaloneFilterWindow& filterWindow);
    ~SideBarComponent() override;

    void resized() override;
    void paint (juce::Graphics& g) override;

    static constexpr int width = 200;

    StandaloneFilterWindow&         filterWindow;
    StandaloneAudioProcessorPlayer& player;

    CopperLookAndFeel   lf;

    TriggeredScope      scope { player.scopeFifo };
    SpectrumAnalyzer    spectrum { player.spectrumFifo };
};
