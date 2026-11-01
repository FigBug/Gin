/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Support for reading and writing Windows Bitmap files.

    Reads 1, 4, 8, 16, 24 and 32 bits per pixel, uncompressed or RLE4 or RLE8, with
    the channels laid out either the usual way or by the masks of a BI_BITFIELDS
    image, and rows running either way up. The OS/2 header and every version of the
    Windows one are understood. A BMP holding a whole JPEG or PNG is not.

    Always writes 32 bit uncompressed images.
 */
class BMPImageFormat : public juce::ImageFileFormat
{
public:
    juce::String getFormatName() override;

    bool canUnderstand (juce::InputStream& input) override;

    bool usesFileExtension (const juce::File& possibleFile) override;

    juce::Image decodeImage (juce::InputStream& input) override;

    bool writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& destStream) override;
};
