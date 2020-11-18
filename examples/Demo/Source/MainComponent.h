/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public juce::Component,
public juce::ListBoxModel
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::ListBox demoList;
    juce::OwnedArray<Component> demoComponents;

private:
    int getNumRows() override   { return demoComponents.size(); }
    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged (int lastRowSelected) override;

    juce::Component::SafePointer<gin::ComponentViewer> componentViewer;
    juce::TextButton toggleComponentViewer {"Component Viewer"};
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
