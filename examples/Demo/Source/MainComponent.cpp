/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

static juce::ThreadPool pool (juce::SystemStats::getNumCpus());

//==============================================================================
struct PerlinNoiseDemo : public juce::Component
{
public:
    PerlinNoiseDemo()
    {
        setName ("Perlin Noise");

        gin::PerlinNoise<float> n (5);

        for (int x = 0; x < 500; x++)
            for (int y = 0; y < 500; y++)
                img.setPixelAt (x, y, juce::Colour::greyLevel (n.noise (x / 12.0f, y / 12.0f)));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();

        g.drawImage (img, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image img {juce::Image::ARGB, 500, 500, true};
};

//==============================================================================
struct TextRenderDemo : public juce::Component
{
    TextRenderDemo()
    {
        setName ("Wave to Text Render");

        addAndMakeVisible (text);
        text.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::plain));
        text.setReadOnly (true);
        text.setMultiLine (true, false);

        juce::AudioSampleBuffer buffer (2, 512);

        gin::BandLimitedLookupTables bllt;
        gin::StereoOscillator osc (bllt);

        gin::StereoOscillator::Params p;
        p.wave = gin::Wave::sine;

        osc.noteOn();
        osc.process (69, p, buffer);

        auto txt = gin::TextRenderer::renderBuffer (buffer, 7, 4);
        text.setText (txt, juce::dontSendNotification);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        text.setBounds (rc);
    }

    juce::TextEditor text;
};

//==============================================================================
struct AsyncUpdateDemo : public juce::Component,
                         private juce::Thread,
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
        text.setText (juce::String (count.get()));
    }

    juce::TextEditor text;
    juce::Atomic<int> count;
};

