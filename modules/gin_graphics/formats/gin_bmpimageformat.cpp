/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

struct BMPHeader
{
    uint16_t        magic;
    uint32_t        fileSize;
    uint16_t        reserved1;
    uint16_t        reserved2;
    uint32_t        dataOffset;
    uint32_t        headerSize;
    int32_t         width;
    int32_t         height;
    uint16_t        planes;
    uint16_t        bitsPerPixel;
    uint32_t        compression;
    uint32_t        imageDataSize;
    int32_t         hPixelsPerMeter;
    int32_t         vPixelsPerMeter;
    uint32_t        coloursUsed;
    uint32_t        coloursRequired;
};

juce::String BMPImageFormat::getFormatName()
{
    return "Windows Bitmap";
}

bool BMPImageFormat::canUnderstand (juce::InputStream& input)
{
    return input.readByte() == 'B' &&
           input.readByte() == 'M';
}

bool BMPImageFormat::usesFileExtension (const juce::File& possibleFile)
{
    return possibleFile.hasFileExtension ("bmp");
}

juce::Image BMPImageFormat::decodeImage (juce::InputStream& input)
{
    BMPHeader hdr;
    hdr.magic           = uint16_t (input.readShort());
    hdr.fileSize        = uint32_t (input.readInt());
    hdr.reserved1       = uint16_t (input.readShort());
    hdr.reserved2       = uint16_t (input.readShort());
    hdr.dataOffset      = uint32_t (input.readInt());
    hdr.headerSize      = uint32_t (input.readInt());
    hdr.width           = int32_t (input.readInt());
    hdr.height          = int32_t (input.readInt());
    hdr.planes          = uint16_t (input.readShort());
    hdr.bitsPerPixel    = uint16_t (input.readShort());
    hdr.compression     = uint32_t (input.readInt());
    hdr.imageDataSize   = uint32_t (input.readInt());
    hdr.hPixelsPerMeter = int32_t (input.readInt());
    hdr.vPixelsPerMeter = int32_t (input.readInt());
    hdr.coloursUsed     = uint32_t (input.readInt());
    hdr.coloursRequired = uint32_t (input.readInt());

    if (hdr.compression != 0 || (hdr.bitsPerPixel != 8 && hdr.bitsPerPixel != 24 && hdr.bitsPerPixel != 32))
    {
        jassertfalse; // Unsupported BMP format
        return juce::Image();
    }

    if (hdr.bitsPerPixel == 8 && hdr.coloursUsed == 0)
        hdr.coloursUsed = 256;

    juce::Array<juce::PixelARGB> colourTable;

    for (int i = 0; i < int (hdr.coloursUsed); i++)
    {
        auto b = uint8_t (input.readByte());
        auto g = uint8_t (input.readByte());
        auto r = uint8_t (input.readByte());
        input.readByte();

        colourTable.add (juce::PixelARGB (255, r, g, b));
    }

    bool bottomUp = hdr.height < 0;
    hdr.height = std::abs (hdr.height);

    juce::Image img (juce::Image::ARGB, int (hdr.width), int (hdr.height), true);
    juce::Image::BitmapData data (img, juce::Image::BitmapData::writeOnly);

    input.setPosition (hdr.dataOffset);

    int bytesPerPixel = hdr.bitsPerPixel / 8;
    int bytesPerRow = int (std::floor ((hdr.bitsPerPixel * hdr.width + 31) / 32.0) * 4);

    auto rowData = new uint8_t[size_t (bytesPerRow)];
    for (int y = 0; y < int (hdr.height); y++)
    {
        input.read (rowData, bytesPerRow);

        for (int x = 0; x < int (hdr.width); x++)
        {
            uint8_t* d = &rowData[x * bytesPerPixel];

            juce::PixelARGB* p = (juce::PixelARGB*)data.getPixelPointer (x, int (bottomUp ? y : hdr.height - y - 1));
            if (hdr.bitsPerPixel == 8)
                *p = colourTable[d[0]];
            else
                p->setARGB (bytesPerPixel == 4 ? d[3] : 255, d[2], d[1], d[0]);
        }
    }
    delete[] rowData;

    return img;
}

bool BMPImageFormat::writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& dst)
{
    juce::Image img = sourceImage.convertedToFormat (juce::Image::ARGB);

    dst.writeByte ('B');
    dst.writeByte ('M');
    dst.writeInt (40 + img.getWidth() * img.getHeight() * 4);
    dst.writeShort (0);
    dst.writeShort (0);
    dst.writeInt (54);
    dst.writeInt (40);
    dst.writeInt (img.getWidth());
    dst.writeInt (img.getHeight());
    dst.writeShort (1);
    dst.writeShort (32);
    dst.writeInt (0);
    dst.writeInt (img.getWidth() * img.getHeight() * 4);
    dst.writeInt (2835);
    dst.writeInt (2835);
    dst.writeInt (0);
    dst.writeInt (0);

    juce::Image::BitmapData data (img, juce::Image::BitmapData::readOnly);
    for (int y = 0; y < img.getHeight(); y++)
    {
        for (int x = 0; x < img.getWidth(); x++)
        {
            juce::PixelARGB* p = (juce::PixelARGB*)data.getPixelPointer (x, int (img.getHeight() - y - 1));
            dst.writeByte (char (p->getBlue()));
            dst.writeByte (char (p->getGreen()));
            dst.writeByte (char (p->getRed()));
            dst.writeByte (char (p->getAlpha()));
        }
    }

    return true;
}
