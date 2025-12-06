/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "Demos.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    demoComponents.add (new LayoutDemo());
    demoComponents.add (new EquationParserDemo());
    demoComponents.add (new ComponentGridDemo());
    demoComponents.add (new EasingDemo());
    demoComponents.add (new BLLTDemo());
    demoComponents.add (new WavetableDemo());
    demoComponents.add (new CatenaryDemo());
    demoComponents.add (new EllipseDemo());
    demoComponents.add (new PerlinNoiseDemo());
    demoComponents.add (new TextRenderDemo());
    demoComponents.add (new AsyncUpdateDemo());
    demoComponents.add (new ValueTreeJsonDemo());
    demoComponents.add (new MessagePackDemo());
    demoComponents.add (new SVGDemo());
    demoComponents.add (new WebsocketDemo());
    demoComponents.add (new SolidBlendingDemo());
    demoComponents.add (new BlendingDemo());
    demoComponents.add (new GradientMapDemo());
    demoComponents.add (new ImageResizeDemo());
   #if defined JUCE_MAC || defined JUCE_WINDOWS
    demoComponents.add (new ElevatedFileCopyDemo());
   #endif
    demoComponents.add (new DownloadManagerDemo());
    demoComponents.add (new DownloadManagerToSaveToFileDemo());
    demoComponents.add (new ImageEffectsDemo());
    demoComponents.add (new BoxBlurDemo());
   #if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
    demoComponents.add (new FileSystemWatcherDemo());
   #endif
    demoComponents.add (new MetadataDemo());
    demoComponents.add (new BmpImageDemo());
    demoComponents.add (new WebpImageDemo());
    demoComponents.add (new MapDemo());
    //demoComponents.add (new SemaphoreDemo());
    demoComponents.add (new SharedMemoryDemo());
    demoComponents.add (new LeastSquaresDemo());
    demoComponents.add (new LinearDemo());
    demoComponents.add (new SplineDemo());
    demoComponents.add (new LagrangeDemo());
    demoComponents.add (new Wireframe3DDemo());
    demoComponents.add (new Wavetable3DDemo());

    std::sort (demoComponents.begin(), demoComponents.end(), [] (juce::Component* a, juce::Component* b)
    {
        return a->getName() < b->getName();
    });

    for (auto* c : demoComponents)
        addChildComponent (c);

    demoList.setModel (this);
    demoList.updateContent();
    demoList.selectRow (0);
    addAndMakeVisible (demoList);
    addAndMakeVisible (toggleComponentViewer);

    setSize (1000, 640);

    toggleComponentViewer.onClick = [this]
    {
        if (componentViewer == nullptr)
            componentViewer = new gin::ComponentViewer (this);
        else
            delete componentViewer.getComponent();
    };
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainContentComponent::resized()
{
    auto rc = getLocalBounds();

    auto col = rc.removeFromLeft (150);
    toggleComponentViewer.setBounds (col.removeFromBottom (25));
    demoList.setBounds (col);

    for (auto* c : demoComponents)
        c->setBounds (rc);
}

void MainContentComponent::paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool rowIsSelected)
{
    juce::Rectangle<int> rc (0, 0, w, h);
    if (rowIsSelected)
    {
        g.setColour (juce::Colours::lightblue);
        g.fillAll();
    }

    g.setColour (findColour (juce::ListBox::textColourId));
    g.drawText (demoComponents[row]->getName(), rc.reduced (2), juce::Justification::centredLeft);
}

void MainContentComponent::selectedRowsChanged (int lastRowSelected)
{
    for (int i = 0; i < demoComponents.size(); i++)
        demoComponents[i]->setVisible (i == lastRowSelected);
}
