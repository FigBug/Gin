/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

ExifMetadata::MetadataItem::~MetadataItem()
{
    ::operator delete (data);
}

juce::String ExifMetadata::MetadataItem::getName() const
{
    if (section == 0x8825)
    {
        switch (tag)
        {
            case 0: return "GPS Version ID";
            case 1: return "GPS Latitude Ref";
            case 2: return "GPS Latitude";
            case 3: return "GPS Longitude Ref";
            case 4: return "GPS Longitude";
            case 5: return "GPS Altitude Ref";
            case 6: return "GPS Altitude";
            case 7: return "GPS TimeStamp";
            case 8: return "GPS Satellites";
            case 9: return "GPS Status";
            case 10: return "GPS Measure Mode";
            case 11: return "GPS DOP";
            case 12: return "GPS Speed Ref";
            case 13: return "GPS Speed";
            case 14: return "GPS Track Ref";
            case 15: return "GPS Track";
            case 16: return "GPS Img Direction Ref";
            case 17: return "GPS Img Direction";
            case 18: return "GPS Map Datum";
            case 19: return "GPS Dest Latitude Ref";
            case 20: return "GPS Dest Latitude";
            case 21: return "GPS Dest Longitude Ref";
            case 22: return "GPS Dest Longitude";
            case 23: return "GPS Dest Bearing Ref";
            case 24: return "GPS Dest Bearing";
            case 25: return "GPS Dest Distance Ref";
            case 26: return "GPS Dest Distance";
            case 27: return "GPS Processing Method";
            case 28: return "GPS Area Information";
            case 29: return "GPS Date Stamp";
            case 30: return "GPS Differential";
            case 0x001f: return "GPS H Positioning Error";
        }
    }
    else
    {
        switch (tag)
        {
            case 1: return "Interop Index";
            case 2: return "Interop Version";
            case 256: return "Image Width";
            case 257: return "Image Length";
            case 258: return "Bits Per Sample";
            case 259: return "Compression";
            case 262: return "Photometric Interpretation";
            case 270: return "Image Description";
            case 271: return "Make";
            case 272: return "Model";
            case 273: return "Strip Offsets";
            case 274: return "Orientation";
            case 277: return "Samples Per Pixel";
            case 278: return "Rows Per Strip";
            case 279: return "Strip Byte Counts";
            case 282: return "X Resolution";
            case 283: return "Y Resolution";
            case 284: return "Planar Configuration";
            case 296: return "Resolution Unit";
            case 301: return "Transfer Function";
            case 305: return "Software";
            case 306: return "Date Time";
            case 315: return "Artist";
            case 318: return "White Point";
            case 319: return "Primary Chromaticities";
            case 529: return "YCbCr Coefficients";
            case 530: return "YCbCr SubSampling";
            case 531: return "YCbCr Positioning";
            case 532: return "Reference Black White";
            case 33432: return "Copyright";
            case 33434: return "Exposure Time";
            case 33437: return "F Number";
            case 34850: return "Exposure Program";
            case 34852: return "Spectral Sensitivity";
            case 34855: return "ISO Speed Ratings";
            case 34857: return "OECF";
            case 36864: return "EXIF Version";
            case 36867: return "Date Time Original";
            case 36868: return "Date Time Digitized";
            case 37121: return "Components Configuration";
            case 37122: return "Compressed Bits Per Pixel";
            case 37377: return "Shutter Speed Value";
            case 37378: return "Aperture Value";
            case 37379: return "Brightness Value";
            case 37380: return "Exposure Bias Value";
            case 37381: return "Max Aperture Value";
            case 37382: return "Subject Distance";
            case 37383: return "Metering Mode";
            case 37384: return "Light Source";
            case 37385: return "Flash";
            case 37386: return "Focal Length";
            case 37500: return "Maker Note";
            case 37510: return "User Comment";
            case 37520: return "Sub Sec Time";
            case 37521: return "Sub Sec Time Original";
            case 37522: return "Sub Sec Time Digitized";
            case 40960: return "Flashpix Version";
            case 40961: return "Color Space";
            case 40962: return "Pixel X Dimension";
            case 40963: return "Pixel Y Dimension";
            case 40964: return "Related Sound File";
            case 41483: return "Flash Energy";
            case 41484: return "Spatial Frequency Response";
            case 41486: return "Focalplane X Resolution";
            case 41487: return "Focalplane Y Resolution";
            case 41488: return "Focalplane Resolution Unit";
            case 41492: return "Subject Location";
            case 41493: return "Exposure Index";
            case 41495: return "Sensing Method";
            case 41728: return "File Source";
            case 41729: return "Scene Type";
            case 41730: return "CFAPattern";
            case 41985: return "Custom Image Processing";
            case 41986: return "Exposure Mode";
            case 41987: return "White Balance";
            case 41988: return "Digital Zoom Ratio";
            case 41989: return "Focal Length in 35 mm Film";
            case 41990: return "Scene Capture Type";
            case 41991: return "Gain Control";
            case 41992: return "Contrast";
            case 41993: return "Saturation";
            case 41994: return "Sharpness";
            case 41995: return "Device Settings Description";
            case 41996: return "Subject Distance Range";
            case 0xA432: return "Lens Info";
            case 0xA433: return "Lens Make";
            case 0xA434: return "Lens Model";
            case 0xA435: return "Lens Serial Number";
            case 0x9214: return "Subject Area";

        }
    }
    return juce::String::formatted("Unknown tag: %d %X", tag, tag);
}

