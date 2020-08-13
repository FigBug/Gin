/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class ImageMetadata
{
public:
    ImageMetadata (const juce::String& type);

    virtual ~ImageMetadata();

    juce::String getType() const { return type; }

    virtual juce::StringPairArray getAllMetadata() const = 0;

    static bool getFromImage (juce::InputStream& is, juce::OwnedArray<ImageMetadata>& metadata);

protected:
    juce::String type;
};
