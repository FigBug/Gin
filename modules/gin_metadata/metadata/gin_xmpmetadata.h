/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class XmpMetadata : public ImageMetadata
{
public:
    XmpMetadata();
    virtual ~XmpMetadata();

    static XmpMetadata* createFromJpg (const juce::uint8* data, int sz);
    static XmpMetadata* createFromPng (const char* data, int sz);

    juce::StringPairArray getAllMetadata() const;

private:
    juce::String xmp;
};
