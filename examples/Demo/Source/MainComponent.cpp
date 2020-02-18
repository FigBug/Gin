/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

static ThreadPool pool (SystemStats::getNumCpus());

//==============================================================================
struct AsyncUpdateDemo : public Component,
                         private Thread,
                         private gin::RealtimeAsyncUpdater
{
    AsyncUpdateDemo() : Thread ("AsyncUpdateDemo")
    {
        setName ("AsyncUpdate");
        
        addAndMakeVisible (text);
        text.setReadOnly (true);
        
        startThread();
    }
    
    ~AsyncUpdateDemo() override
    {
        stopThread (1000);
    }
    
    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);
        
        text.setBounds (rc);
    }
    
    void run() override
    {
        while (! threadShouldExit())
        {
            count += 1;
            triggerAsyncUpdate();
        }
    }
    
    void handleAsyncUpdate() override
    {
        text.setText (String (count.get()));
    }
    
    TextEditor text;
    Atomic<int> count;
};

//==============================================================================
struct ValueTreeJsonDemo : public Component,
                           private TextEditor::Listener
{
    ValueTreeJsonDemo()
    {
        setName ("ValueTree Json");
        
        addAndMakeVisible (xmlIn);
        addAndMakeVisible (xmlOut);
        addAndMakeVisible (jsonOut);
        
        xmlIn.setTextToShowWhenEmpty ("Add some ValueTree XML here and hit enter", Colours::white.withAlpha (0.5f));
        jsonOut.setTextToShowWhenEmpty ("JSON will appear here", Colours::white.withAlpha (0.5f));
        xmlOut.setTextToShowWhenEmpty ("And hopeful original xml will reappear here", Colours::white.withAlpha (0.5f));
        
        xmlIn.setMultiLine (true);
        jsonOut.setReadOnly (true);
        jsonOut.setMultiLine (true);
        xmlOut.setMultiLine (true);
        xmlOut.setReadOnly (true);
        
        xmlIn.addListener (this);
    }
    
    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);
        
        int h = rc.getHeight() / 3;
        
        xmlIn.setBounds (rc.removeFromTop (h));
        jsonOut.setBounds (rc.removeFromTop (h));
        xmlOut.setBounds (rc);
    }
    
    void textEditorReturnKeyPressed (TextEditor&) override
    {
        auto vt1 = ValueTree::fromXml (xmlIn.getText());
        
        auto json = gin::valueTreeToJSON (vt1);
        auto vt2 = gin::valueTreeFromJSON (json);
        
        jsonOut.setText (json);
        xmlOut.setText (vt2.toXmlString());
    }
    
    TextEditor xmlIn, xmlOut, jsonOut;
};

//==============================================================================
struct MessagePackDemo : public Component,
                         private TextEditor::Listener
{
    MessagePackDemo()
    {
        setName ("Message Pack");
        
        addAndMakeVisible (jsonIn);
        addAndMakeVisible (raw);
        addAndMakeVisible (jsonOut);
        
        jsonIn.setTextToShowWhenEmpty ("Add some json here and hit enter", Colours::white.withAlpha (0.5f));
        raw.setTextToShowWhenEmpty ("Base64 MessagePack will appear here", Colours::white.withAlpha (0.5f));
        jsonOut.setTextToShowWhenEmpty ("And hopeful original json will reappear here", Colours::white.withAlpha (0.5f));

        jsonIn.setMultiLine (true);
        raw.setReadOnly (true);
        jsonOut.setMultiLine (true);
        jsonOut.setReadOnly (true);
        
        jsonIn.addListener (this);
    }
    
    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);
        
        int h = (rc.getHeight() - 20) / 2;
        
        jsonIn.setBounds (rc.removeFromTop (h));
        jsonOut.setBounds (rc.removeFromBottom (h));
        raw.setBounds (rc);
    }
    
    void textEditorReturnKeyPressed (TextEditor&) override
    {
        var v1 = JSON::parse (jsonIn.getText());
        auto mb = gin::MessagePack::toMessagePack (v1);
        
        raw.setText (Base64::toBase64 (mb.getData(), mb.getSize()), dontSendNotification);
        
        var v2 = gin::MessagePack::parse (mb);
        jsonOut.setText (JSON::toString (v2));
    }
  
    TextEditor jsonIn, raw, jsonOut;
};

