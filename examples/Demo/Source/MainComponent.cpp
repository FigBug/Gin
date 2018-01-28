/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
struct ImageEffectsDemo : public Component
{
    ImageEffectsDemo()
    {
        setName ("Image Effects");
        addAndMakeVisible (effects);
        effects.addItemList ({"None", "Vignette", "Sepia", "Greyscale", "Soften",
                              "Sharpen", "Gamma", "Invert", "Contrast",
                              "Brightness/Contrast", "Hue/Sat/Light"}, 1);
        effects.setSelectedItemIndex (0);
        effects.onChange = [this]
        {
            repaint();
        };
        
        source = ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
    }
    
    void resized() override
    {
        effects.setBounds (5, 5, 150, 20);
    }

    void paint (Graphics& g) override
    {
        Image img = source.createCopy();
        
        switch (effects.getSelectedItemIndex())
        {
            case 1: img = gin::applyVignette (img, 0.7f, 0.9f, 0.4f); break;
            case 2: img = gin::applySepia (img); break;
            case 3: img = gin::applyGreyScale (img); break;
            case 4: img = gin::applySoften (img); break;
            case 5: img = gin::applySharpen (img); break;
            case 6: img = gin::applyGamma (img, 0.3f); break;
            case 7: img = gin::applyInvert (img); break;
            case 8: img = gin::applyContrast (img, 50); break;
            case 9: img = gin::applyBrightnessContrast (img, 40, 20); break;
            case 10: img = gin::applyHueSaturationLightness (img, 170, 60, 50); break;
        }
        
        g.fillAll (Colours::black);
        g.drawImage (img, getLocalBounds().toFloat(), RectanglePlacement::centred);
    }
    
    Image source;
    ComboBox effects;
};

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
    
    gin::MapViewer map;
};

//==============================================================================
struct SemaphoreDemo : public Component,
                       private Button::Listener
{
    SemaphoreDemo()
    {
        setName ("System Semaphore");
        addAndMakeVisible (lockA);
        addAndMakeVisible (unlockA);
        addAndMakeVisible (lockB);
        addAndMakeVisible (unlockB);

        lockA.addListener (this);
        unlockA.addListener (this);
        lockB.addListener (this);
        unlockB.addListener (this);
    }
    
    void resized() override
    {
        auto rc = getLocalBounds().reduced (8).removeFromLeft (100);
        
        lockA.setBounds (rc.removeFromTop (20));
        rc.removeFromTop (8);
        unlockA.setBounds (rc.removeFromTop (20));
        rc.removeFromTop (8);
        lockB.setBounds (rc.removeFromTop (20));
        rc.removeFromTop (8);
        unlockB.setBounds (rc.removeFromTop (20));
        rc.removeFromTop (8);
    }
    
    void buttonClicked (Button* b) override
    {
        if (b == &lockA)
        {
            gin::callInBackground ([this] ()
                                   {
                                       if (semA.lock())
                                           printf ("Locked\n");
                                   });
        }
        else if (b == &unlockA)
        {
            gin::callInBackground ([this] ()
                                   {
                                       if (semA.unlock())
                                           printf ("Unlocked\n");
                                   });
        }
        else if (b == &lockB)
        {
            gin::callInBackground ([this] ()
                                   {
                                       if (semB.lock())
                                           printf ("Locked\n");
                                   });
        }
        else if (b == &unlockB)
        {
            gin::callInBackground ([this] ()
                                   {
                                       if (semB.unlock())
                                           printf ("Unlocked\n");
                                   });
        }

    }
    
    TextButton lockA {"Lock A"}, unlockA {"Unlock A"}, lockB {"Lock B"}, unlockB {"Unlock B"};
    gin::SystemSemaphore semA {"demo_sem"}, semB {"demo_sem"};
};

//==============================================================================
struct SharedMemoryDemo : public Component,
                          private TextEditor::Listener,
                          private Timer
{
    SharedMemoryDemo()
    {
        setName ("Shared Memory");
        addAndMakeVisible (text);
        text.addListener (this);
        text.setText ("Launch two copies of the app and then type in this box");
        
        startTimerHz (30);
    }
    
    void resized() override
    {
        text.setBounds (getLocalBounds());
    }
    
    void textEditorTextChanged (TextEditor&) override
    {
        strncpy ((char*)mem.getData(), text.getText().toRawUTF8(), size_t (mem.getSize() - 1));
    }
    
    void timerCallback() override
    {
        if (! text.hasKeyboardFocus (true))
        {
            String fromMem ((char*)mem.getData(), size_t (mem.getSize()));
            
            if (fromMem != text.getText())
                text.setText (fromMem);
        }
    }
    
    gin::SharedMemory mem {"demo", 1024};
    TextEditor text;
};

