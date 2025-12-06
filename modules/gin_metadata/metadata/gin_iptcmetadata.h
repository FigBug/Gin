/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Reads IPTC (International Press Telecommunications Council) metadata from images.

    IptcMetadata extracts press and journalism-specific metadata commonly used
    by news organizations, photo agencies, and professional photographers.
    IPTC data includes captions, keywords, copyright, and byline information.

    Common IPTC fields:
    - Caption/Description: Image description
    - Keywords: Search terms
    - Byline: Photographer/creator name
    - BylineTitle: Creator's job title
    - Credit: Provider attribution
    - Source: Original source
    - Copyright Notice: Copyright information
    - Headline: Brief title
    - Category: Subject category

    Usage:
    @code
    FileInputStream stream(imageFile);
    OwnedArray<ImageMetadata> metadata;
    ImageMetadata::getFromImage(stream, metadata);

    for (auto* meta : metadata)
    {
        if (auto* iptc = dynamic_cast<IptcMetadata*>(meta))
        {
            StringPairArray data = iptc->getAllMetadata();
            String caption = data["Caption"];
            String byline = data["Byline"];
            String copyright = data["Copyright Notice"];
            String keywords = data["Keywords"];
        }
    }
    @endcode

    @see ImageMetadata, XmpMetadata, ExifMetadata
*/
class IptcMetadata : public ImageMetadata
{
public:
    IptcMetadata();
    virtual ~IptcMetadata();

    static IptcMetadata* create(const juce::uint8* data, int sz);

    juce::StringPairArray getAllMetadata() const;

private:
    struct MetadataItem
    {
        int cat;
        int type;
        juce::String data;

        juce::String getName() const;
        juce::String getValue() const;
    };
    juce::OwnedArray<MetadataItem> items;
};
