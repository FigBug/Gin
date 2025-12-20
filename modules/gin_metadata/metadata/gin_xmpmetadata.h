/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Reads XMP (Extensible Metadata Platform) metadata from JPEG and PNG images.

    XmpMetadata extracts Adobe's XMP data, which provides a standardized way to
    store metadata across different file formats. XMP is commonly used by Adobe
    products and professional photography workflows for storing rich metadata.

    XMP can contain:
    - Dublin Core metadata (title, creator, description, keywords)
    - Rights management information
    - Adobe-specific data (Lightroom, Photoshop settings)
    - Custom metadata namespaces
    - IPTC Core information

    The metadata is stored as XML and this class parses it into key-value pairs.

    Usage:
    @code
    FileInputStream stream(imageFile);
    OwnedArray<ImageMetadata> metadata;
    ImageMetadata::getFromImage(stream, metadata);

    for (auto* meta : metadata)
    {
        if (auto* xmp = dynamic_cast<XmpMetadata*>(meta))
        {
            StringPairArray data = xmp->getAllMetadata();
            String title = data["dc:title"];
            String creator = data["dc:creator"];
            String keywords = data["dc:subject"];
        }
    }
    @endcode

    @see ImageMetadata, ExifMetadata, IptcMetadata
*/
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