//==============================================================================
struct LeastSquaresDemo : public Component
{
    LeastSquaresDemo()
    {
        setName ("Least Squares");
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
        {
            lsr.clear();
            points.clear();
        }
        else
        {
            lsr.addPoint (e.x, e.y);
            points.add (e.getPosition());
        }
        repaint();
    }
    
    void paint (Graphics& g) override
    {
        if (lsr.enoughPoints())
        {
            g.setColour (Colours::red);
            
            Path p;
            
            double a = lsr.aTerm();
            double b = lsr.bTerm();
            double c = lsr.cTerm();

            for (int x = 0; x < getWidth(); x++)
            {
                double y = a * x * x + b * x + c;
                if (x == 0)
                    p.startNewSubPath (float (x), float (y));
                else
                    p.lineTo (float (x), float (y));
            }
            g.strokePath (p, PathStrokeType (2));
        }
        
        g.setColour (Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);
        
        if (points.isEmpty())
            g.drawText ("Click to add point. Double click to reset.", getLocalBounds(), Justification::centred);
    }
    
    Array<Point<int>> points;
    gin::LeastSquaresRegression lsr;
};

//==============================================================================
struct LinearDemo : public Component
{
    LinearDemo()
    {
        setName ("Linear Regression");
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
        {
            lr.clear();
            points.clear();
        }
        else
        {
            lr.addPoint ({double (e.x), double (e.y)});
            lr.calculate();
            points.add (e.getPosition());
        }
        repaint();
    }
    
    void paint (Graphics& g) override
    {
        if (lr.haveData())
        {
            g.setColour (Colours::red);
            
            Path p;
            
            for (int x = 0; x < getWidth(); x++)
            {
                double y = lr.estimateY (x);
                if (x == 0)
                    p.startNewSubPath (float (x), float (y));
                else
                    p.lineTo (float (x), float (y));
            }
            g.strokePath (p, PathStrokeType (2));
        }
        
        g.setColour (Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);
        
        if (points.isEmpty())
            g.drawText ("Click to add point. Double click to reset.", getLocalBounds(), Justification::centred);
    }
    
    Array<Point<int>> points;
    gin::LinearRegression lr;
};

//==============================================================================
struct SplineDemo : public Component
{
    SplineDemo()
    {
        setName ("Spline");
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
            points.clear();
        else if (points.size() == 0 || e.x > points.getLast().getX())
            points.add (e.getPosition());
        
        repaint();
    }
    
    void paint (Graphics& g) override
    {
        Array<Point<double>> dpoints;
        for (auto p : points)
            dpoints.add ({ double (p.getX()), double (p.getY())});
        
        if (dpoints.size() >= 3)
        {
            gin::Spline spline (dpoints);
            
            g.setColour (Colours::red);
            
            Path p;
            
            p.startNewSubPath (points.getFirst().toFloat());
            for (int x = points.getFirst().getX(); x < points.getLast().getX(); x++)
            {
                double y = spline.interpolate (x);
                p.lineTo (float (x), float (y));
            }
            p.lineTo (points.getLast().toFloat());
            
            g.strokePath (p, PathStrokeType (2));
        }
        
        g.setColour (Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);
        
        if (points.isEmpty())
            g.drawText ("Click from left to right to add points. Double click to reset.", getLocalBounds(), Justification::centred);
    }
    
    Array<Point<int>> points;
};

//==============================================================================
MainContentComponent::MainContentComponent()
{
    demoComponents.add (new ImageEffectsDemo());
    demoComponents.add (new MapDemo());
#if !JUCE_WINDOWS
    demoComponents.add (new SemaphoreDemo());
    demoComponents.add (new SharedMemoryDemo());
#endif
    demoComponents.add (new LeastSquaresDemo());
    demoComponents.add (new LinearDemo());
    demoComponents.add (new SplineDemo());

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
