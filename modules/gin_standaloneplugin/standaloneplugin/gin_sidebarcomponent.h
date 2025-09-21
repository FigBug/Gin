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
};
