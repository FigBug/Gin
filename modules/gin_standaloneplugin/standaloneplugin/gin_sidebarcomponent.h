#pragma once

class StandaloneFilterWindow;

//==============================================================================
class SideBarComponent : public juce::Component
{
public:
    SideBarComponent (StandaloneFilterWindow& filterWindow);
    ~SideBarComponent() override;

    void resized() override;

    static constexpr int width = 200;

    StandaloneFilterWindow& filterWindow;
    PluginWrapper&          pluginWrapper;

    CopperLookAndFeel       lf;

    TriggeredScope  scope { pluginWrapper.scopeFifo };
};