//==============================================================================
struct ValueTreeJsonDemo : public juce::Component,
                           private juce::TextEditor::Listener
{
    ValueTreeJsonDemo()
    {
        setName ("ValueTree Json");

        addAndMakeVisible (xmlIn);
        addAndMakeVisible (xmlOut);
        addAndMakeVisible (jsonOut);

        xmlIn.setTextToShowWhenEmpty ("Add some ValueTree XML here and hit enter", juce::Colours::white.withAlpha (0.5f));
        jsonOut.setTextToShowWhenEmpty ("JSON will appear here", juce::Colours::white.withAlpha (0.5f));
        xmlOut.setTextToShowWhenEmpty ("And hopeful original xml will reappear here", juce::Colours::white.withAlpha (0.5f));

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

    void textEditorReturnKeyPressed (juce::TextEditor&) override
    {
        auto vt1 = juce::ValueTree::fromXml (xmlIn.getText());

        auto json = gin::valueTreeToJSON (vt1);
        auto vt2 = gin::valueTreeFromJSON (json);

        jsonOut.setText (json);
        xmlOut.setText (vt2.toXmlString());
    }

    juce::TextEditor xmlIn, xmlOut, jsonOut;
};

//==============================================================================
struct MessagePackDemo : public juce::Component,
                         private juce::TextEditor::Listener
{
    MessagePackDemo()
    {
        setName ("Message Pack");

        addAndMakeVisible (jsonIn);
        addAndMakeVisible (raw);
        addAndMakeVisible (jsonOut);

        jsonIn.setTextToShowWhenEmpty ("Add some json here and hit enter", juce::Colours::white.withAlpha (0.5f));
        raw.setTextToShowWhenEmpty ("Base64 MessagePack will appear here", juce::Colours::white.withAlpha (0.5f));
        jsonOut.setTextToShowWhenEmpty ("And hopeful original json will reappear here", juce::Colours::white.withAlpha (0.5f));

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

    void textEditorReturnKeyPressed (juce::TextEditor&) override
    {
        juce::var v1 = juce::JSON::parse (jsonIn.getText());
        auto mb = gin::MessagePack::toMessagePack (v1);

        raw.setText (juce::Base64::toBase64 (mb.getData(), mb.getSize()), juce::dontSendNotification);

        juce::var v2 = gin::MessagePack::parse (mb);
        jsonOut.setText (juce::JSON::toString (v2));
    }

    juce::TextEditor jsonIn, raw, jsonOut;
};

//==============================================================================
struct SVGDemo : public juce::Component
{
    SVGDemo()
    {
        setName ("SVG");
    }

    void paint (juce::Graphics& g) override
    {
        auto svg = juce::String (BinaryData::SVG_example_markup_grid_svg, BinaryData::SVG_example_markup_grid_svgSize);
        auto img = gin::rasterizeSVG (svg, getWidth(), getHeight());

        g.drawImageAt (img, 0, 0);
    }
};

//==============================================================================
struct WebsocketDemo : public juce::Component
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
        websocket.onText = [this] (const juce::String& txt)
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

    juce::TextEditor inText;
    juce::TextEditor outText;
    juce::TextButton sendButton {"Send"};

    gin::AsyncWebsocket websocket {juce::URL ("wss://ws.postman-echo.com/raw") };
};

//==============================================================================
struct SolidBlendingDemo : public juce::Component,
                           private juce::ComboBox::Listener,
                           private juce::Slider::Listener,
                           private juce::ChangeListener
{
    SolidBlendingDemo()
    {
        setName ("Solid Blending");

        img = juce::ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize);

        modeBox.addItemList (modeNames, 1);
        modeBox.setSelectedItemIndex (0);
        modeBox.addListener (this);
        addAndMakeVisible (modeBox);

        alphaSlider.setRange (0.0, 1.0);
        alphaSlider.setValue (1.0);
        alphaSlider.addListener (this);
        addAndMakeVisible (alphaSlider);

        selector.setCurrentColour (juce::Colour (0xffe3c916));
        selector.addChangeListener (this);
        addAndMakeVisible (selector);
    }

    void sliderValueChanged (juce::Slider*) override                  { repaint(); }
    void comboBoxChanged (juce::ComboBox*) override                   { repaint(); }
    void changeListenerCallback (juce::ChangeBroadcaster*) override   { repaint(); }

    void resized() override
    {
        auto rc = getLocalBounds();

        auto rcBottom = getLocalBounds().removeFromBottom (20);
        alphaSlider.setBounds (rcBottom.removeFromLeft (getWidth() / 3));

        selector.setBounds (rc.removeFromRight (rc.getWidth() / 3).removeFromTop (rc.getHeight() / 3));

        modeBox.setBounds (5, 5, 150, 20);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();

        auto copy = img.createCopy();

        gin::BlendMode blendMode = (gin::BlendMode) modeBox.getSelectedItemIndex();
        float alpha = float (alphaSlider.getValue());

        auto c = selector.getCurrentColour();
        c = c.withMultipliedAlpha (alpha);

        gin::applyBlend (copy, blendMode, c, &pool);

        g.drawImage (copy, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image img;

    juce::StringArray modeNames =
    {
        "Normal", "Lighten", "Darken", "Multiply", "Average", "Add", "Subtract", "Difference", "Negation", "Screen", "Exclusion", "Overlay", "Soft Light", "Hard Light",
        "Color Dodge", "Color Burn", "Linear Dodge", "Linear Burn", "Linear Light", "Vivid Light", "Pin Light", "Hard Mix", "Reflect", "Glow", "Phoenix"
    };

    juce::ComboBox modeBox;
    juce::Slider alphaSlider;
    juce::ColourSelector selector;
};

//==============================================================================
struct BlendingDemo : public juce::Component,
                      private juce::ComboBox::Listener,
                      private juce::Slider::Listener
{
    BlendingDemo()
    {
        setName ("Blending");

        imgA = juce::ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize);
        imgB = juce::ImageFileFormat::loadFrom (BinaryData::mountain_jpg, BinaryData::mountain_jpgSize);


        modeBox.addItemList (modeNames, 1);
        modeBox.setSelectedItemIndex (0);
        modeBox.addListener (this);
        addAndMakeVisible (modeBox);

        alphaSlider.setRange (0.0, 1.0);
        alphaSlider.setValue (1.0);
        alphaSlider.addListener (this);
        addAndMakeVisible (alphaSlider);
    }

    void sliderValueChanged (juce::Slider*) override { repaint(); }
    void comboBoxChanged (juce::ComboBox*) override  { repaint(); }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        alphaSlider.setBounds (rc.removeFromLeft (getWidth() / 3));

        modeBox.setBounds (5, 5, 150, 20);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();

        auto img = imgB.createCopy();

        gin::BlendMode blendMode = (gin::BlendMode) modeBox.getSelectedItemIndex();
        float alpha = float (alphaSlider.getValue());

        gin::applyBlend (img, imgA, blendMode, alpha, {}, &pool);

        g.drawImage (img, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image imgA, imgB;

    juce::StringArray modeNames =
    {
        "Normal", "Lighten", "Darken", "Multiply", "Average", "Add", "Subtract", "Difference", "Negation", "Screen", "Exclusion", "Overlay", "Soft Light", "Hard Light",
        "Color Dodge", "Color Burn", "Linear Dodge", "Linear Burn", "Linear Light", "Vivid Light", "Pin Light", "Hard Mix", "Reflect", "Glow", "Phoenix"
    };


    juce::ComboBox modeBox;
    juce::Slider alphaSlider;
};

//==============================================================================
struct GradientMapDemo : public juce::Component,
                         private juce::ChangeListener
{
    GradientMapDemo()
    {
        setName ("Gradient Map");

        source = juce::ImageFileFormat::loadFrom (BinaryData::Castle_jpg, BinaryData::Castle_jpgSize);

        selector1.setCurrentColour (juce::Colour (0xffe3c916));
        selector2.setCurrentColour (juce::Colour (0xff0c0d01));

        addAndMakeVisible (selector1);
        addAndMakeVisible (selector2);

        selector1.addChangeListener (this);
        selector2.addChangeListener (this);
    }

    void changeListenerCallback (juce::ChangeBroadcaster*) override
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

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();
        rc = rc.withTrimmedTop (rc.getHeight() / 2);

        auto img = source.createCopy();
        gin::applyGradientMap (img, selector1.getCurrentColour(), selector2.getCurrentColour(), &pool);

        g.drawImage (img, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source;
    juce::ColourSelector selector1, selector2;
};

//==============================================================================
struct ImageResizeDemo : public juce::Component,
                         private juce::Slider::Listener
{
    ImageResizeDemo()
    {
        setName ("Image Resize");

        auto source = juce::ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);

        source = gin::applyResize (source, 0.97f, gin::ResizeAlgorirm::avir);

        sourceARGB = source.convertedToFormat (juce::Image::ARGB);
        sourceRGB = source.convertedToFormat (juce::Image::RGB);
        sourceBW = convertToBW (source);

        addAndMakeVisible (zoom);
        zoom.addListener (this);
        zoom.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        zoom.setRange (0.1, 4.0);
        zoom.setValue (1.0);
    }

    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        zoom.setBounds (rc.removeFromLeft (w));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto t1 = juce::Time::getMillisecondCounterHiRes();
        auto zoomed1 = gin::applyResize (sourceARGB, (float) zoom.getValue(), gin::ResizeAlgorirm::avir);
        auto t2 = juce::Time::getMillisecondCounterHiRes();
        auto zoomed2 = gin::applyResize (sourceARGB, (float) zoom.getValue(), gin::ResizeAlgorirm::lanczos);
        auto t3 = juce::Time::getMillisecondCounterHiRes();

        printf ("ImageResizeDemo: AVIR:     %.1f ms\n", t2 - t1);
        printf ("ImageResizeDemo: Lanczos:  %.1f ms\n", t3 - t2);
        printf ("ImageResizeDemo: Ratio:    %.1f x\n", (t2 - t1) / (t3 - t2));

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (getLocalBounds().removeFromLeft (getWidth() / 2 + 1));
            g.drawImageAt (zoomed1, getWidth() / 2 - zoomed1.getWidth() / 2, getHeight() / 2 - zoomed1.getHeight() / 2);
        }

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (getLocalBounds().removeFromRight (getWidth() / 2 + 1));
            g.drawImageAt (zoomed2, getWidth() / 2 - zoomed2.getWidth() / 2, getHeight() / 2 - zoomed2.getHeight() / 2);
        }
    }

    juce::Image convertToBW (const juce::Image& src)
    {
        auto dst = juce::Image (juce::Image::SingleChannel, src.getWidth(), src.getHeight(), true);

        for (int y = 0; y < src.getHeight(); y++)
        {
            for (int x = 0; x < src.getWidth(); x++)
            {
                auto colour = src.getPixelAt (x, y);
                uint8_t bw = (colour.getRed() + colour.getGreen() + colour.getBlue()) / 3;
                dst.setPixelAt (x, y, juce::Colour (bw, bw, bw, bw));
            }
        }

        return dst;
    }

    juce::Image sourceARGB, sourceRGB, sourceBW;
    juce::Slider zoom;
};

