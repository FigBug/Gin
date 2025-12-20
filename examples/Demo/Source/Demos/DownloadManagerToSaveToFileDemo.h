/*
  ==============================================================================

    DownloadManagerToSaveToFileDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct DownloadManagerToSaveToFileDemo : public DemoComponent,
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