//==============================================================================
struct SVGDemo : public Component
{
    SVGDemo()
    {
        setName ("SVG");
    }

	void paint (Graphics& g) override
	{
		auto svg = String (BinaryData::SVG_example_markup_grid_svg, BinaryData::SVG_example_markup_grid_svgSize);
		auto img = gin::rasterizeSVG (svg, getWidth(), getHeight());

		g.drawImageAt (img, 0, 0);
	}
};

//==============================================================================
struct WebsocketDemo : public Component
{
    WebsocketDemo()
    {
        setName ("Websocket");
        
        addAndMakeVisible (inText);
        addAndMakeVisible (outText);
        addAndMakeVisible (sendButton);
        
        outText.setText ("Hello World");
        inText.setMultiLine (true);
        inText.setReadOnly (true);
        
        sendButton.onClick = [this]
        {
            websocket.send (outText.getText());
        };
        
        websocket.onConnect = [this]
        {
            inText.moveCaretToEnd();
            inText.insertTextAtCaret ("Connected\n");
        };
        websocket.onDisconnect = [this]
        {
            inText.moveCaretToEnd();
            inText.insertTextAtCaret ("Disconnected\n");
        };
        websocket.onText = [this] (const String& txt)
        {
            inText.moveCaretToEnd();
            inText.insertTextAtCaret (txt + "\n");
        };
        
        websocket.connect();
    }
    
    void resized() override
    {
        auto r = getLocalBounds().reduced (8);
        
        auto top = r.removeFromTop (20);
        
        sendButton.setBounds (top.removeFromRight (80));
        top.removeFromRight (8);
        outText.setBounds (top);
        
        r.removeFromTop (8);
        inText.setBounds (r);
    }

    TextEditor inText;
    TextEditor outText;
    TextButton sendButton {"Send"};
    
    gin::AsyncWebsocket websocket {URL ("wss://demos.kaazing.com/echo") };
};

//==============================================================================
struct SolidBlendingDemo : public Component,
                           private ComboBox::Listener,
                           private Slider::Listener,
                           private ChangeListener
{
    SolidBlendingDemo()
    {
        setName ("Solid Blending");

        img = ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize);

        modeBox.addItemList (modeNames, 1);
        modeBox.setSelectedItemIndex (0);
        modeBox.addListener (this);
        addAndMakeVisible (modeBox);

        alphaSlider.setRange (0.0, 1.0);
        alphaSlider.setValue (1.0);
        alphaSlider.addListener (this);
        addAndMakeVisible (alphaSlider);

        selector.setCurrentColour (Colour (0xffe3c916));
        selector.addChangeListener (this);
        addAndMakeVisible (selector);
    }

    void sliderValueChanged (Slider*) override                  { repaint(); }
    void comboBoxChanged (ComboBox*) override                   { repaint(); }
    void changeListenerCallback (ChangeBroadcaster*) override   { repaint(); }

    void resized() override
    {
        auto rc = getLocalBounds();

        auto rcBottom = getLocalBounds().removeFromBottom (20);
        alphaSlider.setBounds (rcBottom.removeFromLeft (getWidth() / 3));

        selector.setBounds (rc.removeFromRight (rc.getWidth() / 3).removeFromTop (rc.getHeight() / 3));

        modeBox.setBounds (5, 5, 150, 20);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        auto rc = getLocalBounds();

        auto copy = img.createCopy();

        gin::BlendMode blendMode = (gin::BlendMode) modeBox.getSelectedItemIndex();
        float alpha = float (alphaSlider.getValue());

        auto c = selector.getCurrentColour();
        c = c.withMultipliedAlpha (alpha);

        gin::applyBlend (copy, blendMode, c, &pool);

        g.drawImage (copy, rc.toFloat(), RectanglePlacement::centred);
    }

    Image img;

    StringArray modeNames =
    {
        "Normal", "Lighten", "Darken", "Multiply", "Average", "Add", "Subtract", "Difference", "Negation", "Screen", "Exclusion", "Overlay", "Soft Light", "Hard Light",
        "Color Dodge", "Color Burn", "Linear Dodge", "Linear Burn", "Linear Light", "Vivid Light", "Pin Light", "Hard Mix", "Reflect", "Glow", "Phoenix"
    };

    ComboBox modeBox;
    Slider alphaSlider;
    ColourSelector selector;
};

