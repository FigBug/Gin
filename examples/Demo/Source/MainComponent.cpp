/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
struct MapDemo : public Component
{
    MapDemo()
    {
        setName ("OpenStreetMaps");
        addAndMakeVisible (map);
    }
    
    void resized() override
    {
        map.setBounds (getLocalBounds());
    }
    
    juceex::MapViewer map;
};

//==============================================================================
MainContentComponent::MainContentComponent()
{
    demoComponents.add (new MapDemo());
    
    for (auto* c : demoComponents)
        addChildComponent (c);
    
    demoList.setModel (this);
    demoList.updateContent();
    demoList.selectRow (0);
    addAndMakeVisible (demoList);
    
    setSize (600, 400);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainContentComponent::resized()
{
    auto rc = getLocalBounds();
    
    demoList.setBounds (rc.removeFromLeft (150));
    
    for (auto* c : demoComponents)
        c->setBounds (rc);
}

void MainContentComponent::paintListBoxItem (int row, Graphics& g, int w, int h, bool rowIsSelected)
{
    Rectangle<int> rc (0, 0, w, h);
    if (rowIsSelected)
    {
        g.setColour (Colours::lightblue);
        g.fillAll();
    }
    
    g.setColour (findColour (ListBox::textColourId));
    g.drawText (demoComponents[row]->getName(), rc.reduced (2), Justification::centredLeft);
}

void MainContentComponent::selectedRowsChanged (int lastRowSelected)
{
    for (int i = 0; i < demoComponents.size(); i++)
        demoComponents[i]->setVisible (i == lastRowSelected);
}