//==============================================================================
#if defined JUCE_MAC || defined JUCE_WINDOWS
struct ElevatedFileCopyDemo : public juce::Component
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
        juce::File src (srcDir.getText());
        juce::File dst (dstDir.getText());

        juce::Array<juce::File> files;
        src.findChildFiles (files, juce::File::findFiles, false);

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

    juce::TextEditor srcDir, dstDir;
    juce::TextButton copyButton { "Copy" };
};
#endif

//==============================================================================
struct BoxBlurDemo : public juce::Component,
                     private juce::Slider::Listener
{
    BoxBlurDemo()
    {
        setName ("Box Blur Effects");

        auto source = juce::ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        sourceARGB = source.convertedToFormat (juce::Image::ARGB);
        sourceRGB = source.convertedToFormat (juce::Image::RGB);
        sourceBW = convertToBW (source);

        addAndMakeVisible (radius);
        radius.addListener (this);
        radius.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        radius.setRange (2, 254);
    }

    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        radius.setBounds (rc.removeFromLeft (w));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        auto rc = getLocalBounds();
        int w = rc.getWidth() / 3;

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc.removeFromLeft (w));

            auto img = sourceARGB.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc.removeFromLeft (w));

            auto img = sourceRGB.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc);

            auto img = sourceBW.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }
    }

    juce::Image convertToBW (const juce::Image& src)
    {
        auto dst = juce::Image (juce::Image::SingleChannel, src.getWidth(), src.getHeight(), true);

        for (int y = 0; y < src.getHeight(); y++)
        {
            for (int x = 0; x < src.getWidth(); x++)
            {
                auto colour = src.getPixelAt (x, y);
                uint8_t bw = (colour.getRed() + colour.getGreen() + colour.getBlue()) / 3;
                dst.setPixelAt (x, y, juce::Colour (bw, bw, bw, bw));
            }
        }

        return dst;
    }

    juce::Image sourceARGB, sourceRGB, sourceBW;
    juce::Slider radius;
};