//==============================================================================
struct BlendingDemo : public Component,
                      private ComboBox::Listener,
                      private Slider::Listener
{
    BlendingDemo()
    {
        setName ("Blending");

        imgA = ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize);
        imgB = ImageFileFormat::loadFrom (BinaryData::mountain_jpg, BinaryData::mountain_jpgSize);


        modeBox.addItemList (modeNames, 1);
        modeBox.setSelectedItemIndex (0);
        modeBox.addListener (this);
        addAndMakeVisible (modeBox);

        alphaSlider.setRange (0.0, 1.0);
        alphaSlider.setValue (1.0);
        alphaSlider.addListener (this);
        addAndMakeVisible (alphaSlider);
    }

    void sliderValueChanged (Slider*) override { repaint(); }
    void comboBoxChanged (ComboBox*) override  { repaint(); }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        alphaSlider.setBounds (rc.removeFromLeft (getWidth() / 3));

        modeBox.setBounds (5, 5, 150, 20);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        auto rc = getLocalBounds();

        auto img = imgB.createCopy();

        gin::BlendMode blendMode = (gin::BlendMode) modeBox.getSelectedItemIndex();
        float alpha = float (alphaSlider.getValue());

        gin::applyBlend (img, imgA, blendMode, alpha, {}, &pool);

        g.drawImage (img, rc.toFloat(), RectanglePlacement::centred);
    }

    Image imgA, imgB;

    StringArray modeNames =
    {
        "Normal", "Lighten", "Darken", "Multiply", "Average", "Add", "Subtract", "Difference", "Negation", "Screen", "Exclusion", "Overlay", "Soft Light", "Hard Light",
        "Color Dodge", "Color Burn", "Linear Dodge", "Linear Burn", "Linear Light", "Vivid Light", "Pin Light", "Hard Mix", "Reflect", "Glow", "Phoenix"
    };


    ComboBox modeBox;
    Slider alphaSlider;
};

//==============================================================================
struct GradientMapDemo : public Component,
                         private ChangeListener
{
    GradientMapDemo()
    {
        setName ("Gradient Map");

        source = ImageFileFormat::loadFrom (BinaryData::Castle_jpg, BinaryData::Castle_jpgSize);

        selector1.setCurrentColour (Colour (0xffe3c916));
        selector2.setCurrentColour (Colour (0xff0c0d01));

        addAndMakeVisible (selector1);
        addAndMakeVisible (selector2);

        selector1.addChangeListener (this);
        selector2.addChangeListener (this);
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds();
        rc = rc.withTrimmedBottom (rc.getHeight() / 2);

        selector1.setBounds (rc.removeFromLeft (rc.getWidth() / 2));
        selector2.setBounds (rc.removeFromLeft (rc.getWidth()));
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        auto rc = getLocalBounds();
        rc = rc.withTrimmedTop (rc.getHeight() / 2);

        auto img = source.createCopy();
        gin::applyGradientMap (img, selector1.getCurrentColour(), selector2.getCurrentColour(), &pool);

        g.drawImage (img, rc.toFloat(), RectanglePlacement::centred);
    }

    Image source;
    ColourSelector selector1, selector2;
};

//==============================================================================
struct ImageResizeDemo : public Component,
                         private Slider::Listener
{
    ImageResizeDemo()
    {
        setName ("Image Resize");

        auto source = ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        source = gin::applyResize (source, 0.97f);

        sourceARGB = source.convertedToFormat (Image::ARGB);
        sourceRGB = source.convertedToFormat (Image::RGB);
        sourceBW = convertToBW (source);

        addAndMakeVisible (zoom);
        zoom.addListener (this);
        zoom.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);

        zoom.setRange (0.1, 4.0);
        zoom.setValue (1.0);
    }

    void sliderValueChanged (Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        zoom.setBounds (rc.removeFromLeft (w));
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        auto zoomed = gin::applyResize (sourceARGB, (float) zoom.getValue());

        g.drawImageAt (zoomed,
                       getWidth() / 2 - zoomed.getWidth() / 2,
                       getHeight() / 2 - zoomed.getHeight() / 2);
    }

    Image convertToBW (const Image& src)
    {
        auto dst = Image (Image::SingleChannel, src.getWidth(), src.getHeight(), true);

        for (int y = 0; y < src.getHeight(); y++)
        {
            for (int x = 0; x < src.getWidth(); x++)
            {
                auto colour = src.getPixelAt (x, y);
                uint8 bw = (colour.getRed() + colour.getGreen() + colour.getBlue()) / 3;
                dst.setPixelAt (x, y, Colour (bw, bw, bw, bw));
            }
        }

        return dst;
    }

    Image sourceARGB, sourceRGB, sourceBW;
    Slider zoom;
};

