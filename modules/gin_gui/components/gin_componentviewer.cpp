/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

static juce::String getClassName (juce::Component* c)
{
    // clang on windows uses msvc name mangling for compatibility
    #if !JUCE_WINDOWS
    int status = 0;
    if (char* demangled = abi::__cxa_demangle (typeid (*c).name(), nullptr, nullptr, &status))
    {
        auto res = juce::String (demangled);
        free (demangled);
        return res;
    }
    return {};
   #else
    juce::String res = typeid (*c).name();
    if (res.startsWith ("class ")) res = res.substring (6);
    if (res.startsWith ("struct ")) res = res.substring (7);
    return res;
   #endif
}

//==============================================================================
class ComponentViewer::Snapshot : public Component
{
public:
    void update (Component* c, juce::Point<int> pos, int zoom_)
    {
        zoom = zoom_;
        if (c != nullptr)
        {
            int w = getWidth()  / zoom + 1;
            int h = getHeight() / zoom + 1;

            auto root = c->getTopLevelComponent();
            auto rootPos = root->getLocalPoint (c, pos);

            if (auto disp = juce::Desktop::getInstance().getDisplays().getDisplayForPoint (c->localPointToGlobal (pos)))
                scale = float (disp->scale);

            image = root->createComponentSnapshot ({rootPos.getX() - w / 2, rootPos.getY() - h / 2, w, h}, false, scale);
            image = image.rescaled (w * zoom, h * zoom, juce::Graphics::lowResamplingQuality);
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

    juce::Colour getColourUnderMouse()
    {
        if (! image.isNull() && zoom > 0)
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
    void paint (juce::Graphics& g) override
    {
        if (! image.isNull() && zoom > 0 && scale != 0.0f)
        {
            g.drawImageAt (image, 0, 0);

            int w = getWidth()  / zoom + 1;
            int h = getHeight() / zoom + 1;

            juce::Rectangle<int> rc (w / 2 * zoom, h / 2 * zoom, int (float ( zoom ) / scale), int ( float ( zoom ) / scale));

            auto c = image.getPixelAt (rc.getX(), rc.getY());
            g.setColour (c.contrasting());
            g.drawRect (rc, 1);
        }
    }

    juce::Image image;
    int zoom = 10;
    float scale = 1.0f;
};

//==============================================================================
class ComponentViewer::ContentComponent : public Component,
                                          private juce::Timer,
                                          private juce::Slider::Listener
{
public:
    ContentComponent (juce::PropertiesFile& settings_)
        : settings (settings_)
    {
        juce::Font f (juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::plain);

        addAndMakeVisible (mouseDetails);
        mouseDetails.setMultiLine (true, false);
        mouseDetails.setFont (f);
        mouseDetails.setReadOnly (true);

        addAndMakeVisible (componentDetails);
        componentDetails.setFont (f);
        componentDetails.setMultiLine (true, false);
        componentDetails.setReadOnly (true);

        addAndMakeVisible (snapshotDetails);
        snapshotDetails.setFont (f);
        snapshotDetails.setReadOnly (true);
        snapshotDetails.setJustification (juce::Justification::centred);

        addAndMakeVisible (zoom);
        zoom.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        zoom.setRange (1, 30, 1);
        zoom.setValue (settings.getIntValue ("ginZoom", 10));
        zoom.addListener (this);

        addAndMakeVisible (snapshot);

        juce::Desktop::getInstance().addGlobalMouseListener (this);
    }

    ~ContentComponent() override
    {
        juce::Desktop::getInstance().removeGlobalMouseListener (this);
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

    void mouseUp (const juce::MouseEvent& )   override { updateComponentDetailsAsync(); }
    void mouseDown (const juce::MouseEvent& ) override { updateComponentDetailsAsync(); }
    void mouseDrag (const juce::MouseEvent& ) override { updateComponentDetailsAsync(); }
    void mouseMove (const juce::MouseEvent& ) override { updateComponentDetailsAsync(); }

    void updateComponentDetailsAsync()
    {
        if (! isTimerRunning())
            startTimer (50);
    }

    void timerCallback() override
    {
        stopTimer();
        updateComponentDetails();
    }

    void sliderValueChanged (juce::Slider*) override
    {
        settings.setValue ("ginZoom", int (zoom.getValue()));
        updateComponentDetails();
    }

    void updateComponentDetails()
    {
        auto mouse = juce::Desktop::getInstance().getMainMouseSource();

        auto pos = mouse.getScreenPosition().toInt();

        juce::StringArray componentHierarchy;
        juce::String cursorPos, colourDetails;

        if (auto c = juce::Desktop::getInstance().getMainMouseSource().getComponentUnderMouse())
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

    juce::StringArray getComponentHierarchy (Component* c)
    {
        juce::StringArray res;

        while (c != nullptr)
        {
            juce::String str;

            str += ("[" + juce::String (getClassName (c)) + "]").paddedRight (' ', 60);
            str += (" \"" + c->getName() + "\"").paddedRight (' ', 20);
            str += (" (" + c->getBounds().toString() + ")").paddedRight (' ', 20);
            str += juce::String (c->isOpaque() ? " Opaque" : "").paddedRight (' ', 8);
            str += juce::String (c->isPaintingUnclipped() ? " Unclipped" : "").paddedRight (' ', 11);

            res.add (str);

            c = c->getParentComponent();
        }
        return res;
    }

    juce::PropertiesFile& settings;

    juce::TextEditor mouseDetails, componentDetails, snapshotDetails;
    juce::Slider zoom;
    Snapshot snapshot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContentComponent)
};

//==============================================================================
ComponentViewer::ComponentViewer (Component* toTrack_, juce::PropertiesFile* settings_, bool alwaysOnTop)
    : DocumentWindow ("Component Viewer Window", juce::Colours::white, allButtons, true),
    toTrack (toTrack_)
{
    if (settings_ != nullptr)
    {
        settings.set (settings_, false);
    }
    else
    {
        juce::PropertiesFile::Options opts;

        opts.applicationName = "Gin";
        opts.filenameSuffix = ".xml";
        opts.folderName = "Gin";
        opts.osxLibrarySubFolder = "Application Support";
        opts.commonToAllUsers = false;
        opts.ignoreCaseOfKeyNames = false;
        opts.doNotSave = false;
        opts.millisecondsBeforeSaving = 1;
        opts.storageFormat = juce::PropertiesFile::storeAsXML;

        settings.set (new juce::PropertiesFile (opts), true);
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

void ComponentViewer::lookAndFeelChanged()
{
    auto& lf = getLookAndFeel ();
    setBackgroundColour (lf.findColour (ResizableWindow::backgroundColourId));
}
