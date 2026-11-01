/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Support for reading and writing ZSoft PCX files.

    Reads 1, 2, 4 and 8 bits per plane across 1, 3 and 4 planes, which covers
    monochrome, the planar EGA layouts, 256 colour images with the VGA palette that
    trails the file, and 24 bit colour. Both the run length encoded and the rare
    uncompressed encodings are handled.

    Always writes 24 bit run length encoded images. PCX carries no alpha, so an image
    with transparency loses it.
 */
class PCXImageFormat : public juce::ImageFileFormat
{
public:
    juce::String getFormatName() override;

    bool canUnderstand (juce::InputStream& input) override;

    bool usesFileExtension (const juce::File& possibleFile) override;

    juce::Image decodeImage (juce::InputStream& input) override;

    bool writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& destStream) override;
};
