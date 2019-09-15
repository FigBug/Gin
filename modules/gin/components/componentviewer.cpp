/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

Component* realGetComponentUnderMouse()
{
    auto mouse = Desktop::getInstance().getMainMouseSource();
    auto pos = mouse.getScreenPosition().toInt();
    
    if (auto c = mouse.getComponentUnderMouse())
        return c;
 
    auto& desktop = Desktop::getInstance();
    
    for (int i = 0; i < desktop.getNumComponents(); i++)
    {
        auto dtc = desktop.getComponent (i);
        
        auto localPos = dtc->getLocalPoint (nullptr, pos);
        
        if (auto c = dtc->getComponentAt (localPos))
            return c;
    }
    
    return {};
}
//==============================================================================
class ComponentViewer::Snapshot : public Component
{
public:
    void update (Component* c, Point<int> pos, int zoom_)
    {
        zoom = zoom_;
        if (c != nullptr)
        {
            int w = getWidth()  / zoom + 1;
            int h = getHeight() / zoom + 1;
            
            auto root = c->getTopLevelComponent();
            auto rootPos = root->getLocalPoint (c, pos);
            
            auto disp = Desktop::getInstance().getDisplays().findDisplayForPoint (c->localPointToGlobal (pos));
            scale = float (disp.scale);
            
            image = root->createComponentSnapshot ({rootPos.getX() - w / 2, rootPos.getY() - h / 2, w, h}, false, scale);
            image = image.rescaled (w * zoom, h * zoom, Graphics::lowResamplingQuality);
            repaint();
        }
        else
        {
            clear();
        }
    }
    
    void clear()
    {
        image = {};
        repaint();
    }
    
    Colour getColourUnderMouse()
    {
        if (! image.isNull())
        {
            int w = getWidth()  / zoom + 1;
            int h = getHeight() / zoom + 1;
            
            int x = w / 2 * zoom;
            int y = h / 2 * zoom;

            return image.getPixelAt (x, y);
        }
        return {};
    }
    
private:
    void paint (Graphics& g) override
    {
        if (! image.isNull())
        {
            g.drawImageAt (image, 0, 0);
            
            int w = getWidth()  / zoom + 1;
            int h = getHeight() / zoom + 1;

            Rectangle<int> rc (w / 2 * zoom, h / 2 * zoom, int (zoom / scale), int (zoom / scale));

            auto c = image.getPixelAt (rc.getX(), rc.getY());
            g.setColour (c.contrasting());
            g.drawRect (rc, 1);
        }
    }

    Image image;
    int zoom = 10;
    float scale = 1.0f;
};