//==============================================================================
#if defined JUCE_MAC || defined JUCE_WINDOWS
struct ElevatedFileCopyDemo : public Component
{
public:
    ElevatedFileCopyDemo()
    {
        setName ("Elevated File Copy");

        addAndMakeVisible (srcDir);
        addAndMakeVisible (dstDir);
        addAndMakeVisible (copyButton);

       #ifdef JUCE_MAC
        srcDir.setText ("/src");
        dstDir.setText ("/dst");
       #endif
       #ifdef JUCE_WINDOWS
        srcDir.setText ("c:\\src");
        dstDir.setText ("d:\\dst");
       #endif

        copyButton.onClick = [this] { copyFiles(); };
    }

    void copyFiles()
    {
        File src (srcDir.getText());
        File dst (dstDir.getText());

        Array<File> files;
        src.findChildFiles (files, File::findFiles, false);

        gin::ElevatedFileCopy efc;

        for (auto f :files)
            efc.copyFile (f, dst.getChildFile (f.getFileName()));

        efc.execute (true);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        srcDir.setBounds (rc.removeFromTop (25));
        rc.removeFromTop (4);
        dstDir.setBounds (rc.removeFromTop (25));
        rc.removeFromTop (4);
        copyButton.setBounds (rc.removeFromTop (25).removeFromLeft (100));
    }

    TextEditor srcDir, dstDir;
    TextButton copyButton { "Copy" };
};
#endif

//==============================================================================
struct BoxBlurDemo : public Component,
                     private Slider::Listener
{
    BoxBlurDemo()
    {
        setName ("Box Blur Effects");

        auto source = ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        sourceARGB = source.convertedToFormat (Image::ARGB);
        sourceRGB = source.convertedToFormat (Image::RGB);
        sourceBW = convertToBW (source);

        addAndMakeVisible (radius);
        radius.addListener (this);
        radius.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);

        radius.setRange (2, 254);
    }

    void sliderValueChanged (Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        radius.setBounds (rc.removeFromLeft (w));
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
        auto rc = getLocalBounds();
        int w = rc.getWidth() / 3;

        {
            Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc.removeFromLeft (w));

            auto img = sourceARGB.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), RectanglePlacement::centred);
        }

        {
            Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc.removeFromLeft (w));

            auto img = sourceRGB.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), RectanglePlacement::centred);
        }

        {
            Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc);

            auto img = sourceBW.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), RectanglePlacement::centred);
        }
    }

    Image convertToBW (const Image& src)
    {
        auto dst = Image (Image::SingleChannel, src.getWidth(), src.getHeight(), true);

        for (int y = 0; y < src.getHeight(); y++)
        {
            for (int x = 0; x < src.getWidth(); x++)
            {
                auto colour = src.getPixelAt (x, y);
                uint8 bw = (colour.getRed() + colour.getGreen() + colour.getBlue()) / 3;
                dst.setPixelAt (x, y, Colour (bw, bw, bw, bw));
            }
        }

        return dst;
    }

    Image sourceARGB, sourceRGB, sourceBW;
    Slider radius;
};

//==============================================================================
struct DownloadManagerDemo : public Component,
                             private Timer
{
    DownloadManagerDemo()
    {
        setName ("Download Manager");
        downloadManager.setConcurrentDownloadLimit (4);
        downloadManager.setProgressInterval (1);
        downloadManager.enableGzipDeflate (true);
        downloadManager.setQueueFinishedCallback([] {
            DBG("All done!");
        });
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
        downloadImages();
    }

    void downloadImages()
    {
        for (int i = 0; i < 4; i++)
        {
            String url = String::formatted ("https://picsum.photos/id/%d/%d/%d/", Random::getSystemRandom().nextInt (500), getWidth(), getHeight());
            downloadManager.startAsyncDownload (url, [this, i] (gin::DownloadManager::DownloadResult result)
                                                {
                                                    const MessageManagerLock mmLock;

                                                    DBG(result.url.toString (true) + " downloaded " + (result.ok ? "ok: " : "failed: ") + String (result.httpCode));

                                                    if (result.ok)
                                                    {
                                                        Image newImg = ImageFileFormat::loadFrom (result.data.getData(), result.data.getSize());
                                                        if (newImg.isValid())
                                                        {
                                                            img[i % 4] = newImg;
                                                            repaint();
                                                        }
                                                    }
                                                }, [url] (int64 current, int64 total, int64 sinceLast)
                                                {
                                                    double percent = double (current) / double (total) * 100;
                                                    DBG(url + ": " + String (int (percent)) + "% " + String (current) + " of " + String (total) + " downloaded. This block: " + String (sinceLast));
                                                });
        }
    }

    Image img[4];
    gin::DownloadManager downloadManager {3 * 1000, 3 * 1000};
};

