/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
struct DownloadManagerDemo : public Component,
                             private Timer
{
    DownloadManagerDemo()
    {
        setName ("Download Manager");
    }
    
    void resized() override
    {
        repaint();
    }
    
    void visibilityChanged() override
    {
        if (isVisible())
            startTimerHz (10);
        else
            stopTimer();
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
        for (int i = 0; i < 4; i++)
        {
            float w = getWidth()  / 2.0f;
            float h = getHeight() / 2.0f;
            
            Rectangle<float> rc;
            if (i == 0) rc = Rectangle<float> (0, 0, w, h);
            if (i == 1) rc = Rectangle<float> (w, 0, w, h);
            if (i == 2) rc = Rectangle<float> (0, h, w, h);
            if (i == 3) rc = Rectangle<float> (w, h, w, h);

            if (img[i].isValid())
                g.drawImage (img[i], rc, RectanglePlacement::centred, false);
        }
    }
    
    void timerCallback() override
    {
        for (int i = 0; i < 4; i++)
        {
            String url = String::formatted ("https://picsum.photos/%d/%d/?image=%d", getWidth(), getHeight(), Random::getSystemRandom().nextInt (500));
            downloadManager.startAsyncDownload (url, [this, i] (gin::DownloadManager::DownloadResult result)
                                                {
                                                    DBG(result.url.toString (true) + " downloaded " + (result.ok ? "ok: " : "failed: ") + String (result.httpCode));
                                                    
                                                    if (result.ok)
                                                    {
                                                        Image newImg = ImageFileFormat::loadFrom (result.data.getData(), result.data.getSize());
                                                        if (newImg.isValid())
                                                        {
                                                            img[i] = newImg;
                                                            repaint();
                                                        }
                                                    }
                                                });
        }
    }
    
    Image img[4];
    gin::DownloadManager downloadManager;
};

//==============================================================================
struct FileSystemWatcherDemo : public Component,
                               private gin::FileSystemWatcher::Listener
{
    FileSystemWatcherDemo()
    {
        setName ("File System Watcher");
        
        addAndMakeVisible (contents);
        addAndMakeVisible (events);
        
        contents.setMultiLine (true);
        events.setMultiLine (true);
        
        File f = File::getSpecialLocation (File::userDesktopDirectory);
        watcher.addFolder (f);
        watcher.addListener (this);
        
        folderChanged (f);
    }
    
    void resized() override
    {
        auto rc = getLocalBounds();
        contents.setBounds (rc.removeFromTop (rc.getHeight() / 2));
        events.setBounds (rc);
    }
    
    void folderChanged (File f) override
    {
        Array<File> files;
        f.findChildFiles (files, File::findFiles, false);
        files.sort();

        contents.clear();
        
        String txt;
        for (auto ff : files)
            txt += ff.getFileName() + "\n";
        
        contents.setText (txt);
    }

    void fileChanged (File f, gin::FileSystemWatcher::FileSystemEvent fsEvent) override
    {
        auto eventToString = [] (gin::FileSystemWatcher::FileSystemEvent evt) -> String
        {
            switch (evt)
            {
                case gin::FileSystemWatcher::fileCreated: return "Created";
                case gin::FileSystemWatcher::fileUpdated: return "Updated";
                case gin::FileSystemWatcher::fileDeleted: return "Deleted";
                case gin::FileSystemWatcher::fileRenamedOldName: return "Renamed From";
                case gin::FileSystemWatcher::fileRenamedNewName: return "Renamed To";
                default: jassertfalse; return {};
            }
        };

        events.moveCaretToEnd (false);
        events.insertTextAtCaret (f.getFullPathName() + ": " + eventToString (fsEvent) + "\n");

        events.scrollEditorToPositionCaret (0, events.getHeight() - 20);
    }
    
    TextEditor contents, events;
    gin::FileSystemWatcher watcher;
};

//==============================================================================
struct MetadataDemo : public Component
{
    MetadataDemo()
    {
        setName ("Metadata");
        
        MemoryBlock mb (BinaryData::IMG_1883_JPG, BinaryData::IMG_1883_JPGSize);
        MemoryInputStream is (mb, true);
        
        addAndMakeVisible (panel);
        
        OwnedArray<gin::ImageMetadata> metadata;
        if (gin::ImageMetadata::getFromImage (is, metadata))
        {
            for (auto* m : metadata)
            {
                Array<PropertyComponent*> comps;
                StringPairArray values = m->getAllMetadata();
                
                for (String key : values.getAllKeys())
                {
                    Value v = Value (values [key]);
                    TextPropertyComponent* tpc = new TextPropertyComponent (v, key, 1000, false, false);
                    comps.add (tpc);
                }
                
                panel.addSection (m->getType(), comps);
            }
        }
    }
    
    void resized() override
    {
        panel.setBounds (getLocalBounds());
    }
    
    PropertyPanel panel;
};

