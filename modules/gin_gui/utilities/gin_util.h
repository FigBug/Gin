/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Returns the next colour in a set where the hues differ by the golden ratio.
    Good for coming up with a random set of colours
 */
juce::Colour goldenRatioColor (int idx);

//==============================================================================
/** Async Download. Doesn't have the main thread pause the URL::downloadToFile has
  */
class AsyncDownload : private juce::Thread,
                      private juce::AsyncUpdater
{
public:
    AsyncDownload (juce::String url_, std::function<void (AsyncDownload*, const juce::MemoryBlock&, bool)> cb_, int timeoutMS_ = 0)
      : Thread ("AsyncDownload"), url (url_), cb (cb_), timeoutMS (timeoutMS_)
    {
        startThread();
    }

    AsyncDownload (juce::URL url_, std::function<void (AsyncDownload*, const juce::MemoryBlock&, bool)> cb_, int timeoutMS_ = 0)
        : Thread ("AsyncDownload"), url (url_), cb (cb_), timeoutMS (timeoutMS_)
    {
        startThread();
    }

    ~AsyncDownload() override
    {
        stopThread (100);
    }

    void run() override
    {
        ok = readEntireBinaryStream (data);
        triggerAsyncUpdate();
    }

    bool readEntireBinaryStream (juce::MemoryBlock& destData, bool usePostCommand = false)
    {
        auto opts = juce::URL::InputStreamOptions (usePostCommand ? juce::URL::ParameterHandling::inPostData : juce::URL::ParameterHandling::inAddress).withConnectionTimeoutMs (timeoutMS);
        const std::unique_ptr<juce::InputStream> in (url.isLocalFile() ? url.getLocalFile().createInputStream() : url.createInputStream (opts));

        if (in != nullptr)
        {
            in->readIntoMemoryBlock (destData);
            return true;
        }

        return false;
    }

    void handleAsyncUpdate() override
    {
        if (cb)
            cb (this, data, ok);
    }

    juce::URL url;
    std::function<void (AsyncDownload*, juce::MemoryBlock, bool)> cb;
    int timeoutMS = 0;
    bool ok = false;
    juce::MemoryBlock data;
};

void asyncDownload (const juce::URL&, std::function<void (const juce::String&)>);
void asyncDownload (const juce::URL&, std::function<void (const juce::MemoryBlock&)>);
