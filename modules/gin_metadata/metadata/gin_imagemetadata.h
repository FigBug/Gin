/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Base class for reading metadata from image files (JPEG, PNG, GIF).

    ImageMetadata provides a polymorphic interface for accessing various types
    of image metadata (EXIF, XMP, IPTC, comments). Different metadata formats
    are handled by derived classes that parse format-specific data.

    The getFromImage() factory method automatically detects and extracts all
    available metadata types from an image file, returning them as an array
    of ImageMetadata objects.

    Supported metadata formats:
    - EXIF: Camera settings, GPS, timestamps
    - XMP: Adobe extensible metadata
    - IPTC: Press/journalism metadata
    - Comment: Simple text comments

    Usage:
    @code
    FileInputStream stream(imageFile);
    OwnedArray<ImageMetadata> metadata;

    if (ImageMetadata::getFromImage(stream, metadata))
    {
        for (auto* meta : metadata)
        {
            DBG("Metadata type: " + meta->getType());
            StringPairArray data = meta->getAllMetadata();
            for (auto& key : data.getAllKeys())
                DBG(key + ": " + data[key]);
        }
    }
    @endcode

    @see ExifMetadata, XmpMetadata, IptcMetadata, CommentMetadata
*/
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