//==============================================================================
struct DownloadManagerDemo : public juce::Component,
                             private juce::Timer
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

        gin::asyncDownload (juce::URL ("https://rabien.com"), [] (const juce::String& t)
        {
            juce::ignoreUnused (t);
            DBG(t);
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

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        for (int i = 0; i < 4; i++)
        {
            float w = getWidth()  / 2.0f;
            float h = getHeight() / 2.0f;

            juce::Rectangle<float> rc;
            if (i == 0) rc = juce::Rectangle<float> (0, 0, w, h);
            if (i == 1) rc = juce::Rectangle<float> (w, 0, w, h);
            if (i == 2) rc = juce::Rectangle<float> (0, h, w, h);
            if (i == 3) rc = juce::Rectangle<float> (w, h, w, h);

            if (img[i].isValid())
                g.drawImage (img[i], rc, juce::RectanglePlacement::centred, false);
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
            juce::String url = juce::String::formatted ("https://picsum.photos/id/%d/%d/%d/", juce::Random::getSystemRandom().nextInt (500), getWidth(), getHeight());
            downloadManager.startAsyncDownload (url, [this, i] (gin::DownloadManager::DownloadResult result)
                                                {
                                                    const juce::MessageManagerLock mmLock;

                                                    DBG(result.url.toString (true) + " downloaded " + (result.ok ? "ok: " : "failed: ") + juce::String (result.httpCode));

                                                    if (result.ok)
                                                    {
                                                        juce::Image newImg = juce::ImageFileFormat::loadFrom (result.data.getData(), result.data.getSize());
                                                        if (newImg.isValid())
                                                        {
                                                            img[i % 4] = newImg;
                                                            repaint();
                                                        }
                                                    }
                                                }, [url] (int64_t current, int64_t total, [[ maybe_unused ]] int64_t sinceLast)
                                                {
                                                    [[ maybe_unused ]] double percent = double (current) / double (total) * 100;
                                                    DBG(url + ": " + juce::String (int (percent)) + "% " + juce::String (current) + " of " + juce::String (total) + " downloaded. This block: " + juce::String (sinceLast));
                                                });
        }
    }

    juce::Image img[4];
    gin::DownloadManager downloadManager {3 * 1000, 3 * 1000};
};

