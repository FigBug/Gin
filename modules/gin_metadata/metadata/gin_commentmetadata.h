/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Reads simple text comment metadata from image files.

    CommentMetadata extracts basic comment fields that some image formats support.
    This is the simplest form of metadata, typically just a single text string
    describing or annotating the image.

    JPEG comment segments and similar simple metadata formats are handled by
    this class. Unlike EXIF, XMP, or IPTC, comments don't have structured fields.

    Usage:
    @code
    FileInputStream stream(imageFile);
    OwnedArray<ImageMetadata> metadata;
    ImageMetadata::getFromImage(stream, metadata);

    for (auto* meta : metadata)
    {
        if (auto* comment = dynamic_cast<CommentMetadata*>(meta))
        {
            StringPairArray data = comment->getAllMetadata();
            String commentText = data["Comment"];
        }
    }
    @endcode

    @see ImageMetadata, ExifMetadata
*/
class CommentMetadata : public ImageMetadata
{
public:
    CommentMetadata();
    virtual ~CommentMetadata();

    static CommentMetadata* create (const juce::uint8* data, int sz);

    juce::StringPairArray getAllMetadata() const;

private:
    juce::String comment;
};
