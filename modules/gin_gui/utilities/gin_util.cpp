/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

juce::Colour goldenRatioColor (int idx)
{
    double h = std::fmod (idx * 0.618033988749895, 1);
    return juce::Colour (float (h), 0.8f, 0.95f, 1.0f);
}

void asyncDownload (const juce::URL& url, std::function<void (const juce::String&)> callback)
{
    new AsyncDownload (url, [callback] (AsyncDownload* dl, const juce::MemoryBlock& mb, bool ok)
    {
        if (ok)
            callback (mb.toString());
        else
            callback ({});

        delete dl;
    });
}

void asyncDownload (const juce::URL& url, std::function<void (const juce::MemoryBlock&)> callback)
{
    new AsyncDownload (url, [callback] (AsyncDownload* dl, const juce::MemoryBlock& mb, bool ok)
    {
        if (ok)
            callback (mb);
        else
            callback ({});

        delete dl;
    });
}
