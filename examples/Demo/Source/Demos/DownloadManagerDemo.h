/*
  ==============================================================================

    DownloadManagerDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct DownloadManagerDemo : public DemoComponent,
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