juce::String ExifMetadata::MetadataItem::getValue() const
{
    juce::String s;

    if (type == 1)
    {
        for (int i = 0; i < count; i++)
            s += juce::String (((juce::uint8*)data)[i]) + " ";
        return s.trim();
    }
    else if (type == 2)
    {
        return juce::String ((char*)data);
    }
    else if (type == 3)
    {
        for (int i = 0; i < count; i++)
            s += juce::String (swap16 (((juce::uint16*)data)[i])) + " ";
        return s.trim();
    }
    else if (type == 4)
    {
        for (int i = 0; i < count; i++)
            s += juce::String (swap32 (((juce::uint32*)data)[i])) + " ";
        return s.trim();
    }
    else if (type == 5)
    {
        for (int i = 0; i < count; i++)
            s += juce::String (double (swap32 (((juce::uint32*)data)[i*2])) / double (swap32 (((juce::uint32*)data)[i*2+1]))) + " ";
        return s.trim();
    }
    else if (type == 7)
    {
        for (int i = 0; i < count; i++)
            s += juce::String::formatted ("0x%.2X ", ((juce::uint8*)data)[i]);
        return s.trim();
    }
    else if (type == 9)
    {
        for (int i = 0; i < count; i++)
            s += juce::String (swap32 (((juce::int32*)data)[i])) + " ";
        return s.trim();
    }
    else if (type == 10)
    {
        for (int i = 0; i < count; i++)
            s += juce::String (double (swap32 (((juce::int32*)data)[i*2])) / double (swap32 (((juce::int32*)data)[i*2+1]))) + " ";
        return s.trim();
    }
    jassertfalse;
    return s;
}

juce::uint16 ExifMetadata::MetadataItem::swap16 (juce::uint16 a) const
{
#ifdef JUCE_LITTLE_ENDIAN
    if (bigEndian)
        return juce::ByteOrder::bigEndianShort ((const char*)&a);
    else
        return a;
#else
    if (!bigEndian)
        return littleEndianShort ((const char*)&a);
    else
        return a;
#endif
}

juce::int16 ExifMetadata::MetadataItem::swap16 (juce::int16 a) const
{
#ifdef JUCE_LITTLE_ENDIAN
    if (bigEndian)
        return juce::int16 (juce::ByteOrder::bigEndianShort (&a));
    else
        return a;
#else
    if (! bigEndian)
        return littleEndianShort (&a);
    else
        return a;
#endif
}

juce::uint32 ExifMetadata::MetadataItem::swap32 (juce::uint32 a) const
{
#ifdef JUCE_LITTLE_ENDIAN
    if (bigEndian)
        return juce::ByteOrder::bigEndianInt (&a);
    else
        return a;
#else
    if (! bigEndian)
        return ByteOrder::littleEndianInt (&a);
    else
        return a;
#endif
}

juce::int32 ExifMetadata::MetadataItem::swap32 (juce::int32 a) const
{
#ifdef JUCE_LITTLE_ENDIAN
    if (bigEndian)
        return juce::int32 (juce::ByteOrder::bigEndianInt (&a));
    else
        return a;
#else
    if (! bigEndian)
        return ByteOrder::littleEndianInt (&a);
    else
        return a;
#endif
}