//==============================================================================
struct DownloadManagerToSaveToFileDemo : public juce::Component,
                                         private juce::Button::Listener
{
    DownloadManagerToSaveToFileDemo():
        downloadButton ("Choose File"),
        openButton ("Open File"),
        fileChooser ("Download random picture to...", juce::File(), "*.jpg")
    {
        setName ("Download and Save");
        downloadManager.setConcurrentDownloadLimit (1);
        downloadManager.setProgressInterval (1);
        downloadManager.enableGzipDeflate (true);
        downloadManager.setQueueFinishedCallback([] {
            DBG("All done!");
        });
        downloadButton.addListener (this);
        addAndMakeVisible (downloadButton );
        openButton.addListener (this);
        openButton.setEnabled (false);
        addAndMakeVisible (openButton);
        lastResult.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (lastResult);
    }

    void resized() override
    {
        int itemWidth = getWidth() - 20;
        static constexpr int itemHeight = 30;
        auto centre = getLocalBounds().getCentre();
        lastResult.setBounds (centre.getX() - itemWidth / 2, centre.getY() - itemHeight / 2, itemWidth, itemHeight);
        downloadButton.setBounds (lastResult.getBounds().withY (lastResult.getY() - itemHeight));
        openButton.setBounds (lastResult.getBounds().withY (lastResult.getBottom()));
    }

    void buttonClicked (juce::Button* b) override
    {
        if (b == &downloadButton)
        {
            fileChooser.launchAsync (juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::saveMode, [&] (const juce::FileChooser&)
            {
                file = fileChooser.getResult();
                openButton.setEnabled (false);

                if (file != juce::File())
                {
                    juce::String url = juce::String::formatted ("https://picsum.photos/id/%d/%d/%d/", juce::Random::getSystemRandom().nextInt (500), getWidth(), getHeight());

                    auto progressCb = [&] (juce::int64 current, juce::int64 total, juce::int64)
                    {
                        updateText (juce::String::formatted ("%d%", int (double (current) / double (total) * 100.0)));
                    };

                    auto completionCb = [&]( gin::DownloadManager::DownloadResult result )
                    {
                        if (!result.ok)
                            updateText ("Error: download failed!", true);
                        else if (result.saveToFile (file))
                            updateText ("Success: picture saved to " + file.getFullPathName());
                        else
                            updateText ("Error: could not save to " + file.getFullPathName());
                        downloadButton.setEnabled (true);
                        openButton.setEnabled (result.ok && file.existsAsFile());
                    };

                    downloadButton.setEnabled (false);
                    downloadManager.startAsyncDownload (url, completionCb, progressCb);
                }
                else
                {
                    updateText ("");
                }
            });
        }
        else if (b == &openButton)
        {
            if (file.existsAsFile())
                file.startAsProcess();
            else
                openButton.setEnabled (false);
        }
    }

    void updateText (juce::String msg, bool isError = false )
    {
        lastResult.setColour (juce::Label::ColourIds::textColourId, isError ? juce::Colours::red : juce::Colours::green);
        lastResult.setText (msg, juce::dontSendNotification);
    }

    juce::TextButton downloadButton;
    juce::TextButton openButton;
    juce::Label lastResult;
    juce::FileChooser fileChooser;
    juce::File file;
    gin::DownloadManager downloadManager {3 * 1000, 3 * 1000};
};

