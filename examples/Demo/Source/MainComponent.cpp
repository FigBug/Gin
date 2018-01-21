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
            juceex::callInBackground ([this] ()
                                      {
                                          if (semA.lock())
                                              printf ("Locked\n");
                                      });
        }
        else if (b == &unlockA)
        {
            juceex::callInBackground ([this] ()
                                      {
                                          if (semA.unlock())
                                              printf ("Unlocked\n");
                                      });
        }
        else if (b == &lockB)
        {
            juceex::callInBackground ([this] ()
                                      {
                                          if (semB.lock())
                                              printf ("Locked\n");
                                      });
        }
        else if (b == &unlockB)
        {
            juceex::callInBackground ([this] ()
                                      {
                                          if (semB.unlock())
                                              printf ("Unlocked\n");
                                      });
        }

    }
    
    TextButton lockA {"Lock A"}, unlockA {"Unlock A"}, lockB {"Lock B"}, unlockB {"Unlock B"};
    juceex::SystemSemaphore semA {"demo_sem"}, semB {"demo_sem"};
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
    
    juceex::SharedMemory mem {"demo", 1024};
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
    juceex::LeastSquaresRegression lsr;
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
    juceex::LinearRegression lr;
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
            juceex::Spline spline (dpoints);
            
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
    demoComponents.add (new MapDemo());
    demoComponents.add (new SemaphoreDemo());
    demoComponents.add (new SharedMemoryDemo());
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
