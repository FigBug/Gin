/*
  ==============================================================================

    Demos.h
    Contains all demo components extracted from MainComponent.cpp

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

static juce::ThreadPool pool (juce::SystemStats::getNumCpus());

//==============================================================================
// Shared audio device manager for demos that need audio output
inline juce::AudioDeviceManager& getSharedAudioDeviceManager()
{
    static juce::AudioDeviceManager deviceManager;
    static bool initialised = false;

    if (! initialised)
    {
        initialised = true;
        deviceManager.initialiseWithDefaultDevices (0, 2);
    }

    return deviceManager;
}

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
struct TextRenderDemo final : public juce::Component
{
    TextRenderDemo()
    {
        setName ("Wave to Text Render");

        addAndMakeVisible (text);
        text.setFont (juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::plain)));
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

    void folderChanged (const juce::File& f) override
    {
        juce::Array<juce::File> files;
        f.findChildFiles (files, juce::File::findFiles, false);
        files.sort();

        contents.clear();

        juce::String txt;
        for (const auto& ff : files)
            txt += ff.getFileName() + "\n";

        contents.setText (txt);
    }

    void fileChanged (const juce::File& f, gin::FileSystemWatcher::FileSystemEvent fsEvent) override
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

                gin::loadWavetables (bllt, reader->sampleRate, buffer, reader->sampleRate, size);

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

    gin::Wavetable bllt;
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
        blltFFT.loadFromBuffer (fft, 44100, buf, 44100, 12);

        blltLowpass.loadFromBuffer (44100, buf, 44100, 44100.0f / 2048.0f, 12);
    }

    void paint (juce::Graphics& g) override
    {

        //
        // From formula
        //
        {
            auto area = getLocalBounds();
            auto note = 0.5f;
            for (int i = 0; i < int (blltFFT.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / blltFFT.tables.size()).reduced (3);

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
        // From wavefile FFT
        //
        {
            auto area = getLocalBounds();
            for (int i = 0; i < int (blltFFT.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / blltFFT.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = blltFFT.get (i, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }
                g.setColour (juce::Colours::yellow);
                g.strokePath (p, juce::PathStrokeType (1.0f));
            }
        }

        //
        // From wavefile Lowpass
        //
        {
            auto area = getLocalBounds();
            for (int i = 0; i < int (blltLowpass.tables.size()); i++)
            {
                auto rc = area.removeFromTop (getHeight() / blltLowpass.tables.size()).reduced (3);

                juce::Path p;

                for (auto x = 0; x < 2048; x++)
                {
                    auto fx = x / 2048.0f * rc.getWidth() + rc.getX();
                    auto fy = blltLowpass.get (i, x / 2048.0f) * rc.getHeight() / 2.0f + rc.getCentreY();

                    if (x == 0)
                        p.startNewSubPath (fx, fy);
                    else
                        p.lineTo (fx, fy);
                }
                g.setColour (juce::Colours::orange);
                g.strokePath (p, juce::PathStrokeType (1.0f));
            }
        }
    }

    gin::BandLimitedLookupTables tables {44100, 12, 2048};
    gin::BandLimitedLookupTable blltFFT;
    gin::BandLimitedLookupTable blltLowpass;
};

//==============================================================================
struct ComponentGridDemo : public juce::Component
{
    ComponentGridDemo()
        : horizontalGrid ("HorizontalGrid", gin::ComponentGrid::horizontal),
          verticalGrid ("VerticalGrid", gin::ComponentGrid::vertical)
    {
        setName ("Component Grid");

        // Setup horizontal grid
        horizontalGrid.setGap (8);
        addAndMakeVisible (horizontalGrid);

        // Add colorful buttons to horizontal grid
        for (int i = 0; i < 8; i++)
        {
            auto* btn = new juce::TextButton (juce::String (i + 1));
            btn->setSize (60, 40);
            btn->setColour (juce::TextButton::buttonColourId, juce::Colour::fromHSV (i / 8.0f, 0.7f, 0.8f, 1.0f));
            horizontalGrid.addAndMakeVisible (btn);
            horizontalButtons.add (btn);
        }

        // Setup vertical grid
        verticalGrid.setGap (8);
        addAndMakeVisible (verticalGrid);

        // Add colorful buttons to vertical grid
        for (int i = 0; i < 6; i++)
        {
            auto* btn = new juce::TextButton (juce::String::charToString ('A' + i));
            btn->setSize (80, 35);
            btn->setColour (juce::TextButton::buttonColourId, juce::Colour::fromHSV ((i + 4) / 10.0f, 0.6f, 0.9f, 1.0f));
            verticalGrid.addAndMakeVisible (btn);
            verticalButtons.add (btn);
        }

        // Add instruction label
        addAndMakeVisible (instructionLabel);
        instructionLabel.setText ("Drag and drop items to reorder them", juce::dontSendNotification);
        instructionLabel.setJustificationType (juce::Justification::centred);
        instructionLabel.setColour (juce::Label::textColourId, juce::Colours::white);

        // Add section labels
        addAndMakeVisible (horizontalLabel);
        horizontalLabel.setText ("Horizontal Grid:", juce::dontSendNotification);
        horizontalLabel.setColour (juce::Label::textColourId, juce::Colours::white);

        addAndMakeVisible (verticalLabel);
        verticalLabel.setText ("Vertical Grid:", juce::dontSendNotification);
        verticalLabel.setColour (juce::Label::textColourId, juce::Colours::white);

        // Setup callbacks
        horizontalGrid.onOrderChanged = [] (int oldIndex, int newIndex)
        {
            juce::ignoreUnused (oldIndex, newIndex);
            DBG ("Horizontal grid: moved item from " << oldIndex << " to " << newIndex);
        };

        verticalGrid.onOrderChanged = [] (int oldIndex, int newIndex)
        {
            juce::ignoreUnused (oldIndex, newIndex);
            DBG ("Vertical grid: moved item from " << oldIndex << " to " << newIndex);
        };
    }

    ~ComponentGridDemo() override
    {
        horizontalButtons.clear (true);
        verticalButtons.clear (true);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (20);

        // Instruction at top
        instructionLabel.setBounds (bounds.removeFromTop (25));
        bounds.removeFromTop (10);

        // Horizontal section
        horizontalLabel.setBounds (bounds.removeFromTop (20));
        bounds.removeFromTop (5);
        horizontalGrid.setBounds (bounds.removeFromTop (60));
        bounds.removeFromTop (30);

        // Vertical section
        verticalLabel.setBounds (bounds.removeFromTop (20));
        bounds.removeFromTop (5);
        verticalGrid.setBounds (bounds.removeFromLeft (100));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
    }

    gin::ComponentGrid horizontalGrid;
    gin::ComponentGrid verticalGrid;
    juce::OwnedArray<juce::TextButton> horizontalButtons;
    juce::OwnedArray<juce::TextButton> verticalButtons;
    juce::Label instructionLabel;
    juce::Label horizontalLabel;
    juce::Label verticalLabel;
};

//==============================================================================
struct EasingDemo : public juce::Component
{
    EasingDemo()
    {
        setName ("Easing");
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        // Create a list of all easing functions with their names
        struct EasingFunction
        {
            juce::String name;
            std::function<float(float)> func;
        };

        std::vector<EasingFunction> easingFunctions = {
            {"Linear", [](float p) { return gin::easeLinear(p); }},
            {"QuadIn", [](float p) { return gin::easeQuadraticIn(p); }},
            {"QuadOut", [](float p) { return gin::easeQuadraticOut(p); }},
            {"QuadInOut", [](float p) { return gin::easeQuadraticInOut(p); }},
            {"CubicIn", [](float p) { return gin::easeCubicIn(p); }},
            {"CubicOut", [](float p) { return gin::easeCubicOut(p); }},
            {"CubicInOut", [](float p) { return gin::easeCubicInOut(p); }},
            {"QuartIn", [](float p) { return gin::easeQuarticIn(p); }},
            {"QuartOut", [](float p) { return gin::easeQuarticOut(p); }},
            {"QuartInOut", [](float p) { return gin::easeQuarticInOut(p); }},
            {"QuintIn", [](float p) { return gin::easeQuinticIn(p); }},
            {"QuintOut", [](float p) { return gin::easeQuinticOut(p); }},
            {"QuintInOut", [](float p) { return gin::easeQuinticInOut(p); }},
            {"SineIn", [](float p) { return gin::easeSineIn(p); }},
            {"SineOut", [](float p) { return gin::easeSineOut(p); }},
            {"SineInOut", [](float p) { return gin::easeSineInOut(p); }},
            {"CircIn", [](float p) { return gin::easeCircularIn(p); }},
            {"CircOut", [](float p) { return gin::easeCircularOut(p); }},
            {"CircInOut", [](float p) { return gin::easeCircularInOut(p); }},
            {"ExpoIn", [](float p) { return gin::easeExponentialIn(p); }},
            {"ExpoOut", [](float p) { return gin::easeExponentialOut(p); }},
            {"ExpoInOut", [](float p) { return gin::easeExponentialInOut(p); }},
            {"ElasticIn", [](float p) { return gin::easeElasticIn(p); }},
            {"ElasticOut", [](float p) { return gin::easeElasticOut(p); }},
            {"ElasticInOut", [](float p) { return gin::easeElasticInOut(p); }},
            {"BackIn", [](float p) { return gin::easeBackIn(p); }},
            {"BackOut", [](float p) { return gin::easeBackOut(p); }},
            {"BackInOut", [](float p) { return gin::easeBackInOut(p); }},
            {"BounceIn", [](float p) { return gin::easeBounceIn(p); }},
            {"BounceOut", [](float p) { return gin::easeBounceOut(p); }},
            {"BounceInOut", [](float p) { return gin::easeBounceInOut(p); }}
        };

        // Calculate grid layout
        const int cols = 6;
        const int rows = (int(easingFunctions.size()) + cols - 1) / cols;

        const int cellWidth = getWidth() / cols;
        const int cellHeight = getHeight() / rows;
        const int padding = 10;
        const int textHeight = 15;

        // Draw each easing function
        for (size_t i = 0; i < easingFunctions.size(); i++)
        {
            int col = int(i) % cols;
            int row = int(i) / cols;

            juce::Rectangle<int> cell (col * cellWidth, row * cellHeight, cellWidth, cellHeight);
            juce::Rectangle<int> graphArea = cell.reduced (padding).withTrimmedBottom (textHeight);

            // Draw a subtle box around each cell
            g.setColour (juce::Colours::grey.withAlpha (0.3f));
            g.drawRect (cell.reduced (2));

            // Draw the easing curve
            juce::Path path;
            const int steps = 100;

            // First, build the path in normalized coordinates
            for (int step = 0; step <= steps; step++)
            {
                float t = step / float(steps);
                float easedValue = easingFunctions[i].func (t);

                if (step == 0)
                    path.startNewSubPath (t, easedValue);
                else
                    path.lineTo (t, easedValue);
            }

            // Get the actual bounds of the path (handles overshoot)
            auto pathBounds = path.getBounds();

            // Create transform to fit path into graphArea
            // Flip Y axis since screen coordinates go top-down
            auto transform = juce::AffineTransform()
                .translated (-pathBounds.getX(), -pathBounds.getY())  // Move to origin
                .scaled (graphArea.getWidth() / pathBounds.getWidth(),
                        -graphArea.getHeight() / pathBounds.getHeight())  // Scale and flip Y
                .translated (graphArea.getX(), graphArea.getBottom());  // Move to position

            path.applyTransform (transform);

            g.setColour (juce::Colours::cyan);
            g.strokePath (path, juce::PathStrokeType (2.0f));

            // Draw reference axes
            g.setColour (juce::Colours::grey.withAlpha (0.5f));
            g.drawRect (graphArea);

            // Draw label
            g.setColour (juce::Colours::white);
            g.setFont (12.0f);
            juce::Rectangle<int> textArea = cell.removeFromBottom (textHeight);
            g.drawText (easingFunctions[i].name, textArea, juce::Justification::centred);
        }
    }
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
juce::String layoutTxt = R"~~~(
{
   "components": [
       {
           "id": "a",
           "x": 10,
           "y": 10,
           "w": "parW - 20",
           "h": "(parH - 40) / 3",
           "components": [
               {
                   "id": "e",
                   "x": "10",
                   "y": "10",
                   "r": "parW - 10",
                   "h": "parH / 3"
               },
               {
                   "id": "f",
                   "x": "10",
                   "b": "parH - 10",
                   "r": "parW - 10",
                   "h": "parH / 3"
               }
           ]
       },
       {
           "id": "b",
           "x": 10,
           "y": "prevB + 10",
           "w": "parW - 20",
           "h": "(parH - 40) / 3",
           "components": [
               {
                   "id": "g",
                   "x": 10,
                   "y": 10,
                   "w": "parW / 3",
                   "b": "parH - 10"
               },
               {
                   "id": "h",
                   "r": "parW - 10",
                   "y": 10,
                   "w": "parW / 3",
                   "b": "parH - 10"
               }
           ]
       },
       {
           "id": "c",
           "x": 10,
           "y": "prevB + 10",
           "r": "parW - 10",
           "h": "(parH - 40) / 3",
           "components": [
               {
                   "grid": "c0/1fr,c1/3fr,c2/50px,c3/25px",
                   "bounds": "10,10,parW-20,parH-20",
                   "cols": 4,
                   "rows": 1,
                   "colGap": 1,
                   "rowGap": 0
               }
           ]
       }
   ]
}
)~~~";

struct LayoutDemo : public juce::Component,
                    private juce::CodeDocument::Listener
{
    LayoutDemo()
    {
        doc.addListener (this);
        setName ("Layout");

        auto a = new ColouredComponent ("a", juce::Colours::green);
        auto b = new ColouredComponent ("b", juce::Colours::green);
        auto c = new ColouredComponent ("c", juce::Colours::green);

        a->addAndMakeVisible (new ColouredComponent ("e", juce::Colours::yellow));
        a->addAndMakeVisible (new ColouredComponent ("f", juce::Colours::yellow));
        b->addAndMakeVisible (new ColouredComponent ("g", juce::Colours::yellow));
        b->addAndMakeVisible (new ColouredComponent ("h", juce::Colours::yellow));
        c->addAndMakeVisible (new ColouredComponent ("c0", juce::Colours::orange));
        c->addAndMakeVisible (new ColouredComponent ("c1", juce::Colours::cyan));
        c->addAndMakeVisible (new ColouredComponent ("c2", juce::Colours::magenta));
        c->addAndMakeVisible (new ColouredComponent ("c3", juce::Colours::red));

        layoutRoot.addAndMakeVisible (a);
        layoutRoot.addAndMakeVisible (b);
        layoutRoot.addAndMakeVisible (c);

        layoutJson.loadContent (layoutTxt);
        addAndMakeVisible (layoutJson);
        addAndMakeVisible (layoutRoot);
        addAndMakeVisible (error);

        error.setInterceptsMouseClicks (false, false);
        error.setJustificationType (juce::Justification::bottomLeft);
        error.setColour (juce::Label::textColourId, juce::Colours::red);
    }

    void codeDocumentTextInserted (const juce::String&, int) override
    {
        update();
    }

    void codeDocumentTextDeleted (int, int) override
    {
        update();
    }

    void update()
    {
        juce::var o;

        auto t = doc.getAllContent();
        auto e = juce::JSON::parse (t, o);

        if (e.wasOk())
        {
            resized();
            error.setText ({}, juce::dontSendNotification);
        }
        else
        {
            error.setText (e.getErrorMessage(), juce::dontSendNotification);
        }
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        layoutJson.setBounds (rc.removeFromLeft (rc.getWidth() / 2 - 4));
        error.setBounds (layoutJson.getBounds());
        rc.removeFromLeft (8);
        layoutRoot.setBounds (rc);

        layout.setLayout (doc.getAllContent());
    }

    class ColouredComponent : public juce::Component
    {
    public:
        ColouredComponent (const juce::String& n, const juce::Colour& c)
            : juce::Component (n), colour (c)
        {
        }

        ~ColouredComponent() override
        {
            deleteAllChildren();
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (colour.withMultipliedAlpha (0.8f));
            g.setColour (juce::Colours::white);
            g.drawText (getName(), getLocalBounds(), juce::Justification::centred);
        }

        juce::Colour colour;
    };

    juce::CodeDocument doc;
    juce::CodeEditorComponent layoutJson { doc, nullptr };
    ColouredComponent layoutRoot { "root", juce::Colours::blue };

    gin::LayoutSupport layout { layoutRoot };

    juce::Label error;
};

//==============================================================================
struct Wireframe3DDemo : public juce::Component,
                         private juce::Timer
{
    // Utah teapot - 306 control points for 32 Bezier patches (original Newell data)
    static constexpr float teapotVertices[][3] = {
        {1.4f,0.0f,2.4f}, {1.4f,-0.784f,2.4f}, {0.784f,-1.4f,2.4f}, {0.0f,-1.4f,2.4f},
        {1.3375f,0.0f,2.53125f}, {1.3375f,-0.749f,2.53125f}, {0.749f,-1.3375f,2.53125f}, {0.0f,-1.3375f,2.53125f},
        {1.4375f,0.0f,2.53125f}, {1.4375f,-0.805f,2.53125f}, {0.805f,-1.4375f,2.53125f}, {0.0f,-1.4375f,2.53125f},
        {1.5f,0.0f,2.4f}, {1.5f,-0.84f,2.4f}, {0.84f,-1.5f,2.4f}, {0.0f,-1.5f,2.4f},
        {-0.784f,-1.4f,2.4f}, {-1.4f,-0.784f,2.4f}, {-1.4f,0.0f,2.4f},
        {-0.749f,-1.3375f,2.53125f}, {-1.3375f,-0.749f,2.53125f}, {-1.3375f,0.0f,2.53125f},
        {-0.805f,-1.4375f,2.53125f}, {-1.4375f,-0.805f,2.53125f}, {-1.4375f,0.0f,2.53125f},
        {-0.84f,-1.5f,2.4f}, {-1.5f,-0.84f,2.4f}, {-1.5f,0.0f,2.4f},
        {-1.4f,0.784f,2.4f}, {-0.784f,1.4f,2.4f}, {0.0f,1.4f,2.4f},
        {-1.3375f,0.749f,2.53125f}, {-0.749f,1.3375f,2.53125f}, {0.0f,1.3375f,2.53125f},
        {-1.4375f,0.805f,2.53125f}, {-0.805f,1.4375f,2.53125f}, {0.0f,1.4375f,2.53125f},
        {-1.5f,0.84f,2.4f}, {-0.84f,1.5f,2.4f}, {0.0f,1.5f,2.4f},
        {0.784f,1.4f,2.4f}, {1.4f,0.784f,2.4f},
        {0.749f,1.3375f,2.53125f}, {1.3375f,0.749f,2.53125f},
        {0.805f,1.4375f,2.53125f}, {1.4375f,0.805f,2.53125f},
        {0.84f,1.5f,2.4f}, {1.5f,0.84f,2.4f},
        {1.75f,0.0f,1.875f}, {1.75f,-0.98f,1.875f}, {0.98f,-1.75f,1.875f}, {0.0f,-1.75f,1.875f},
        {2.0f,0.0f,1.35f}, {2.0f,-1.12f,1.35f}, {1.12f,-2.0f,1.35f}, {0.0f,-2.0f,1.35f},
        {2.0f,0.0f,0.9f}, {2.0f,-1.12f,0.9f}, {1.12f,-2.0f,0.9f}, {0.0f,-2.0f,0.9f},
        {-0.98f,-1.75f,1.875f}, {-1.75f,-0.98f,1.875f}, {-1.75f,0.0f,1.875f},
        {-1.12f,-2.0f,1.35f}, {-2.0f,-1.12f,1.35f}, {-2.0f,0.0f,1.35f},
        {-1.12f,-2.0f,0.9f}, {-2.0f,-1.12f,0.9f}, {-2.0f,0.0f,0.9f},
        {-1.75f,0.98f,1.875f}, {-0.98f,1.75f,1.875f}, {0.0f,1.75f,1.875f},
        {-2.0f,1.12f,1.35f}, {-1.12f,2.0f,1.35f}, {0.0f,2.0f,1.35f},
        {-2.0f,1.12f,0.9f}, {-1.12f,2.0f,0.9f}, {0.0f,2.0f,0.9f},
        {0.98f,1.75f,1.875f}, {1.75f,0.98f,1.875f},
        {1.12f,2.0f,1.35f}, {2.0f,1.12f,1.35f},
        {1.12f,2.0f,0.9f}, {2.0f,1.12f,0.9f},
        {2.0f,0.0f,0.45f}, {2.0f,-1.12f,0.45f}, {1.12f,-2.0f,0.45f}, {0.0f,-2.0f,0.45f},
        {1.5f,0.0f,0.225f}, {1.5f,-0.84f,0.225f}, {0.84f,-1.5f,0.225f}, {0.0f,-1.5f,0.225f},
        {1.5f,0.0f,0.15f}, {1.5f,-0.84f,0.15f}, {0.84f,-1.5f,0.15f}, {0.0f,-1.5f,0.15f},
        {-1.12f,-2.0f,0.45f}, {-2.0f,-1.12f,0.45f}, {-2.0f,0.0f,0.45f},
        {-0.84f,-1.5f,0.225f}, {-1.5f,-0.84f,0.225f}, {-1.5f,0.0f,0.225f},
        {-0.84f,-1.5f,0.15f}, {-1.5f,-0.84f,0.15f}, {-1.5f,0.0f,0.15f},
        {-2.0f,1.12f,0.45f}, {-1.12f,2.0f,0.45f}, {0.0f,2.0f,0.45f},
        {-1.5f,0.84f,0.225f}, {-0.84f,1.5f,0.225f}, {0.0f,1.5f,0.225f},
        {-1.5f,0.84f,0.15f}, {-0.84f,1.5f,0.15f}, {0.0f,1.5f,0.15f},
        {1.12f,2.0f,0.45f}, {2.0f,1.12f,0.45f},
        {0.84f,1.5f,0.225f}, {1.5f,0.84f,0.225f},
        {0.84f,1.5f,0.15f}, {1.5f,0.84f,0.15f},
        {-1.6f,0.0f,2.025f}, {-1.6f,-0.3f,2.025f}, {-1.5f,-0.3f,2.25f}, {-1.5f,0.0f,2.25f},
        {-2.3f,0.0f,2.025f}, {-2.3f,-0.3f,2.025f}, {-2.5f,-0.3f,2.25f}, {-2.5f,0.0f,2.25f},
        {-2.7f,0.0f,2.025f}, {-2.7f,-0.3f,2.025f}, {-3.0f,-0.3f,2.25f}, {-3.0f,0.0f,2.25f},
        {-2.7f,0.0f,1.8f}, {-2.7f,-0.3f,1.8f}, {-3.0f,-0.3f,1.8f}, {-3.0f,0.0f,1.8f},
        {-1.5f,0.3f,2.25f}, {-1.6f,0.3f,2.025f},
        {-2.5f,0.3f,2.25f}, {-2.3f,0.3f,2.025f},
        {-3.0f,0.3f,2.25f}, {-2.7f,0.3f,2.025f},
        {-3.0f,0.3f,1.8f}, {-2.7f,0.3f,1.8f},
        {-2.7f,0.0f,1.575f}, {-2.7f,-0.3f,1.575f}, {-3.0f,-0.3f,1.35f}, {-3.0f,0.0f,1.35f},
        {-2.5f,0.0f,1.125f}, {-2.5f,-0.3f,1.125f}, {-2.65f,-0.3f,0.9375f}, {-2.65f,0.0f,0.9375f},
        {-2.0f,-0.3f,0.9f}, {-1.9f,-0.3f,0.6f}, {-1.9f,0.0f,0.6f},
        {-3.0f,0.3f,1.35f}, {-2.7f,0.3f,1.575f},
        {-2.65f,0.3f,0.9375f}, {-2.5f,0.3f,1.125f},
        {-1.9f,0.3f,0.6f}, {-2.0f,0.3f,0.9f},
        {1.7f,0.0f,1.425f}, {1.7f,-0.66f,1.425f}, {1.7f,-0.66f,0.6f}, {1.7f,0.0f,0.6f},
        {2.6f,0.0f,1.425f}, {2.6f,-0.66f,1.425f}, {3.1f,-0.66f,0.825f}, {3.1f,0.0f,0.825f},
        {2.3f,0.0f,2.1f}, {2.3f,-0.25f,2.1f}, {2.4f,-0.25f,2.025f}, {2.4f,0.0f,2.025f},
        {2.7f,0.0f,2.4f}, {2.7f,-0.25f,2.4f}, {3.3f,-0.25f,2.4f}, {3.3f,0.0f,2.4f},
        {1.7f,0.66f,0.6f}, {1.7f,0.66f,1.425f},
        {3.1f,0.66f,0.825f}, {2.6f,0.66f,1.425f},
        {2.4f,0.25f,2.025f}, {2.3f,0.25f,2.1f},
        {3.3f,0.25f,2.4f}, {2.7f,0.25f,2.4f},
        {2.8f,0.0f,2.475f}, {2.8f,-0.25f,2.475f}, {3.525f,-0.25f,2.49375f}, {3.525f,0.0f,2.49375f},
        {2.9f,0.0f,2.475f}, {2.9f,-0.15f,2.475f}, {3.45f,-0.15f,2.5125f}, {3.45f,0.0f,2.5125f},
        {2.8f,0.0f,2.4f}, {2.8f,-0.15f,2.4f}, {3.2f,-0.15f,2.4f}, {3.2f,0.0f,2.4f},
        {3.525f,0.25f,2.49375f}, {2.8f,0.25f,2.475f},
        {3.45f,0.15f,2.5125f}, {2.9f,0.15f,2.475f},
        {3.2f,0.15f,2.4f}, {2.8f,0.15f,2.4f},
        {0.0f,0.0f,3.15f}, {0.0f,-0.002f,3.15f}, {0.002f,0.0f,3.15f},
        {0.8f,0.0f,3.15f}, {0.8f,-0.45f,3.15f}, {0.45f,-0.8f,3.15f}, {0.0f,-0.8f,3.15f},
        {0.0f,0.0f,2.85f},
        {0.2f,0.0f,2.7f}, {0.2f,-0.112f,2.7f}, {0.112f,-0.2f,2.7f}, {0.0f,-0.2f,2.7f},
        {-0.002f,0.0f,3.15f}, {-0.45f,-0.8f,3.15f}, {-0.8f,-0.45f,3.15f}, {-0.8f,0.0f,3.15f},
        {-0.112f,-0.2f,2.7f}, {-0.2f,-0.112f,2.7f}, {-0.2f,0.0f,2.7f},
        {0.0f,0.002f,3.15f}, {-0.8f,0.45f,3.15f}, {-0.45f,0.8f,3.15f}, {0.0f,0.8f,3.15f},
        {-0.2f,0.112f,2.7f}, {-0.112f,0.2f,2.7f}, {0.0f,0.2f,2.7f},
        {0.45f,0.8f,3.15f}, {0.8f,0.45f,3.15f},
        {0.112f,0.2f,2.7f}, {0.2f,0.112f,2.7f},
        {0.4f,0.0f,2.55f}, {0.4f,-0.224f,2.55f}, {0.224f,-0.4f,2.55f}, {0.0f,-0.4f,2.55f},
        {1.3f,0.0f,2.55f}, {1.3f,-0.728f,2.55f}, {0.728f,-1.3f,2.55f}, {0.0f,-1.3f,2.55f},
        {1.3f,0.0f,2.4f}, {1.3f,-0.728f,2.4f}, {0.728f,-1.3f,2.4f}, {0.0f,-1.3f,2.4f},
        {-0.224f,-0.4f,2.55f}, {-0.4f,-0.224f,2.55f}, {-0.4f,0.0f,2.55f},
        {-0.728f,-1.3f,2.55f}, {-1.3f,-0.728f,2.55f}, {-1.3f,0.0f,2.55f},
        {-0.728f,-1.3f,2.4f}, {-1.3f,-0.728f,2.4f}, {-1.3f,0.0f,2.4f},
        {-0.4f,0.224f,2.55f}, {-0.224f,0.4f,2.55f}, {0.0f,0.4f,2.55f},
        {-1.3f,0.728f,2.55f}, {-0.728f,1.3f,2.55f}, {0.0f,1.3f,2.55f},
        {-1.3f,0.728f,2.4f}, {-0.728f,1.3f,2.4f}, {0.0f,1.3f,2.4f},
        {0.224f,0.4f,2.55f}, {0.4f,0.224f,2.55f},
        {0.728f,1.3f,2.55f}, {1.3f,0.728f,2.55f},
        {0.728f,1.3f,2.4f}, {1.3f,0.728f,2.4f},
        {0.0f,0.0f,0.0f},
        {1.5f,0.0f,0.15f}, {1.5f,0.84f,0.15f}, {0.84f,1.5f,0.15f}, {0.0f,1.5f,0.15f},
        {1.5f,0.0f,0.075f}, {1.5f,0.84f,0.075f}, {0.84f,1.5f,0.075f}, {0.0f,1.5f,0.075f},
        {1.425f,0.0f,0.0f}, {1.425f,0.798f,0.0f}, {0.798f,1.425f,0.0f}, {0.0f,1.425f,0.0f},
        {-0.84f,1.5f,0.15f}, {-1.5f,0.84f,0.15f}, {-1.5f,0.0f,0.15f},
        {-0.84f,1.5f,0.075f}, {-1.5f,0.84f,0.075f}, {-1.5f,0.0f,0.075f},
        {-0.798f,1.425f,0.0f}, {-1.425f,0.798f,0.0f}, {-1.425f,0.0f,0.0f},
        {-1.5f,-0.84f,0.15f}, {-0.84f,-1.5f,0.15f}, {0.0f,-1.5f,0.15f},
        {-1.5f,-0.84f,0.075f}, {-0.84f,-1.5f,0.075f}, {0.0f,-1.5f,0.075f},
        {-1.425f,-0.798f,0.0f}, {-0.798f,-1.425f,0.0f}, {0.0f,-1.425f,0.0f},
        {0.84f,-1.5f,0.15f}, {1.5f,-0.84f,0.15f},
        {0.84f,-1.5f,0.075f}, {1.5f,-0.84f,0.075f},
        {0.798f,-1.425f,0.0f}, {1.425f,-0.798f,0.0f}
    };

    // 32 Bezier patches, each with 16 control point indices (1-based, will subtract 1)
    static constexpr int teapotPatches[32][16] = {
        {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
        {4,17,18,19,8,20,21,22,12,23,24,25,16,26,27,28},
        {19,29,30,31,22,32,33,34,25,35,36,37,28,38,39,40},
        {31,41,42,1,34,43,44,5,37,45,46,9,40,47,48,13},
        {13,14,15,16,49,50,51,52,53,54,55,56,57,58,59,60},
        {16,26,27,28,52,61,62,63,56,64,65,66,60,67,68,69},
        {28,38,39,40,63,70,71,72,66,73,74,75,69,76,77,78},
        {40,47,48,13,72,79,80,49,75,81,82,53,78,83,84,57},
        {57,58,59,60,85,86,87,88,89,90,91,92,93,94,95,96},
        {60,67,68,69,88,97,98,99,92,100,101,102,96,103,104,105},
        {69,76,77,78,99,106,107,108,102,109,110,111,105,112,113,114},
        {78,83,84,57,108,115,116,85,111,117,118,89,114,119,120,93},
        {121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136},
        {124,137,138,121,128,139,140,125,132,141,142,129,136,143,144,133},
        {133,134,135,136,145,146,147,148,149,150,151,152,69,153,154,155},
        {136,143,144,133,148,156,157,145,152,158,159,149,155,160,161,69},
        {162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177},
        {165,178,179,162,169,180,181,166,173,182,183,170,177,184,185,174},
        {174,175,176,177,186,187,188,189,190,191,192,193,194,195,196,197},
        {177,184,185,174,189,198,199,186,193,200,201,190,197,202,203,194},
        {204,204,204,204,207,208,209,210,211,211,211,211,212,213,214,215},
        {204,204,204,204,210,217,218,219,211,211,211,211,215,220,221,222},
        {204,204,204,204,219,224,225,226,211,211,211,211,222,227,228,229},
        {204,204,204,204,226,230,231,207,211,211,211,211,229,232,233,212},
        {212,213,214,215,234,235,236,237,238,239,240,241,242,243,244,245},
        {215,220,221,222,237,246,247,248,241,249,250,251,245,252,253,254},
        {222,227,228,229,248,255,256,257,251,258,259,260,254,261,262,263},
        {229,232,233,212,257,264,265,234,260,266,267,238,263,268,269,242},
        {270,270,270,270,279,280,281,282,275,276,277,278,271,272,273,274},
        {270,270,270,270,282,289,290,291,278,286,287,288,274,283,284,285},
        {270,270,270,270,291,298,299,300,288,295,296,297,285,292,293,294},
        {270,270,270,270,300,305,306,279,297,303,304,275,294,301,302,271}
    };

    // Evaluate a point on a Bezier patch at parameters (u, v)
    static gin::Vec3f evaluateBezierPatch (const int patchIndices[16], float u, float v)
    {
        // Bernstein polynomials for cubic Bezier
        auto bernstein = [] (float t, int i) -> float {
            float mt = 1.0f - t;
            switch (i) {
                case 0: return mt * mt * mt;
                case 1: return 3.0f * mt * mt * t;
                case 2: return 3.0f * mt * t * t;
                case 3: return t * t * t;
                default: return 0.0f;
            }
        };

        gin::Vec3f result { 0, 0, 0 };

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                int idx = patchIndices[i * 4 + j] - 1;  // Convert 1-based to 0-based
                if (idx >= 0 && idx < 306)
                {
                    float weight = bernstein (u, j) * bernstein (v, i);
                    result.x += teapotVertices[idx][0] * weight;
                    result.y += teapotVertices[idx][1] * weight;
                    result.z += teapotVertices[idx][2] * weight;
                }
            }
        }

        return result;
    }

    static void addTeapot (gin::Object3D& obj, float scale, juce::Colour colour, int subdivisions = 8)
    {
        float offsetZ = 1.5f;

        for (int p = 0; p < 32; ++p)
        {
            for (int i = 0; i < subdivisions; ++i)
            {
                for (int j = 0; j < subdivisions; ++j)
                {
                    float u0 = float (i) / float (subdivisions);
                    float u1 = float (i + 1) / float (subdivisions);
                    float v0 = float (j) / float (subdivisions);
                    float v1 = float (j + 1) / float (subdivisions);

                    auto p00 = evaluateBezierPatch (teapotPatches[p], u0, v0);
                    auto p10 = evaluateBezierPatch (teapotPatches[p], u1, v0);
                    auto p01 = evaluateBezierPatch (teapotPatches[p], u0, v1);

                    // Transform: swap Y and Z, center vertically, apply scale
                    gin::Vec3f v1pt { p00.x * scale, (p00.z - offsetZ) * scale, p00.y * scale };
                    gin::Vec3f v2pt { p10.x * scale, (p10.z - offsetZ) * scale, p10.y * scale };
                    gin::Vec3f v3pt { p01.x * scale, (p01.z - offsetZ) * scale, p01.y * scale };

                    // Add horizontal and vertical lines of the grid
                    obj.addLine (v1pt, v2pt, colour);
                    obj.addLine (v1pt, v3pt, colour);
                }
            }
        }
    }

    Wireframe3DDemo()
    {
        setName ("Wireframe 3D");

        // Set up the camera
        scene.getCamera().setPosition (0, 2, 6);
        scene.getCamera().setTarget (0, 0, 0);
        scene.getCamera().setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        // Add a grid
        auto* grid = scene.addObject();
        grid->addGrid (10.0f, 10, juce::Colours::grey.withAlpha (0.3f));

        // Add axes
        auto* axes = scene.addObject();
        axes->addAxes (2.0f);

        // Add the teapot
        teapotObj = scene.addObject();
        addTeapot (*teapotObj, 0.5f, juce::Colours::cyan);

        startTimerHz (60);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff1a1a2e));

        // Update camera aspect ratio
        scene.getCamera().setPerspective (45.0f, getWidth() / (float) getHeight(), 0.1f, 100.0f);

        // Render the scene
        renderer.render (g, scene, getLocalBounds());

        // Draw instructions
        g.setColour (juce::Colours::white.withAlpha (0.7f));
        g.setFont (12.0f);
        g.drawText ("Drag to orbit, scroll to zoom, shift+drag to pan",
                    getLocalBounds().removeFromBottom (25), juce::Justification::centred);
    }

    void timerCallback() override
    {
        // Auto-rotate the teapot
        if (! isDragging)
        {
            rotation += 0.01f;
            teapotObj->setTransform (gin::Mat4f::rotationY (rotation));
            repaint();
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        lastMousePos = e.position;
        isDragging = true;
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        isDragging = false;
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        auto delta = e.position - lastMousePos;
        lastMousePos = e.position;

        if (e.mods.isShiftDown())
        {
            // Pan
            scene.getCamera().pan (-delta.x * 0.01f, delta.y * 0.01f);
        }
        else
        {
            // Orbit
            scene.getCamera().orbit (-delta.x * 0.01f, -delta.y * 0.01f);
        }
        repaint();
    }

    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
    {
        scene.getCamera().zoom (wheel.deltaY * 2.0f);
        repaint();
    }

    gin::Scene3D scene;
    gin::Renderer3D renderer;
    gin::Object3D* teapotObj = nullptr;
    float rotation = 0.0f;
    juce::Point<float> lastMousePos;
    bool isDragging = false;
};

//==============================================================================
struct Wavetable3DDemo : public juce::Component,
                         private juce::Timer
{
    Wavetable3DDemo()
    {
        setName ("Wavetable 3D");

        // Set up the camera for a nice viewing angle
        scene.getCamera().setPosition (5, 3, 5);
        scene.getCamera().setTarget (0, 0, 0);
        scene.getCamera().setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        // Generate 64 wavetables morphing from sine to square
        generateWavetables();

        startTimerHz (60);
    }

    void generateWavetables()
    {
        gin::BandLimitedLookupTables tables { 44100, 12, 2048 };

        const float tableSpacing = 0.035f;

        juce::Colour colour = juce::Colours::green.withAlpha (0.6f);

        for (int t = 0; t < numTables; ++t)
        {
            auto* obj = scene.addObject();
            float morph = float (t) / float (numTables - 1);
            float zPos = (float (t) - float (numTables) / 2.0f) * tableSpacing;

            // Generate points for this wavetable by interpolating sine and square
            wavetablePoints[t].reserve (samplesPerTable);

            for (int i = 0; i < samplesPerTable; ++i)
            {
                float phase = float (i) / float (samplesPerTable);
                float xPos = (phase - 0.5f) * waveWidth;

                // Get sine and square values and interpolate
                float sineVal = tables.processSine (phase);
                float squareVal = tables.processSquare (60.0f, phase);  // Use middle note
                float yVal = (sineVal * (1.0f - morph) + squareVal * morph) * waveHeight;

                wavetablePoints[t].push_back ({ xPos, yVal, zPos });
            }

            // Connect points with lines (don't close the loop)
            for (int i = 0; i < samplesPerTable - 1; ++i)
                obj->addLine (wavetablePoints[t][size_t (i)], wavetablePoints[t][size_t (i + 1)], colour);
        }

        // Create the tracing point object
        tracingPointObj = scene.addObject();
        tracingPointObj->addPoint ({ 0, 0, 0 }, 8.0f, juce::Colours::yellow);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff1a1a2e));

        scene.getCamera().setPerspective (45.0f, getWidth() / (float) getHeight(), 0.1f, 100.0f);
        renderer.render (g, scene, getLocalBounds());

        g.setColour (juce::Colours::white.withAlpha (0.7f));
        g.setFont (12.0f);
        g.drawText ("Drag to orbit, scroll to zoom, shift+drag to pan",
                    getLocalBounds().removeFromBottom (25), juce::Justification::centred);
    }

    void timerCallback() override
    {
        // Update tracing point position (0.25 Hz per table)
        const float cyclesPerSecond = 0.25f;
        const float secondsPerTable = 1.0f / cyclesPerSecond;
        const float totalCycleTime = secondsPerTable * numTables;

        traceTime += 1.0f / 60.0f;  // Timer runs at 60 Hz
        if (traceTime >= totalCycleTime)
            traceTime -= totalCycleTime;

        // Calculate which table and which sample we're on
        float tableProgress = traceTime / secondsPerTable;
        int currentTable = int (tableProgress) % numTables;
        float sampleProgress = (tableProgress - float (currentTable)) * samplesPerTable;
        int sampleIndex = int (sampleProgress) % samplesPerTable;

        // Update the tracing point position and highlight current table
        if (tracingPointObj != nullptr && currentTable < numTables)
        {
            tracingPointObj->clear();
            tracingPointObj->addPoint (wavetablePoints[currentTable][size_t (sampleIndex)], 4.0f, juce::Colours::yellow);

            // Highlight the current wavetable in yellow
            if (currentTable != lastHighlightedTable)
            {
                // Reset previous table to green
                if (lastHighlightedTable >= 0)
                    updateTableColour (lastHighlightedTable, juce::Colours::green.withAlpha (0.6f));

                // Set current table to yellow
                updateTableColour (currentTable, juce::Colours::yellow);
                lastHighlightedTable = currentTable;
            }
        }

        if (! isDragging)
        {
            rotation += 0.005f;

            // Rotate all wavetable objects around Y axis
            for (size_t i = 0; i < scene.getNumObjects(); ++i)
                scene.getObject (i)->setTransform (gin::Mat4f::rotationY (rotation));
        }

        repaint();
    }

    void updateTableColour (int tableIndex, juce::Colour colour)
    {
        auto* obj = scene.getObject (size_t (tableIndex));
        if (obj == nullptr)
            return;

        obj->clear();
        for (int i = 0; i < samplesPerTable - 1; ++i)
            obj->addLine (wavetablePoints[tableIndex][size_t (i)], wavetablePoints[tableIndex][size_t (i + 1)], colour);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        lastMousePos = e.position;
        isDragging = true;
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        isDragging = false;
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        auto delta = e.position - lastMousePos;
        lastMousePos = e.position;

        if (e.mods.isShiftDown())
            scene.getCamera().pan (-delta.x * 0.01f, delta.y * 0.01f);
        else
            scene.getCamera().orbit (-delta.x * 0.01f, -delta.y * 0.01f);

        repaint();
    }

    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
    {
        scene.getCamera().zoom (wheel.deltaY * 2.0f);
        repaint();
    }

    static constexpr int numTables = 64;
    static constexpr int samplesPerTable = 128;
    static constexpr float waveHeight = 0.4f;
    static constexpr float waveWidth = 2.0f;

    gin::Scene3D scene;
    gin::Renderer3D renderer;
    gin::Object3D* tracingPointObj = nullptr;
    std::array<std::vector<gin::Vec3f>, numTables> wavetablePoints;
    float rotation = 0.0f;
    float traceTime = 0.0f;
    int lastHighlightedTable = -1;
    juce::Point<float> lastMousePos;
    bool isDragging = false;
};

//==============================================================================
struct SamplePlayerDemo : public juce::Component,
                          public juce::FileDragAndDropTarget,
                          public juce::AudioSource,
                          public juce::Timer
{
    SamplePlayerDemo()
    {
        setName ("Sample Player");

        addAndMakeVisible (waveform);
        waveform.setBackgroundColour (juce::Colours::black);
        waveform.setLineColour (juce::Colours::cyan);

        addAndMakeVisible (playButton);
        addAndMakeVisible (stopButton);

        playButton.onClick = [this]
        {
            samplePlayer.play();
            updateButtons();
        };

        stopButton.onClick = [this]
        {
            samplePlayer.stop();
            samplePlayer.setPosition (0.0);
            updateButtons();
        };

        addAndMakeVisible (scope);
        scope.setColour (gin::TriggeredScope::backgroundColourId, juce::Colours::black);
        scope.setColour (gin::TriggeredScope::lineColourId, juce::Colours::grey);
        scope.setColour (gin::TriggeredScope::traceColourId, juce::Colours::cyan);
        scope.setColour (gin::TriggeredScope::envelopeColourId, juce::Colours::cyan.withAlpha (0.5f));

        addAndMakeVisible (spectrum);
        spectrum.setColour (gin::SpectrumAnalyzer::backgroundColourId, juce::Colours::black);
        spectrum.setColour (gin::SpectrumAnalyzer::lineColourId, juce::Colours::grey);
        spectrum.setColour (gin::SpectrumAnalyzer::traceColourId, juce::Colours::orange);
        spectrum.setColour (gin::SpectrumAnalyzer::envelopeColourId, juce::Colours::orange.withAlpha (0.5f));

        addAndMakeVisible (xyScope);
        xyScope.setColour (gin::XYScope::backgroundColourId, juce::Colours::black);
        xyScope.setColour (gin::XYScope::lineColourId, juce::Colours::grey);
        xyScope.setColour (gin::XYScope::traceColourId, juce::Colours::limegreen);

        addAndMakeVisible (dropLabel);
        dropLabel.setText ("Drop audio file here", juce::dontSendNotification);
        dropLabel.setJustificationType (juce::Justification::centred);
        dropLabel.setColour (juce::Label::textColourId, juce::Colours::grey);

        scopeFifo.setSize (2, 44100);
        spectrumFifo.setSize (2, 44100);
        xyFifo.setSize (2, 44100);

        samplePlayer.setLooping (true);

        getSharedAudioDeviceManager().addAudioCallback (&audioSourcePlayer);
        audioSourcePlayer.setSource (this);

        startTimerHz (30);
        updateButtons();
    }

    ~SamplePlayerDemo() override
    {
        stopTimer();
        audioSourcePlayer.setSource (nullptr);
        getSharedAudioDeviceManager().removeAudioCallback (&audioSourcePlayer);
    }

    void timerCallback() override
    {
        if (wasPlaying && ! samplePlayer.isPlaying())
            updateButtons();

        wasPlaying = samplePlayer.isPlaying();

        if (samplePlayer.hasFileLoaded())
            waveform.setPlayheads ({ static_cast<int> (samplePlayer.getPosition()) });
    }

    void updateButtons()
    {
        playButton.setEnabled (samplePlayer.hasFileLoaded() && ! samplePlayer.isPlaying());
        stopButton.setEnabled (samplePlayer.isPlaying() || samplePlayer.getPosition() > 0.0);
        dropLabel.setVisible (! samplePlayer.hasFileLoaded());
    }

    // AudioSource
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        juce::ignoreUnused (samplesPerBlockExpected);
        samplePlayer.setPlaybackSampleRate (sampleRate);
        spectrum.setSampleRate (sampleRate);

        scopeFifo.setSize (2, int (sampleRate));
        spectrumFifo.setSize (2, int (sampleRate));
        xyFifo.setSize (2, int (sampleRate));
    }

    void releaseResources() override {}

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();

        samplePlayer.processBlock (*bufferToFill.buffer);

        // Feed visualizers
        if (scopeFifo.getFreeSpace() >= bufferToFill.numSamples)
            scopeFifo.write (*bufferToFill.buffer);

        if (spectrumFifo.getFreeSpace() >= bufferToFill.numSamples)
            spectrumFifo.write (*bufferToFill.buffer);

        if (bufferToFill.buffer->getNumChannels() >= 2 && xyFifo.getFreeSpace() >= bufferToFill.numSamples)
            xyFifo.write (*bufferToFill.buffer);
    }

    // FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray& files) override
    {
        juce::AudioFormatManager mgr;
        mgr.registerBasicFormats();

        for (const auto& file : files)
            if (mgr.findFormatForFileExtension (juce::File (file).getFileExtension()) != nullptr)
                return true;

        return false;
    }

    void fileDragEnter (const juce::StringArray&, int, int) override
    {
        isDragOver = true;
        repaint();
    }

    void fileDragExit (const juce::StringArray&) override
    {
        isDragOver = false;
        repaint();
    }

    void filesDropped (const juce::StringArray& files, int, int) override
    {
        isDragOver = false;

        juce::AudioFormatManager mgr;
        mgr.registerBasicFormats();

        for (const auto& file : files)
        {
            juce::File f (file);
            if (mgr.findFormatForFileExtension (f.getFileExtension()) != nullptr)
            {
                samplePlayer.load (f);
                waveform.setBuffer (samplePlayer.getBuffer());
                updateButtons();
                break;
            }
        }

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black.brighter (0.1f));

        if (isDragOver)
        {
            g.setColour (juce::Colours::cyan.withAlpha (0.2f));
            g.fillRect (waveform.getBounds());
        }
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        auto topRow = rc.removeFromTop (80);
        waveform.setBounds (topRow);
        dropLabel.setBounds (topRow);

        rc.removeFromTop (8);

        auto buttonRow = rc.removeFromTop (24);
        playButton.setBounds (buttonRow.removeFromLeft (60));
        buttonRow.removeFromLeft (8);
        stopButton.setBounds (buttonRow.removeFromLeft (60));

        rc.removeFromTop (8);

        auto visRow = rc;
        int visWidth = (visRow.getWidth() - 16) / 3;

        scope.setBounds (visRow.removeFromLeft (visWidth));
        visRow.removeFromLeft (8);
        spectrum.setBounds (visRow.removeFromLeft (visWidth));
        visRow.removeFromLeft (8);

        // XY scope should be square
        int xySize = std::min (visRow.getWidth(), visRow.getHeight());
        xyScope.setBounds (visRow.removeFromLeft (xySize).withHeight (xySize));
    }

    gin::SamplePlayer samplePlayer;
    gin::AudioSamplerBufferComponent waveform;
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::Label dropLabel;

    gin::AudioFifo scopeFifo { 2, 44100 };
    gin::AudioFifo spectrumFifo { 2, 44100 };
    gin::AudioFifo xyFifo { 2, 44100 };

    gin::TriggeredScope scope { scopeFifo };
    gin::SpectrumAnalyzer spectrum { spectrumFifo };
    gin::XYScope xyScope { xyFifo };

    juce::AudioSourcePlayer audioSourcePlayer;

    bool isDragOver = false;
    bool wasPlaying = false;
};