//==============================================================================
#if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
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
#endif

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
                              "Brightness/Contrast", "Hue/Sat/Light", "Stack Blur"}, 1);
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
        addAndMakeVisible (radius);

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
        radius.setRange (2, 254);

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
        radius.setVisible (idx == 11);

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
            case 1: gin::applyVignette (img, (float) vignetteAmount.getValue(), (float) vignetteRadius.getValue(), (float) vignetteFalloff.getValue(), &pool); break;
            case 2: gin::applySepia (img, &pool); break;
            case 3: gin::applyGreyScale (img, &pool); break;
            case 4: gin::applySoften (img, &pool); break;
            case 5: gin::applySharpen (img, &pool); break;
            case 6: gin::applyGamma (img, (float) gamma.getValue(), &pool); break;
            case 7: gin::applyInvert (img, &pool); break;
            case 8: gin::applyContrast (img, (float) contrast.getValue(), &pool); break;
            case 9: gin::applyBrightnessContrast (img, (float) brightness.getValue(), (float) contrast.getValue(), &pool); break;
            case 10: gin::applyHueSaturationLightness (img, (float) hue.getValue(), (float) saturation.getValue(), (float) lightness.getValue(), &pool); break;
            case 11: gin::applyStackBlur (img, (int) radius.getValue()); break;
        }

        g.fillAll (Colours::black);
        g.drawImage (img, getLocalBounds().toFloat(), RectanglePlacement::centred);
    }

    Image source;
    ComboBox effects;

    Slider vignetteAmount, vignetteRadius, vignetteFalloff, gamma, contrast, brightness, hue, saturation, lightness, radius;
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
struct LagrangeDemo : public Component
{
    LagrangeDemo()
    {
        setName ("Lagrange");
    }

    void mouseDown (const MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
            points.clear();
        else if (points.size() == 0 || e.x > points.getLast().getX())
            points.add (e.position);

        repaint();
    }

    void paint (Graphics& g) override
    {
        Array<Point<double>> dpoints;
        for (auto p : points)
            dpoints.add ({ double (p.getX()), double (p.getY())});

        if (dpoints.size() >= 2)
        {
            g.setColour (Colours::red);

            Path p;

            p.startNewSubPath (points.getFirst().toFloat());
            for (float x = points.getFirst().getX(); x < points.getLast().getX(); x++)
            {
                float y = gin::Lagrange::interpolate (points, float (x));
                p.lineTo (x, y);
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

    Array<Point<float>> points;
};

//==============================================================================
MainContentComponent::MainContentComponent()
{
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
    demoComponents.add (new ImageEffectsDemo());
    demoComponents.add (new BoxBlurDemo());
   #if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
    demoComponents.add (new FileSystemWatcherDemo());
   #endif
    demoComponents.add (new MetadataDemo());
    demoComponents.add (new BmpImageDemo());
    demoComponents.add (new MapDemo());
    //demoComponents.add (new SemaphoreDemo());
    demoComponents.add (new SharedMemoryDemo());
    demoComponents.add (new LeastSquaresDemo());
    demoComponents.add (new LinearDemo());
    demoComponents.add (new SplineDemo());
    demoComponents.add (new LagrangeDemo());

    for (auto* c : demoComponents)
        addChildComponent (c);

    demoList.setModel (this);
    demoList.updateContent();
    demoList.selectRow (0);
    addAndMakeVisible (demoList);
    addAndMakeVisible (toggleComponentViewer);

    setSize (800, 640);
    
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

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
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
