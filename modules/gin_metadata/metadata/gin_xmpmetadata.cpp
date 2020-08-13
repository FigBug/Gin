/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

XmpMetadata::XmpMetadata() : ImageMetadata ("Xmp")
{
}

XmpMetadata* XmpMetadata::createFromJpg (const juce::uint8* data, int sz)
{
    juce::MemoryInputStream is (data, size_t (sz), false);

    char header[29];
    is.read (header, 29);
    if (memcmp ("http://ns.adobe.com/xap/1.0/", header, 29) != 0)
        return nullptr;

    XmpMetadata* md = new XmpMetadata();

    md->xmp = is.readEntireStreamAsString();

    return md;
}

XmpMetadata* XmpMetadata::createFromPng (const char* data, int sz)
{
    juce::MemoryInputStream is (data, (size_t) sz, false);
    XmpMetadata* md = new XmpMetadata();
    md->xmp = is.readEntireStreamAsString();
    return md;
}

XmpMetadata::~XmpMetadata()
{
}

juce::StringPairArray XmpMetadata::getAllMetadata() const
{
    juce::StringPairArray s;
    s.set ("XMP", xmp);
    return s;
}
