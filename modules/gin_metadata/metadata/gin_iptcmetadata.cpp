/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

juce::String IptcMetadata::MetadataItem::getName() const
{
    if (cat == 2)
    {
        switch (type)
        {
            case 0: return "Record Version";
            case 3: return "Object Type Reference";
            case 4: return "Object Attribute Reference";
            case 5: return "Object Name";
            case 7: return "Edit Status";
            case 8: return "Editorial Update";
            case 10: return "Urgency";
            case 12: return "Subject Reference";
            case 15: return "Category";
            case 20: return "Supplemental Category";
            case 22: return "Fixture Identifier";
            case 25: return "Keywords";
            case 26: return "Content Loctaion Code";
            case 27: return "Content Location Name";
            case 30: return "Release Date";
            case 35: return "Release Time";
            case 37: return "Expiration Date";
            case 38: return "Expiration Time";
            case 40: return "Special Instructions";
            case 42: return "Action Advised";
            case 45: return "Reference Service";
            case 47: return "Reference Date";
            case 50: return "Reference Number";
            case 55: return "Date Created";
            case 60: return "Time Created";
            case 62: return "Digital Creation Date";
            case 63: return "Digital Creation Time";
            case 65: return "Originating Program";
            case 70: return "Program Vesion";
            case 75: return "Object Cycle";
            case 80: return "By-line";
            case 85: return "By-line Title";
            case 90: return "City";
            case 92: return "Sub-location";
            case 95: return "Province/State";
            case 100: return "Country/Primary Location Code";
            case 101: return "Country/Primary Location Name";
            case 103: return "Original Transmission Reference";
            case 105: return "Headline";
            case 110: return "Credit";
            case 115: return "Source";
            case 116: return "Copyright Notice";
            case 118: return "Contact";
            case 120: return "Caption/Abstract";
            case 122: return "Writer/Editor";
            case 125: return "Rasterized Caption";
            case 130: return "Image Type";
            case 131: return "Image Orientation";
            case 135: return "Language Identifier";
            case 150: return "Audio Type";
            case 151: return "Audio Sampling Rate";
            case 152: return "Audio Sampling Resolution";
            case 153: return "Audio Duration";
            case 154: return "Audio Outcue";
            case 200: return "ObjectData Preview File Format";
            case 201: return "ObjectData Preview File Format Version";
            case 202: return "ObjectData Preview Data";
        }
    }
    return juce::String::formatted ("Unknown %d", type);
}

juce::String IptcMetadata::MetadataItem::getValue() const
{
    return data;
}

IptcMetadata::IptcMetadata() : ImageMetadata ("IPTC")
{
}

IptcMetadata* IptcMetadata::create (const juce::uint8* data, int sz)
{
    juce::MemoryInputStream is (data, size_t (sz), false);

    char header[14];
    is.read (header, 14);
    if (memcmp ("Photoshop 3.0", header, 14) != 0)
        return nullptr;

    IptcMetadata* md = new IptcMetadata();

    bool foundIptc = false;
    while (! is.isExhausted())
    {
        char bim[4];
        is.read(bim, 4);
        if (memcmp (bim, "8BIM", 4) != 0)
            break;

        short type = is.readShort();
        auto len  = juce::uint8 (is.readByte());
        if (len == 0)
            is.skipNextBytes (3);
        else
            is.skipNextBytes (len + 2);

        int size = (juce::uint8 (is.readByte()) << 8) + juce::uint8 (is.readByte());

        juce::int64 start = is.getPosition();
        if (type == 0x0404)
        {
            while (is.getPosition() < start + size)
            {
                auto marker = (juce::uint8) is.readByte();
                jassert (marker == 0x1C);
                juce::ignoreUnused (marker);

                int cat  = juce::uint8 (is.readByte());
                int kind = int (is.readByte());
                int szz  = int (juce::uint8 (is.readByte()) << 8) + juce::uint8 (is.readByte());

                char* newData = new char[size_t (size)];
                is.read (newData, size);

                MetadataItem* itm = new MetadataItem();
                itm->cat  = cat;
                itm->type = kind;
                itm->data = juce::String (newData, size_t (szz));
                md->items.add (itm);

                delete[] newData;
            }
            foundIptc = true;
        }
        else
        {
            is.skipNextBytes(size);
        }
    }
    if (foundIptc)
    {
        return md;
    }
    else
    {
        delete md;
        return nullptr;
    }
}

IptcMetadata::~IptcMetadata()
{
}

juce::StringPairArray IptcMetadata::getAllMetadata() const
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
