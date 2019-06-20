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

    static ExifMetadata* create (const uint8* data, int sz);

    StringPairArray getAllMetadata() const;

    Image getThumbnailImage();

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
        uint8* data;

        String getName() const;
        String getValue() const;

        uint16 swap16 (uint16 a) const;
        int16 swap16 (int16 a) const;
        uint32 swap32 (uint32 a) const;
        int32 swap32 (int32 a) const;
    };

    struct MetadataSection
    {
        MetadataSection (int id_, int offset_) { id = id_; offset = offset_; }

        int id;
        int offset;
    };

    OwnedArray<MetadataItem> items;
    char* thumbImg;
    int thumbNumBytes;
};
