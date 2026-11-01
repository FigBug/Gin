/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Support for reading and writing Truevision TGA files.

    Reads colour mapped, true colour and greyscale images, each either uncompressed
    or run length encoded, at 8, 15, 16, 24 or 32 bits per pixel, with the rows and
    columns running whichever way the image descriptor says.

    Always writes 32 bit uncompressed true colour images.

    A TGA has no magic number at the front, so canUnderstand() looks for the file
    footer that version 2 of the format puts at the end, and failing that satisfies
    itself that the header makes sense. Give it a stream it can seek.
 */
class TGAImageFormat : public juce::ImageFileFormat
{
public:
    juce::String getFormatName() override;

    bool canUnderstand (juce::InputStream& input) override;

    bool usesFileExtension (const juce::File& possibleFile) override;

    juce::Image decodeImage (juce::InputStream& input) override;

    bool writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& destStream) override;
};
