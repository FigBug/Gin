/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public Component,
                             public ListBoxModel
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    ListBox demoList;
    OwnedArray<Component> demoComponents;

private:
    int getNumRows() override   { return demoComponents.size(); }
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged (int lastRowSelected) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