//==============================================================================
struct BmpImageDemo : public Component
{
    BmpImageDemo()
    {
        setName ("BMP Image");

        gin::BMPImageFormat bmp;

        {
            MemoryBlock mb (BinaryData::ballon_bmp, BinaryData::ballon_bmpSize);
            MemoryInputStream is (mb, false);

            source1 = bmp.decodeImage (is);
        }
        {
            MemoryBlock mb (BinaryData::ballon_8bit_bmp, BinaryData::ballon_8bit_bmpSize);
            MemoryInputStream is (mb, false);

            source2 = bmp.decodeImage (is);
        }
    }

    void paint (Graphics& g) override
    {
        auto rc = getLocalBounds();

        g.fillAll (Colours::black);
        if (source1.isValid())
            g.drawImage (source1, rc.removeFromLeft (rc.getWidth() / 2).toFloat(), RectanglePlacement::centred);
        if (source2.isValid())
            g.drawImage (source2, rc.toFloat(), RectanglePlacement::centred);
    }

    Image source1, source2;
};

//==============================================================================
struct ImageEffectsDemo : public Component,
                          private Slider::Listener
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
            updateVisibility();
        };

        source = ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        source = source.convertedToFormat (Image::ARGB);

        addAndMakeVisible (vignetteAmount);
        addAndMakeVisible (vignetteRadius);
        addAndMakeVisible (vignetteFalloff);
        addAndMakeVisible (gamma);
        addAndMakeVisible (contrast);
        addAndMakeVisible (brightness);
        addAndMakeVisible (hue);
        addAndMakeVisible (saturation);
        addAndMakeVisible (lightness);

        for (int i = 0; i < getNumChildComponents(); i++)
        {
            if (auto* s = dynamic_cast<Slider*> (getChildComponent (i)))
            {
                s->addListener (this);
                s->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
            }
        }

        vignetteAmount.setRange (0.01, 0.99);
        vignetteRadius.setRange (0.01, 0.99);
        vignetteFalloff.setRange (0.01, 0.99);
        gamma.setRange (0, 4);
        contrast.setRange (-100, 100);
        brightness.setRange (-100, 100);
        hue.setRange (-180, 180);
        saturation.setRange (0, 200);
        lightness.setRange (-100, 100);

        vignetteAmount.setValue (0.5);
        vignetteRadius.setValue (0.5);
        vignetteFalloff.setValue (0.5);
        gamma.setValue (1);
        saturation.setValue (100);

        updateVisibility();
    }

    void updateVisibility()
    {
        int idx = effects.getSelectedItemIndex();
        vignetteAmount.setVisible (idx == 1);
        vignetteRadius.setVisible (idx == 1);
        vignetteFalloff.setVisible (idx == 1);
        gamma.setVisible (idx == 6);
        contrast.setVisible (idx == 8 || idx == 9);
        brightness.setVisible (idx == 9);
        hue.setVisible (idx == 10);
        saturation.setVisible (idx == 10);
        lightness.setVisible (idx == 10);

        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        for (int i = 0; i < getNumChildComponents(); i++)
            if (auto* s = dynamic_cast<Slider*> (getChildComponent (i)))
                if (s->isVisible())
                    s->setBounds (rc.removeFromLeft (w));
    }

    void sliderValueChanged (Slider*) override
    {
        repaint();
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
            case 1: img = gin::applyVignette (img, (float) vignetteAmount.getValue(), (float) vignetteRadius.getValue(), (float) vignetteFalloff.getValue()); break;
            case 2: img = gin::applySepia (img); break;
            case 3: img = gin::applyGreyScale (img); break;
            case 4: img = gin::applySoften (img); break;
            case 5: img = gin::applySharpen (img); break;
            case 6: img = gin::applyGamma (img, (float) gamma.getValue()); break;
            case 7: img = gin::applyInvert (img); break;
            case 8: img = gin::applyContrast (img, (float) contrast.getValue()); break;
            case 9: img = gin::applyBrightnessContrast (img, (float) brightness.getValue(), (float) contrast.getValue()); break;
            case 10: img = gin::applyHueSaturationLightness (img, (float) hue.getValue(), (float) saturation.getValue(), (float) lightness.getValue()); break;
        }

        g.fillAll (Colours::black);
        g.drawImage (img, getLocalBounds().toFloat(), RectanglePlacement::centred);
    }

    Image source;
    ComboBox effects;

    Slider vignetteAmount, vignetteRadius, vignetteFalloff, gamma, contrast, brightness, hue, saturation, lightness;
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
    demoComponents.add (new DownloadManagerDemo());
    demoComponents.add (new FileSystemWatcherDemo());
    demoComponents.add (new ImageEffectsDemo());
    demoComponents.add (new MetadataDemo());
    demoComponents.add (new BmpImageDemo());
    demoComponents.add (new MapDemo());
    //demoComponents.add (new SemaphoreDemo());
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
