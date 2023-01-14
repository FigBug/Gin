/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Support for reading and writing Webp
 */
class WEBPImageFormat : public juce::ImageFileFormat
{
public:
    juce::String getFormatName() override;

    bool canUnderstand (juce::InputStream& input) override;
    bool usesFileExtension (const juce::File& possibleFile) override;
    juce::Image decodeImage (juce::InputStream& input) override;
    bool writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& destStream) override;
    
    float quality = 80.0f;  // Encoding quality: 0 - 100 (bad - good)
    bool loseless = false;  // Loseless output (ignores quality)
};