//==============================================================================
#if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
struct FileSystemWatcherDemo : public juce::Component,
                               private gin::FileSystemWatcher::Listener
{
    FileSystemWatcherDemo()
    {
        setName ("File System Watcher");

        addAndMakeVisible (contents);
        addAndMakeVisible (events);

        contents.setMultiLine (true);
        events.setMultiLine (true);

        juce::File f = juce::File::getSpecialLocation (juce::File::userDesktopDirectory);
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

    void folderChanged (juce::File f) override
    {
        juce::Array<juce::File> files;
        f.findChildFiles (files, juce::File::findFiles, false);
        files.sort();

        contents.clear();

        juce::String txt;
        for (auto ff : files)
            txt += ff.getFileName() + "\n";

        contents.setText (txt);
    }

    void fileChanged (juce::File f, gin::FileSystemWatcher::FileSystemEvent fsEvent) override
    {
        auto eventToString = [] (gin::FileSystemWatcher::FileSystemEvent evt) -> juce::String
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

    juce::TextEditor contents, events;
    gin::FileSystemWatcher watcher;
};
#endif

//==============================================================================
struct MetadataDemo : public juce::Component
{
    MetadataDemo()
    {
        setName ("Metadata");

        juce::MemoryBlock mb (BinaryData::IMG_1883_JPG, BinaryData::IMG_1883_JPGSize);
        juce::MemoryInputStream is (mb, true);

        addAndMakeVisible (panel);

        juce::OwnedArray<gin::ImageMetadata> metadata;
        if (gin::ImageMetadata::getFromImage (is, metadata))
        {
            for (auto* m : metadata)
            {
                juce::Array<juce::PropertyComponent*> comps;
                juce::StringPairArray values = m->getAllMetadata();

                for (auto key : values.getAllKeys())
                {
                    auto v = juce::Value (values [key]);
                    auto tpc = new juce::TextPropertyComponent (v, key, 1000, false, false);
                    comps.add (tpc);
                }

                panel.addSection (m->getType(), comps);
            }
        }
    }

    void resized()
    {
        panel.setBounds (getLocalBounds());
    }

    juce::PropertyPanel panel;
};

//==============================================================================
struct BmpImageDemo : public juce::Component
{
    BmpImageDemo()
    {
        setName ("BMP Image");

        gin::BMPImageFormat bmp;

        {
            juce::MemoryBlock mb (BinaryData::ballon_bmp, BinaryData::ballon_bmpSize);
            juce::MemoryInputStream is (mb, false);

            source1 = bmp.decodeImage (is);
        }
        {
            juce::MemoryBlock mb (BinaryData::ballon_8bit_bmp, BinaryData::ballon_8bit_bmpSize);
            juce::MemoryInputStream is (mb, false);

            source2 = bmp.decodeImage (is);
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto rc = getLocalBounds();

        g.fillAll (juce::Colours::black);
        if (source1.isValid())
            g.drawImage (source1, rc.removeFromLeft (rc.getWidth() / 2).toFloat(), juce::RectanglePlacement::centred);
        if (source2.isValid())
            g.drawImage (source2, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source1, source2;
};

//==============================================================================
struct WebpImageDemo : public juce::Component
{
    WebpImageDemo()
    {
        setName ("Webp Image");

        gin::WEBPImageFormat webp;

        {
            juce::MemoryBlock mb (BinaryData::_1_webp, BinaryData::_1_webpSize);
            juce::MemoryInputStream is (mb, false);

            if (webp.canUnderstand (is))
                source1 = webp.decodeImage (is);
        }
        {
            juce::MemoryBlock mb (BinaryData::_2_webp, BinaryData::_2_webpSize);
            juce::MemoryInputStream is (mb, false);

            if (webp.canUnderstand (is))
                source2 = webp.decodeImage (is);
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto rc = getLocalBounds();

        g.fillAll (juce::Colours::black);
        if (source1.isValid())
            g.drawImage (source1, rc.removeFromLeft (rc.getWidth() / 2).toFloat(), juce::RectanglePlacement::centred);
        if (source2.isValid())
            g.drawImage (source2, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source1, source2;
};


//==============================================================================
struct ImageEffectsDemo : public juce::Component,
                          private juce::Slider::Listener
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

        source = juce::ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        source = source.convertedToFormat (juce::Image::ARGB);

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
            if (auto* s = dynamic_cast<juce::Slider*> (getChildComponent (i)))
            {
                s->addListener (this);
                s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
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
            if (auto* s = dynamic_cast<juce::Slider*> (getChildComponent (i)))
                if (s->isVisible())
                    s->setBounds (rc.removeFromLeft (w));
    }

    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        effects.setBounds (5, 5, 150, 20);
    }

    void paint (juce::Graphics& g) override
    {
        auto img = source.createCopy();

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

        g.fillAll (juce::Colours::black);
        g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source;
    juce::ComboBox effects;

    juce::Slider vignetteAmount, vignetteRadius, vignetteFalloff, gamma, contrast, brightness, hue, saturation, lightness, radius;
};

//==============================================================================
struct MapDemo : public juce::Component
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
struct SemaphoreDemo : public juce::Component,
                       private juce::Button::Listener
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

    void buttonClicked (juce::Button* b) override
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

    juce::TextButton lockA {"Lock A"}, unlockA {"Unlock A"}, lockB {"Lock B"}, unlockB {"Unlock B"};
    gin::SystemSemaphore semA {"demo_sem"}, semB {"demo_sem"};
};

//==============================================================================
struct SharedMemoryDemo : public juce::Component,
                          private juce::TextEditor::Listener,
                          private juce::Timer
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

    void textEditorTextChanged (juce::TextEditor&) override
    {
        strncpy ((char*)mem.getData(), text.getText().toRawUTF8(), size_t (mem.getSize() - 1));
    }

    void timerCallback() override
    {
        if (! text.hasKeyboardFocus (true))
        {
            juce::String fromMem ((char*)mem.getData(), size_t (mem.getSize()));

            if (fromMem != text.getText())
                text.setText (fromMem);
        }
    }

    gin::SharedMemory mem {"demo", 1024};
    juce::TextEditor text;
};

//==============================================================================
struct LeastSquaresDemo : public juce::Component
{
    LeastSquaresDemo()
    {
        setName ("Least Squares");
    }

    void mouseDown (const juce::MouseEvent& e) override
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

    void paint (juce::Graphics& g) override
    {
        if (lsr.enoughPoints())
        {
            g.setColour (juce::Colours::red);

            juce::Path p;

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
            g.strokePath (p, juce::PathStrokeType (2));
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click to add point. Double click to reset.", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<int>> points;
    gin::LeastSquaresRegression lsr;
};

//==============================================================================
struct LinearDemo : public juce::Component
{
    LinearDemo()
    {
        setName ("Linear Regression");
    }

    void mouseDown (const juce::MouseEvent& e) override
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

    void paint (juce::Graphics& g) override
    {
        if (lr.haveData())
        {
            g.setColour (juce::Colours::red);

            juce::Path p;

            for (int x = 0; x < getWidth(); x++)
            {
                double y = lr.estimateY (x);
                if (x == 0)
                    p.startNewSubPath (float (x), float (y));
                else
                    p.lineTo (float (x), float (y));
            }
            g.strokePath (p, juce::PathStrokeType (2));
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click to add point. Double click to reset.", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<int>> points;
    gin::LinearRegression lr;
};

//==============================================================================
struct SplineDemo : public juce::Component
{
    SplineDemo()
    {
        setName ("Spline");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
            points.clear();
        else if (points.size() == 0 || e.x > points.getLast().getX())
            points.add (e.getPosition());

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        juce::Array<gin::Point<double>> dpoints;
        for (auto p : points)
            dpoints.add ({ double (p.getX()), double (p.getY())});

        if (dpoints.size() >= 3)
        {
            gin::Spline spline (dpoints);

            g.setColour (juce::Colours::red);

            juce::Path p;

            p.startNewSubPath (points.getFirst().toFloat());
            for (int x = points.getFirst().getX(); x < points.getLast().getX(); x++)
            {
                double y = spline.interpolate (x);
                p.lineTo (float (x), float (y));
            }
            p.lineTo (points.getLast().toFloat());

            g.strokePath (p, juce::PathStrokeType (2));
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click from left to right to add points. Double click to reset.", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<int>> points;
};

//==============================================================================
struct LagrangeDemo : public juce::Component
{
    LagrangeDemo()
    {
        setName ("Lagrange");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() > 1)
            points.clear();
        else if (points.size() == 0 || e.x > points.getLast().getX())
            points.add ({e.position.x, e.position.y});

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        juce::Array<gin::Point<double>> dpoints;
        for (auto p : points)
            dpoints.add ({ double (p.getX()), double (p.getY())});

        if (dpoints.size() >= 2)
        {
            g.setColour (juce::Colours::red);

            juce::Path p;

            p.startNewSubPath (points.getFirst().x, points.getFirst().y);
            for (float x = points.getFirst().getX(); x < points.getLast().getX(); x++)
            {
                float y = gin::Lagrange::interpolate (points, float (x));
                p.lineTo (x, y);
            }
            p.lineTo (points.getLast().x, points.getLast().y);

            g.strokePath (p, juce::PathStrokeType (2));
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click from left to right to add points. Double click to reset.", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<gin::Point<float>> points;
};

//==============================================================================
struct EllipseDemo : public juce::Component
{
    EllipseDemo()
    {
        setName ("Ellipse");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (points.size() >= 2)
            points.clear();

        points.add ({e.position.x, e.position.y});

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bnds = getLocalBounds().toFloat();

        auto a = getWidth() / 2.0f;
        auto b = getHeight() / 2.0f;

        g.setColour (juce::Colours::green);
        g.drawEllipse (bnds.getCentreX() - a / 2, bnds.getCentreY() - b / 2, a, b, 2.0f);

        if (points.size() == 2)
        {
            g.drawLine (juce::Line<float> (points[0], points[1]), 2.0f);

            gin::Ellipse<float> ellipse (0, 0, a, b);
            auto hits = ellipse.findIntersections (points[0] - bnds.getCentre(), points[1] - bnds.getCentre());

            g.setColour (juce::Colours::red);
            for (auto h : hits)
                g.fillEllipse (h.x - 3.0f + bnds.getCentreX(), h.y - 3.0f + bnds.getCentreY(), 6.0f, 6.0f);
        }

        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.isEmpty())
            g.drawText ("Click twice to add a line, click again to start a new line", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<float>> points;
};

//==============================================================================
struct CatenaryDemo : public juce::Component
{
    CatenaryDemo()
    {
        setName ("Catenary");
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (points.size() >= 2)
            points.clear();

        points.add ({e.position.x, e.position.y});

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::yellow);
        for (auto pt : points)
            g.fillEllipse (pt.getX() - 3.0f, pt.getY() - 3.0f, 6.0f, 6.0f);

        if (points.size() == 2)
        {
            auto h = float (getHeight());
            auto p1 = points[0];
            auto p2 = points[1];

            if (p1.x > p2.x)
                std::swap (p1, p2);

            gin::Catenary catenary (p1.x, h - p1.y, p2.x, h - p2.y, juce::Line<float> (p1, p2).getLength() * 0.2f);

            juce::Path p;
            p.startNewSubPath (p1.x, p1.y);

            for (auto x = p1.x + 2.0f; x < p2.x - 2.0f; x += 2.0f)
                p.lineTo (x, h - catenary.calcY (x));
            p.lineTo (p2.x, p2.y);

            g.strokePath (p, juce::PathStrokeType (1.5f));
        }

        if (points.isEmpty())
            g.drawText ("Click twice to add a wire, click again to start a new wire", getLocalBounds(), juce::Justification::centred);
    }

    juce::Array<juce::Point<float>> points;
};

//==============================================================================
struct WavetableDemo : public juce::Component
{
    WavetableDemo()
    {
        setName ("Wavetable");

        juce::MemoryBlock mb (BinaryData::Analog_PWM_Saw_01_wav, BinaryData::Analog_PWM_Saw_01_wavSize);
        juce::AudioSampleBuffer buffer;

        if (int size = gin::getWavetableSize (mb); size > 0)
        {
            auto is = new juce::MemoryInputStream (mb.getData(), mb.getSize(), false);
            auto reader = std::unique_ptr<juce::AudioFormatReader> (juce::WavAudioFormat().createReaderFor (is, true));

            if (reader)
            {
                buffer.setSize (1, int (reader->lengthInSamples));
                reader->read (&buffer, 0, int (reader->lengthInSamples), 0, true, false);

                loadWavetables (bllt, reader->sampleRate, buffer, reader->sampleRate, size);

                osc = std::make_unique<gin::WTOscillator> ();
                osc->setWavetable (&bllt);
            }
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto note = gin::getMidiNoteFromHertz (44.1f);

        juce::AudioSampleBuffer buf (2, 1024);

        auto rc = getLocalBounds().withTrimmedLeft (bllt.size()).withTrimmedBottom (bllt.size());

        int step = 4;
        for (int i = bllt.size(); i >= 0; i -= step)
        {
            gin::WTOscillator::Params params;
            params.position = float (i) / bllt.size();

            buf.clear();

            osc->noteOn (0.0f);
            osc->process (note, params, buf);

            juce::Path p;
            auto d = buf.getReadPointer (0);
            for (auto x = 0; x < 1024; x++)
            {
                auto fx = x / 1024.0f * rc.getWidth() + rc.getX();
                auto fy = -d[x] * rc.getHeight() / 2.0f + rc.getCentreY();

                if (x == 0)
                    p.startNewSubPath (fx, fy);
                else
                    p.lineTo (fx, fy);
            }

            g.setColour (juce::Colours::yellow.withAlpha (0.3f));
            g.strokePath (p, juce::PathStrokeType (1.0f));

            rc.setX (rc.getX() - step);
            rc.setY (rc.getY() + step);
        }
    }

    juce::OwnedArray<gin::BandLimitedLookupTable> bllt;
    std::unique_ptr<gin::WTOscillator> osc;
};

//==============================================================================
struct BLLTDemo : public juce::Component
{
    BLLTDemo()
    {
        setName ("BLLT");

        juce::AudioSampleBuffer buf (1, 2048);

        auto w = buf.getWritePointer (0);
        for (auto i = 0; i < 2048; i++)
            w[i] = tables.processSquare (0.0f, i / 2048.0f);

        std::unique_ptr<juce::dsp::FFT> fft;
        bllt.loadFromBuffer (fft, 44100, buf, 44100, 12);
    }

    void paint (juce::Graphics& g) override
    {

        //
        // From formula
        //
        {
            auto area = getLocalBounds();
            auto note = 0.5f;
            for (int i = 0; i < int (bllt.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / bllt.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = tables.processSquare (note, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }

                g.setColour (juce::Colours::green);
                g.strokePath (p, juce::PathStrokeType (1.0f));

                note += 12.0f;
            }
        }

        //
        // From wavefile
        //
        {
            auto area = getLocalBounds();
            for (int i = 0; i < int (bllt.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / bllt.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = bllt.get (i, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }
                g.setColour (juce::Colours::yellow);
                g.strokePath (p, juce::PathStrokeType (1.0f));
            }
        }
    }

    gin::BandLimitedLookupTables tables {44100, 12, 2048};
    gin::BandLimitedLookupTable bllt;
};

//==============================================================================
struct EquationParserDemo : public juce::Component
{
    EquationParserDemo()
    {
        setName ("Equation Parser");

        parser.defineNameChars ("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.");

        equation.setText ("2 + 2", juce::dontSendNotification);
        equation.onReturnKey = [this]
        {
            parser.setEquation (equation.getText());
            auto res = parser.evaluate();

            if (parser.hasError())
                result.setText (parser.getError(), juce::dontSendNotification);
            else
                result.setText (juce::String (res), juce::dontSendNotification);
        };
        addAndMakeVisible (equation);

        result.setReadOnly (true);
        addAndMakeVisible (result);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        equation.setBounds (rc.removeFromTop (25));
        rc.removeFromTop (8);
        result.setBounds (rc.removeFromTop (25));
    }

    gin::EquationParser parser;

    juce::TextEditor equation;
    juce::TextEditor result;
};

//==============================================================================
MainContentComponent::MainContentComponent()
{
    demoComponents.add (new EquationParserDemo());
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