//==============================================================================
class ComponentViewer::ContentComponent : public Component,
                                          private Timer,
                                          private Slider::Listener
{
public:
    ContentComponent (PropertiesFile& settings_)
        : settings (settings_)
    {
        startTimer (100);
        
        Font f (Font::getDefaultMonospacedFontName(), 12.0f, Font::plain);

        addAndMakeVisible (mouseDetails);
        mouseDetails.setMultiLine (true, false);
        mouseDetails.setFont (f);
        mouseDetails.setReadOnly (true);
        mouseDetails.setColour (TextEditor::textColourId, Colours::black);
        mouseDetails.setColour (TextEditor::backgroundColourId, Colours::white);

        addAndMakeVisible (componentDetails);
        componentDetails.setFont (f);
        componentDetails.setMultiLine (true, false);
        componentDetails.setReadOnly (true);
        componentDetails.setColour (TextEditor::textColourId, Colours::black);
        componentDetails.setColour (TextEditor::backgroundColourId, Colours::white);
        
        addAndMakeVisible (snapshotDetails);
        snapshotDetails.setFont (f);
        snapshotDetails.setReadOnly (true);
        snapshotDetails.setColour (TextEditor::textColourId, Colours::black);
        snapshotDetails.setColour (TextEditor::backgroundColourId, Colours::white);
        snapshotDetails.setJustification (Justification::centred);
        
        addAndMakeVisible (zoom);
        zoom.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        zoom.setRange (1, 30, 1);
        zoom.setValue (settings.getIntValue ("ginZoom", 10));
        zoom.addListener (this);

        addAndMakeVisible (snapshot);
    }

    void resized() override
    {
        auto rc = getLocalBounds();

        mouseDetails.setBounds (rc.removeFromTop (50));
        componentDetails.setBounds (rc.removeFromTop (int (rc.getHeight() * 0.4)));
        
        auto row = rc.removeFromTop (25);
        int w = row.getWidth() / 3;
        zoom.setBounds (row.removeFromLeft (w));
        snapshotDetails.setBounds (row.removeFromLeft (w));
        
        snapshot.setBounds (rc);
    }

    void timerCallback() override
    {
        updateComponentDetails();
    }
    
    void sliderValueChanged (Slider*) override
    {
        settings.setValue ("ginZoom", int (zoom.getValue()));
        updateComponentDetails();
    }
    
    void updateComponentDetails()
    {
        auto mouse = Desktop::getInstance().getMainMouseSource();

        auto pos = mouse.getScreenPosition().toInt();

        if (pos == lastPos)
            return;

        lastPos = pos;

        StringArray componentHierarchy;
        String cursorPos, colourDetails;
        
        if (auto c = realGetComponentUnderMouse())
        {
            snapshot.update (c, c->getLocalPoint (nullptr, pos), int (zoom.getValue()));
            componentHierarchy = getComponentHierarchy (c);

            cursorPos += "Component: (" + c->getLocalPoint (nullptr, pos).toString() + ")\n";
            cursorPos += "Window:    (" + c->getTopLevelComponent()->getLocalPoint (nullptr, pos).toString() + ")\n";
            
            auto col = snapshot.getColourUnderMouse();
            colourDetails = col.toDisplayString (true);
        }
        else
        {
            snapshot.clear();
        }
        
        cursorPos += "Screen:    (" + pos.toString() + ")";
        
        mouseDetails.setText (cursorPos);
        snapshotDetails.setText (colourDetails);
        componentDetails.setText (componentHierarchy.joinIntoString ("\n"));
    }

    StringArray getComponentHierarchy (Component* c)
    {
        StringArray res;

        while (c != nullptr)
        {
            String str;
            
            str += ("[" + String (typeid (*c).name()) + "]").paddedRight (' ', 50);
            str += (" \"" + c->getName() + "\"").paddedRight (' ', 20);
            str += (" (" + c->getBounds().toString() + ")").paddedRight (' ', 20);
            str += String (c->isOpaque() ? " Opaque" : "").paddedRight (' ', 8);

            res.add (str);

            c = c->getParentComponent();
        }
        return res;
    }
    
    PropertiesFile& settings;

    Point<int> lastPos;

    TextEditor mouseDetails, componentDetails, snapshotDetails;
    Slider zoom;
    Snapshot snapshot;
};

//==============================================================================
ComponentViewer::ComponentViewer (Component* toTrack_, PropertiesFile* settings_, bool alwaysOnTop)
    : DocumentWindow ("Component Viewer Window", Colours::white, allButtons, true),
    toTrack (toTrack_)
{
    if (settings_ != nullptr)
    {
        settings.set (settings_, false);
    }
    else
    {
        PropertiesFile::Options opts;
        
        opts.applicationName = "Gin";
        opts.filenameSuffix = ".xml";
        opts.folderName = "Gin";
        opts.osxLibrarySubFolder = "Application Support";
        opts.commonToAllUsers = false;
        opts.ignoreCaseOfKeyNames = false;
        opts.doNotSave = false;
        opts.millisecondsBeforeSaving = 1;
        opts.storageFormat = juce::PropertiesFile::storeAsXML;

        settings.set (new PropertiesFile (opts), true);
    }
    
    if (toTrack != nullptr)
        toTrack->addComponentListener (this);

    auto position = settings->getValue ("ginComponentViewerPosition", "");

    if (position.isNotEmpty())
        restoreWindowStateFromString (position);
    else
        centreWithSize (640, 480);

    setVisible (true);
    setAlwaysOnTop (alwaysOnTop);
    setResizable (true, false);

    setContentOwned (new ContentComponent (*settings), false);
    
    onClose = [this] { delete this; };
}

ComponentViewer::~ComponentViewer()
{
    if (toTrack != nullptr)
        toTrack->removeComponentListener (this);
    
    saveWindowPosition();
}

void ComponentViewer::closeButtonPressed()
{
    if (onClose)
        onClose();
}

void ComponentViewer::moved()
{
    DocumentWindow::resized();
    saveWindowPosition();
}

void ComponentViewer::resized()
{
    DocumentWindow::resized();
    saveWindowPosition();
}

void ComponentViewer::saveWindowPosition()
{
    if (settings != nullptr)
    {
        settings->setValue ("ginComponentViewerPosition", getWindowStateAsString());
        settings->saveIfNeeded();
    }
}

void ComponentViewer::componentBeingDeleted (Component&)
{
    settings.reset();
    if (toTrack != nullptr)
        toTrack = nullptr;

    delete this;
}
