/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class ExifMetadata : public ImageMetadata
{
public:
    ExifMetadata();
    virtual ~ExifMetadata();

    static ExifMetadata* create (const juce::uint8* data, int sz);

    juce::StringPairArray getAllMetadata() const;

    juce::Image getThumbnailImage();

private:
    static int sizeofType (int type);

    struct MetadataItem
    {
        ~MetadataItem();

        int tag;
        int type;
        int count;
        int section;
        bool bigEndian;
        void* data = nullptr;

        juce::String getName() const;
        juce::String getValue() const;

        juce::uint16 swap16 (juce::uint16 a) const;
        juce::int16 swap16 (juce::int16 a) const;
        juce::uint32 swap32 (juce::uint32 a) const;
        juce::int32 swap32 (juce::int32 a) const;
    };

    struct MetadataSection
    {
        MetadataSection (int id_, int offset_) { id = id_; offset = offset_; }

        int id;
        int offset;
    };

    juce::OwnedArray<MetadataItem> items;
    char* thumbImg = nullptr;
    int thumbNumBytes = 0;
};
