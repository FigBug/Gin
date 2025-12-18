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

private:
    void updatePlayStopButtons();
    void saveMidiFilePath();
    void restoreMidiFilePath();

    StandaloneFilterWindow&         filterWindow;
    StandaloneAudioProcessorPlayer& player;

    CopperLookAndFeel   lf;

    MidiFileComponent   midi { player.midiPlayer };
    SVGButton           midiPlay;
    SVGButton           midiStop;
    TriggeredScope      scope { player.scopeFifo };
    SpectrumAnalyzer    spectrum { player.spectrumFifo };
};