ExifMetadata::ExifMetadata() : ImageMetadata ("EXIF")
{
    thumbImg = nullptr;
    thumbNumBytes = 0;
}

ExifMetadata* ExifMetadata::create (const juce::uint8* data, int sz)
{
    if (sz < 14 || memcmp (data, "Exif", 4) != 0)
        return nullptr;

    juce::MemoryInputStream is (data + 6, size_t (sz - 6), false);

    char endianTag[2];
    is.read (endianTag, 2);

    ExifMetadata* md = new ExifMetadata();

    bool bigEndian = memcmp (endianTag, "MM", 2) == 0;

    is.skipNextBytes (2);

    int offset = bigEndian ? is.readIntBigEndian() : is.readInt();
    if (offset == 0)
    {
        delete md;
        return nullptr;
    }

    juce::OwnedArray<MetadataSection> ifd;
    ifd.add (new MetadataSection (0, offset));

    int thumbOffset = 0;
    int thumbSize    = 0;
    while (ifd.size() > 0)
    {
        MetadataSection* ms = ifd.getFirst();
        is.setPosition (ms->offset);

        int records = bigEndian ? is.readShortBigEndian() : is.readShort();
        for (int i = 0; i < records; i++)
        {
            auto itm = new MetadataItem();

            itm->tag       = juce::uint16 (bigEndian ? is.readShortBigEndian() : is.readShort());
            itm->type      = juce::uint16 (bigEndian ? is.readShortBigEndian() : is.readShort());
            itm->count     =               bigEndian ? is.readIntBigEndian()   : is.readInt();
            itm->bigEndian = bigEndian;
            itm->section   = ms->id;

            int offsetData = is.readInt();

            int off       = int (bigEndian ? juce::ByteOrder::bigEndianInt (&offsetData) : juce::ByteOrder::littleEndianInt (&offsetData));
            int dataBytes = itm->count * sizeofType (itm->type);

            itm->data = ::operator new (size_t (dataBytes));
            if (dataBytes <= 4)
            {
                memcpy (itm->data, &offsetData, size_t (dataBytes));
            }
            else
            {
                juce::int64 curPos = is.getPosition();
                is.setPosition (off);
                is.read (itm->data, dataBytes);
                is.setPosition (curPos);
            }

            if (itm->tag == 513)
            {
                thumbOffset = off;
                delete itm;
            }
            else if (itm->tag == 514)
            {
                thumbSize = off;
                delete itm;
            }
            else if (itm->tag == 0x8769 ||
                     itm->tag == 0x8825 ||
                     itm->tag == 0xA005)
            {
                ifd.add (new MetadataSection (itm->tag, off));
                delete itm;
            }
            else if (itm->tag == 37500)
            {
                // handle maker note
                delete itm;
            }
            else
            {
                md->items.add (itm);
            }
        }
        int off = bigEndian ? is.readIntBigEndian() : is.readInt();
        if (off)
            ifd.add (new MetadataSection (0, off));

        ifd.remove (0);
    }

    if (thumbOffset && thumbSize)
    {
        md->thumbImg = new char[size_t (thumbSize)];
        is.setPosition (thumbOffset);
        is.read (md->thumbImg, thumbSize);
        md->thumbNumBytes = thumbSize;
    }

    return md;
}

ExifMetadata::~ExifMetadata()
{
    delete[] thumbImg;
}

juce::StringPairArray ExifMetadata::getAllMetadata() const
{
    juce::StringPairArray s;

    for (int i = 0; i < items.size(); i++)
    {
        juce::String name = items[i]->getName();
        juce::String val  = items[i]->getValue();

        if (s[name].isEmpty())
            s.set (name, val);
    }

    return s;
}

juce::Image ExifMetadata::getThumbnailImage()
{
    if (thumbImg)
        return juce::ImageFileFormat::loadFrom (thumbImg, (size_t) thumbNumBytes);
    return {};
}

int ExifMetadata::sizeofType (int dataType)
{
    switch (dataType)
    {
        case 1:  return 1;
        case 2:  return 1;
        case 3:  return 2;
        case 4:  return 4;
        case 5:  return 8;
        case 7:  return 1;
        case 9:  return 4;
        case 10: return 8;
    }
    return 0;
}
